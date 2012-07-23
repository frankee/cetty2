/*
 * Copyright (c) 2010-2011 frankee zhou (frankee.zhou at gmail dot com)
 *
 * Distributed under under the Apache License, version 2.0 (the "License").
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 */

#include <cetty/channel/ChannelPipeline.h>

#include <cetty/channel/Channel.h>
#include <cetty/channel/ChannelSink.h>

#include <cetty/channel/ChannelEvent.h>
#include <cetty/channel/MessageEvent.h>

#include <cetty/channel/ChannelPipelineException.h>
#include <cetty/channel/ChannelHandlerLifeCycleException.h>
#include <cetty/channel/ChannelHandlerContext.h>
#include <cetty/channel/ChannelOutboundHandler.h>

#include <cetty/logging/InternalLogger.h>
#include <cetty/logging/InternalLoggerFactory.h>

#include <cetty/util/Exception.h>

namespace cetty {
namespace channel {

using namespace cetty::util;

InternalLogger* ChannelPipeline::logger = NULL;

class DiscardingChannelSink : public ChannelSink {
public:
    DiscardingChannelSink(InternalLogger* logger) : logger(logger)  {}
    virtual ~DiscardingChannelSink() {}

    virtual void eventSunk(const ChannelPipeline& pipeline,
                           const ChannelEvent& e) {
        logger->warn(std::string("Not attached yet; discarding: ") + e.toString());
    }

    virtual void writeRequested(const ChannelPipeline& pipeline,
                                const MessageEvent& e) {
        logger->warn(std::string("Not attached yet; discarding: ") + e.toString());
    }

    virtual void stateChangeRequested(const ChannelPipeline& pipeline,
                                      const ChannelStateEvent& e) {
        logger->warn(std::string("Not attached yet; discarding: ") + e.toString());
    }

    virtual void exceptionCaught(const ChannelPipeline& pipeline,
                                 const ChannelEvent& e,
                                 const ChannelPipelineException& cause) {
        cause.rethrow();
    }

private:
    InternalLogger* logger;
};

class HeadHandler : public ChannelOutboundHandler {
public:
    HeadHandler() : sink(NULL) {}
    virtual ~HeadHandler() {}

    void beforeAdd(ChannelHandlerContext& ctx) {
        // NOOP
    }

    void afterAdd(ChannelHandlerContext& ctx) {
        // NOOP
    }

    void beforeRemove(ChannelHandlerContext& ctx) {
        // NOOP
    }

    void afterRemove(ChannelHandlerContext& ctx) {
        // NOOP
    }

    void bind(ChannelHandlerContext& ctx, const SocketAddress& localAddress, const ChannelFuturePtr& future) {
        if (sink) {
            sink->bind(localAddress, future);
        }
    }

    void connect(ChannelHandlerContext& ctx,
        const SocketAddress& remoteAddress,
        const SocketAddress& localAddress,
        const ChannelFuturePtr& future) {
            if (sink) {
                sink->connect(remoteAddress, localAddress, future);
            }
    }

    void disconnect(ChannelHandlerContext& ctx, const ChannelFuturePtr& future) {
        if (sink) {
            sink->disconnect(future);
        }
    }

    void close(ChannelHandlerContext& ctx, const ChannelFuturePtr& future) {
        if (sink) {
            sink->close(future);
        }
    }

    void flush(ChannelHandlerContext& ctx, const ChannelFuturePtr& future) {
        if (sink) {
            sink->flush(future);
        }
    }

    void exceptionCaught(ChannelHandlerContext& ctx, const ChannelException& cause) {
        ctx.fireExceptionCaught(cause);
    }

    void eventTriggered(ChannelHandlerContext& ctx, const ChannelEvent& evt) {
        ctx.fireEventTriggered(evt);
    }

private:
    ChannelSink* sink;
};


ChannelSinkPtr ChannelPipeline::discardingSink = new DiscardingChannelSink(ChannelPipeline::getLogger());

ChannelPipeline::ChannelPipeline()
    : sink(discardingSink),
      channel(NULL),
      head(NULL),
      tail(NULL),
      inboundHead(NULL),
      outboundHead(NULL) {
    if (NULL == logger) {
        logger = InternalLoggerFactory::getInstance("ChannelPipeline");
    }
}

const ChannelPtr& ChannelPipeline::getChannel() const {
    if (!this->channel) {
        throw IllegalStateException("channel has not been attached");
    }

    return this->channel;
}

const ChannelSinkPtr& ChannelPipeline::getSink() const {
    if (!this->sink) {
        return ChannelPipeline::discardingSink;
    }

    return this->sink;
}

void ChannelPipeline::attach(const ChannelPtr& channel, const ChannelSinkPtr& sink) {
    if (!channel) {
        throw NullPointerException("channel");
    }

    if (!sink) {
        throw NullPointerException("sink");
    }

    if (this->channel || this->sink != discardingSink) {
        throw IllegalStateException("attached already");
    }

    this->channel = channel;
    this->sink = sink;
}

void ChannelPipeline::detach() {
    this->channel = NULL;
    this->sink = NULL;
}

bool ChannelPipeline::isAttached() const {
    return (sink != discardingSink && sink);
}

void ChannelPipeline::addFirst(const std::string& name,
                                      const ChannelHandlerPtr& handler) {
    boost::lock_guard<boost::recursive_mutex> guard(mutex);

    if (!handler) { return; }

    if (name2ctx.empty()) {
        init(name, handler);
    }
    else {
        checkDuplicateName(name);
        DefaultChannelHandlerContext* oldHead = this->head;
        DefaultChannelHandlerContext* newHead =
            new DefaultChannelHandlerContext(*this, name, handler, NULL, oldHead);

        callBeforeAdd(newHead);

        oldHead->prev = newHead;
        this->head = newHead;

        // for upstream and downstream single list.
        if (newHead->canHandleUps) {
            updateUpstreamList();
        }

        if (newHead->canHandleDowns) {
            updateDownstreamList();
        }

        name2ctx[name] = newHead;

        callAfterAdd(newHead);
    }
}

void ChannelPipeline::addLast(const std::string& name,
                                     const ChannelHandlerPtr& handler) {
    boost::lock_guard<boost::recursive_mutex> guard(mutex);

    if (!handler) { return; }

    if (name2ctx.empty()) {
        init(name, handler);
    }
    else {
        checkDuplicateName(name);
        DefaultChannelHandlerContext* oldTail = this->tail;
        DefaultChannelHandlerContext* newTail =
            new DefaultChannelHandlerContext(*this, name, handler, oldTail, NULL);

        callBeforeAdd(newTail);

        oldTail->next = newTail;
        this->tail = newTail;

        // for upstream and downstream single list.
        if (newTail->canHandleUps) {
            updateUpstreamList();
        }

        if (newTail->canHandleDowns) {
            updateDownstreamList();
        }

        name2ctx[name] = newTail;

        callAfterAdd(newTail);
    }
}

void ChannelPipeline::addBefore(const std::string& baseName,
                                       const std::string& name,
                                       const ChannelHandlerPtr& handler) {
    boost::lock_guard<boost::recursive_mutex> guard(mutex);

    if (!handler) { return; }

    DefaultChannelHandlerContext* ctx = getContextOrDie(baseName);

    if (ctx == this->head) {
        addFirst(name, handler);
    }
    else {
        checkDuplicateName(name);
        DefaultChannelHandlerContext* newCtx =
            new DefaultChannelHandlerContext(*this, name, handler, ctx->prev, ctx);

        callBeforeAdd(newCtx);

        ctx->prev->next = newCtx;
        ctx->prev = newCtx;

        // for upstream and downstream single list.
        if (newCtx->canHandleUps) {
            updateUpstreamList();
        }

        if (newCtx->canHandleDowns) {
            updateDownstreamList();
        }

        name2ctx[name] = newCtx;

        callAfterAdd(newCtx);
    }
}

void ChannelPipeline::addAfter(const std::string& baseName,
                                      const std::string& name,
                                      const ChannelHandlerPtr& handler) {
    boost::lock_guard<boost::recursive_mutex> guard(mutex);

    if (!handler) { return; }

    DefaultChannelHandlerContext* ctx = getContextOrDie(baseName);

    if (ctx == this->tail) {
        addLast(name, handler);
    }
    else {
        checkDuplicateName(name);
        DefaultChannelHandlerContext* newCtx =
            new DefaultChannelHandlerContext(*this, name, handler, ctx, ctx->next);

        callBeforeAdd(newCtx);

        ctx->next->prev = newCtx;
        ctx->next = newCtx;

        // for upstream and downstream single list.
        if (newCtx->canHandleUps) {
            updateUpstreamList();
        }

        if (newCtx->canHandleDowns) {
            updateDownstreamList();
        }

        name2ctx[name] = newCtx;

        callAfterAdd(newCtx);
    }
}

void ChannelPipeline::remove(const ChannelHandlerPtr& handler) {
    boost::lock_guard<boost::recursive_mutex> guard(mutex);
    remove(getContextOrDie(handler));
}

ChannelHandlerPtr
ChannelPipeline::remove(const std::string& name) {
    boost::lock_guard<boost::recursive_mutex> guard(mutex);
    return remove(getContextOrDie(name));
}

ChannelHandlerPtr
ChannelPipeline::remove(DefaultChannelHandlerContext* ctx) {
    if (head == tail) {
        head = tail = NULL;
        name2ctx.clear();
    }
    else if (ctx == head) {
        return removeFirst();
    }
    else if (ctx == tail) {
        return removeLast();
    }
    else {
        callBeforeRemove(ctx);

        DefaultChannelHandlerContext* prev = ctx->prev;
        DefaultChannelHandlerContext* next = ctx->next;
        prev->next = next;
        next->prev = prev;

        // for upstream and downstream single list.
        if (ctx->canHandleUps) {
            updateUpstreamList();
        }

        if (ctx->canHandleDowns) {
            updateDownstreamList();
        }

        name2ctx.erase(ctx->getName());

        callAfterRemove(ctx);
    }

    ChannelHandlerPtr ptr = ctx->getHandler();
    delete ctx;
    return ptr;
}

ChannelHandlerPtr ChannelPipeline::removeFirst() {
    boost::lock_guard<boost::recursive_mutex> guard(mutex);

    if (name2ctx.empty()) {
        throw RangeException("name2ctx is empty");
    }

    DefaultChannelHandlerContext* oldHead = head;

    if (oldHead == NULL) {
        throw RangeException("head is null");
    }

    callBeforeRemove(oldHead);

    if (oldHead->next == NULL) {
        head = tail = NULL;
        name2ctx.clear();
    }
    else {
        oldHead->next->prev = NULL;
        head = oldHead->next;
        name2ctx.erase(oldHead->getName());
    }

    // for upstream and downstream single list.
    if (oldHead->canHandleUps) {
        updateUpstreamList();
    }

    if (oldHead->canHandleDowns) {
        updateDownstreamList();
    }

    callAfterRemove(oldHead);

    ChannelHandlerPtr ptr = oldHead->getHandler();
    delete oldHead;
    return ptr;
}

ChannelHandlerPtr ChannelPipeline::removeLast() {
    boost::lock_guard<boost::recursive_mutex> guard(mutex);

    if (name2ctx.empty()) {
        throw RangeException("name2ctx is empty");
    }

    DefaultChannelHandlerContext* oldTail = tail;

    if (oldTail == NULL) {
        throw RangeException("head is null");
    }

    callBeforeRemove(oldTail);

    if (oldTail->prev == NULL) {
        this->head = this->tail = NULL;
        name2ctx.clear();
    }
    else {
        oldTail->prev->next = NULL;
        this->tail = oldTail->prev;
        name2ctx.erase(oldTail->getName());
    }

    // for upstream and downstream single list.
    if (oldTail->canHandleUps) {
        updateUpstreamList();
    }

    if (oldTail->canHandleDowns) {
        updateDownstreamList();
    }

    callAfterRemove(oldTail);

    ChannelHandlerPtr ptr = oldTail->getHandler();
    delete oldTail;
    return ptr;
}

void ChannelPipeline::replace(const ChannelHandlerPtr& oldHandler,
                                     const std::string& newName,
                                     const ChannelHandlerPtr& newHandler) {
    boost::lock_guard<boost::recursive_mutex> guard(mutex);
    replace(getContextOrDie(oldHandler), newName, newHandler);
}

ChannelHandlerPtr
ChannelPipeline::replace(const std::string& oldName,
                                const std::string& newName,
                                const ChannelHandlerPtr& newHandler) {
    boost::lock_guard<boost::recursive_mutex> guard(mutex);
    return replace(getContextOrDie(oldName), newName, newHandler);
}

ChannelHandlerPtr
ChannelPipeline::replace(DefaultChannelHandlerContext* ctx,
                                const std::string& newName,
                                const ChannelHandlerPtr& newHandler) {
    ChannelHandlerPtr replacedHandle;

    if (!newHandler) { return replacedHandle; }

    if (ctx == head) {
        replacedHandle = removeFirst();
        addFirst(newName, newHandler);
    }
    else if (ctx == tail) {
        replacedHandle = removeLast();
        addLast(newName, newHandler);
    }
    else {
        bool sameName = (ctx->getName().compare(newName) == 0);

        if (!sameName) {
            checkDuplicateName(newName);
        }

        DefaultChannelHandlerContext* prev = ctx->prev;
        DefaultChannelHandlerContext* next = ctx->next;
        DefaultChannelHandlerContext* newCtx =
            new DefaultChannelHandlerContext(*this, newName, newHandler, prev, next);

        callBeforeRemove(ctx);
        callBeforeAdd(newCtx);

        prev->next = newCtx;
        next->prev = newCtx;

        // for upstream and downstream single list.
        if (newCtx->canHandleUps || ctx->canHandleUps) {
            updateUpstreamList();
        }

        if (newCtx->canHandleDowns || ctx->canHandleDowns) {
            updateDownstreamList();
        }

        if (!sameName) {
            name2ctx.erase(ctx->getName());
            name2ctx[newName] = newCtx;
        }

        ChannelHandlerLifeCycleException removeException;
        ChannelHandlerLifeCycleException addException;
        bool removed = false;

        try {
            callAfterRemove(ctx);
            removed = true;
        }
        catch (const ChannelHandlerLifeCycleException& e) {
            removeException = e;
        }

        bool added = false;

        try {
            callAfterAdd(newCtx);
            added = true;
        }
        catch (const ChannelHandlerLifeCycleException& e) {
            addException = e;
        }

        if (!removed || !added) {
            delete ctx;
        }

        if (!removed && !added) {
            logger->warn(removeException.what(), removeException);
            logger->warn(addException.what(), addException);
            throw ChannelHandlerLifeCycleException(
                std::string("Both ") +
                ctx->getHandler()->toString() +
                std::string(".afterRemove() and ") +
                newCtx->getHandler()->toString() +
                std::string(".afterAdd() failed; see logs."));
        }
        else if (!removed) {
            removeException.rethrow();
        }
        else if (!added) {
            addException.rethrow();
        }

        replacedHandle = ctx->getHandler();
    }

    delete ctx;
    return replacedHandle;
}

ChannelHandlerPtr ChannelPipeline::getFirst() const {
    boost::lock_guard<boost::recursive_mutex> guard(mutex);

    DefaultChannelHandlerContext* head = this->head;

    if (head == NULL) {
        return ChannelHandlerPtr();
    }

    return head->getHandler();
}

ChannelHandlerPtr ChannelPipeline::getLast() const {
    boost::lock_guard<boost::recursive_mutex> guard(mutex);

    DefaultChannelHandlerContext* tail = this->tail;

    if (tail == NULL) {
        return ChannelHandlerPtr();
    }

    return tail->getHandler();
}

ChannelHandlerPtr
ChannelPipeline::get(const std::string& name) const {
    boost::lock_guard<boost::recursive_mutex> guard(mutex);

    ContextMap::const_iterator itr = name2ctx.find(name);

    if (itr == name2ctx.end()) {
        return ChannelHandlerPtr();
    }

    return itr->second->getHandler();
}

ChannelHandlerContext*
ChannelPipeline::getContext(const std::string& name) const {
    boost::lock_guard<boost::recursive_mutex> guard(mutex);
    return getContextNoLock(name);
}

ChannelHandlerContext*
ChannelPipeline::getContext(const ChannelHandlerPtr& handler) const {
    boost::lock_guard<boost::recursive_mutex> guard(mutex);
    return getContextNoLock(handler);
}

ChannelPipeline::ChannelHandlers ChannelPipeline::getChannelHandles() const {
    ChannelHandlers map;

    if (name2ctx.empty()) {
        return map;
    }

    DefaultChannelHandlerContext* ctx = head;

    for (;;) {
        map.push_back(std::make_pair(ctx->getName(), ctx->getHandler()));
        ctx = ctx->next;

        if (ctx == NULL) {
            break;
        }
    }

    return map;
}

std::string ChannelPipeline::toString() const {
    std::string buf("ChannelPipeline {");
    buf.reserve(1024);

    DefaultChannelHandlerContext* ctx = this->head;

    for (;;) {
        buf.append("(");
        buf.append(ctx->getName());
        buf.append(" = ");
        buf.append(ctx->getHandler()->toString());
        buf.append(")");

        ctx = ctx->next;

        if (ctx == NULL) {
            break;
        }

        buf.append(", ");
    }

    buf.append(" }");
    return buf;
}

void ChannelPipeline::notifyHandlerException(const ChannelEvent& evt, const Exception& e) {
    const ExceptionEvent* exceptionEvt = dynamic_cast<const ExceptionEvent*>(&evt);

    if (exceptionEvt) {
        logger->warn(
            "An exception was thrown by a user handler \
                 while handling an exception event ( ... )", e);
        return;
    }

    ChannelPipelineException pe(e.getMessage(), e.getCode());

    try {
        sink->exceptionCaught(*this, evt, pe);
    }
    catch (const Exception& e1) {
        logger->warn("An exception was thrown by an exception handler.", e1);
    }
}

void ChannelPipeline::callBeforeAdd(ChannelHandlerContext* ctx) {
    LifeCycleAwareChannelHandler* h =
        dynamic_cast<LifeCycleAwareChannelHandler*>(ctx->getHandler().get());

    if (!h) {
        return;
    }

    try {
        h->beforeAdd(*ctx);
    }
    catch (const Exception& e) {
        throw ChannelHandlerLifeCycleException(
            ".beforeAdd() has thrown an exception; not adding.", e);
    }
}

void ChannelPipeline::callAfterAdd(ChannelHandlerContext* ctx) {
    LifeCycleAwareChannelHandlerPtr h =
        boost::dynamic_pointer_cast<LifeCycleAwareChannelHandler>(ctx->getHandler());

    if (!h) {
        return;
    }

    try {
        h->afterAdd(*ctx);
    }
    catch (const Exception& e) {
        logger->warn("call afterAdd has throw an exception, then try to remove", e);

        bool removed = false;

        try {
            remove((DefaultChannelHandlerContext*)ctx);
            removed = true;
        }
        catch (const Exception& e) {
            logger->warn(
                std::string("Failed to remove a handler: ").append(ctx->getName()), e);

            if (removed) {
                throw ChannelHandlerLifeCycleException(
                    ".afterAdd() has thrown an exception; removed.", e);
            }
            else {
                throw ChannelHandlerLifeCycleException(
                    ".afterAdd() has thrown an exception; also failed to remove.", e);
            }
        }
    }
}

void ChannelPipeline::callBeforeRemove(ChannelHandlerContext* ctx) {
    LifeCycleAwareChannelHandlerPtr h =
        boost::dynamic_pointer_cast<LifeCycleAwareChannelHandler>(ctx->getHandler());

    if (!h) { return; }

    try {
        h->beforeRemove(*ctx);
    }
    catch (const Exception& e) {
        throw ChannelHandlerLifeCycleException(
            ".beforeRemove() has thrown an exception; not removing.", e);
    }
}

void ChannelPipeline::callAfterRemove(ChannelHandlerContext* ctx) {
    LifeCycleAwareChannelHandlerPtr h =
        boost::dynamic_pointer_cast<LifeCycleAwareChannelHandler>(ctx->getHandler());

    if (!h) { return; }

    try {
        h->afterRemove(*ctx);
    }
    catch (const Exception& e) {
        throw ChannelHandlerLifeCycleException(
            ".afterRemove() has thrown an exception.", e);
    }
}

void ChannelPipeline::init(const std::string& name, const ChannelHandlerPtr& handler) {
    DefaultChannelHandlerContext* ctx =
        new DefaultChannelHandlerContext(*this, name, handler, NULL, NULL);

    callBeforeAdd(ctx);

    this->head = this->tail = ctx;

    // for upstream and downstream single list.
    if (ctx->canHandleUps) {
        inboundHead = ctx;
    }

    if (ctx->canHandleDowns) {
        outboundHead = ctx;
    }

    name2ctx.clear();
    name2ctx[name] = ctx;

    callAfterAdd(ctx);
}

void ChannelPipeline::checkDuplicateName(const std::string& name) {
    if (name2ctx.find(name) != name2ctx.end()) {
        throw InvalidArgumentException("Duplicate handler name.");
    }
}

DefaultChannelHandlerContext* ChannelPipeline::getContextOrDie(const std::string& name) {
    DefaultChannelHandlerContext* ctx = getContextNoLock(name);

    if (ctx == NULL) {
        throw RangeException(name);
    }
    else {
        return ctx;
    }
}

DefaultChannelHandlerContext* ChannelPipeline::getContextOrDie(const ChannelHandlerPtr& handler) {
    DefaultChannelHandlerContext* ctx = getContextNoLock(handler);

    if (ctx == NULL) {
        throw RangeException("found no handle context");
    }
    else {
        return ctx;
    }
}

DefaultChannelHandlerContext* ChannelPipeline::getContextNoLock(const ChannelHandlerPtr& handler) const {
    if (!handler) {
        throw NullPointerException("handler");
    }

    if (name2ctx.empty()) {
        return NULL;
    }

    DefaultChannelHandlerContext* ctx = head;

    for (;;) {
        if (ctx->getHandler() == handler) {
            return ctx;
        }

        ctx = ctx->next;

        if (ctx == NULL) {
            break;
        }
    }

    return NULL;
}

DefaultChannelHandlerContext* ChannelPipeline::getContextNoLock(const std::string& name) const {
    ContextMap::const_iterator itr = name2ctx.find(name);

    if (itr == name2ctx.end()) {
        return NULL;
    }

    return itr->second;
}


void ChannelPipeline::sendUpstream(const ChannelEvent& e) {
    if (inboundHead) {
        try {
            inboundHead->upstreamHandler->handleUpstream(*inboundHead, e);
        }
        catch (const Exception& t) {
            notifyHandlerException(e, t);
        }
        catch (const std::exception& t) {
            notifyHandlerException(e, Exception(t.what()));
        }
        catch (...) {
            notifyHandlerException(e, Exception("unknown exception."));
        }
    }
    else {
        logger->warn("The pipeline contains no upstream handlers, discarding...");
    }
}

void ChannelPipeline::sendUpstream(const MessageEvent& e) {
    if (inboundHead) {
        try {
            inboundHead->upstreamHandler->messageReceived(*inboundHead, e);
        }
        catch (const Exception& t) {
            notifyHandlerException(e, t);
        }
        catch (const std::exception& t) {
            notifyHandlerException(e, Exception(t.what()));
        }
        catch (...) {
            notifyHandlerException(e, Exception("unknown exception."));
        }
    }
    else {
        logger->warn("The pipeline contains no upstream handlers, discarding...");
    }
}

void ChannelPipeline::sendUpstream(const ExceptionEvent& e) {
    if (inboundHead) {
        try {
            inboundHead->upstreamHandler->exceptionCaught(*inboundHead, e);
        }
        catch (const Exception& t) {
            notifyHandlerException(e, t);
        }
        catch (const std::exception& t) {
            notifyHandlerException(e, Exception(t.what()));
        }
        catch (...) {
            notifyHandlerException(e, Exception("unknown exception."));
        }
    }
    else {
        logger->warn("The pipeline contains no upstream handlers, discarding...");
    }
}

void ChannelPipeline::sendUpstream(const WriteCompletionEvent& e) {
    if (inboundHead) {
        try {
            inboundHead->upstreamHandler->writeCompleted(*inboundHead, e);
        }
        catch (const Exception& t) {
            notifyHandlerException(e, t);
        }
        catch (const std::exception& t) {
            notifyHandlerException(e, Exception(t.what()));
        }
        catch (...) {
            notifyHandlerException(e, Exception("unknown exception."));
        }
    }
    else {
        logger->warn("The pipeline contains no upstream handlers, discarding...");
    }
}

void ChannelPipeline::sendUpstream(const ChannelStateEvent& e) {
    if (inboundHead) {
        try {
            inboundHead->upstreamHandler->channelStateChanged(*inboundHead, e);
        }
        catch (const Exception& t) {
            notifyHandlerException(e, t);
        }
        catch (const std::exception& t) {
            notifyHandlerException(e, Exception(t.what()));
        }
        catch (...) {
            notifyHandlerException(e, Exception("unknown exception."));
        }
    }
    else {
        logger->warn("The pipeline contains no upstream handlers, discarding...");
    }
}

void ChannelPipeline::sendUpstream(const ChildChannelStateEvent& e) {
    if (inboundHead) {
        try {
            inboundHead->upstreamHandler->childChannelStateChanged(*inboundHead, e);
        }
        catch (const Exception& t) {
            notifyHandlerException(e, t);
        }
        catch (const std::exception& t) {
            notifyHandlerException(e, Exception(t.what()));
        }
        catch (...) {
            notifyHandlerException(e, Exception("unknown exception."));
        }
    }
    else {
        logger->warn("The pipeline contains no upstream handlers, discarding...");
    }
}

void ChannelPipeline::sendDownstream(const ChannelEvent& e) {
    try {
        if (outboundHead) {
            outboundHead->downstreamHandler->handleDownstream(*outboundHead, e);
        }
        else {
            BOOST_ASSERT(sink);
            sink->eventSunk(*this, e);
        }
    }
    catch (const Exception& t) {
        notifyHandlerException(e, t);
    }
    catch (const std::exception& t) {
        notifyHandlerException(e, Exception(t.what()));
    }
    catch (...) {
        notifyHandlerException(e, Exception("unknown exception."));
    }
}

void ChannelPipeline::sendDownstream(const MessageEvent& e) {
    try {
        if (outboundHead) {
            outboundHead->downstreamHandler->writeRequested(*outboundHead, e);
        }
        else {
            BOOST_ASSERT(sink);
            sink->writeRequested(*this, e);
        }
    }
    catch (const Exception& t) {
        notifyHandlerException(e, t);
    }
    catch (const std::exception& t) {
        notifyHandlerException(e, Exception(t.what()));
    }
    catch (...) {
        notifyHandlerException(e, Exception("unknown exception."));
    }
}

void ChannelPipeline::sendDownstream(const ChannelStateEvent& e) {
    try {
        if (outboundHead) {
            outboundHead->downstreamHandler->stateChangeRequested(*outboundHead, e);
        }
        else {
            BOOST_ASSERT(sink);
            sink->stateChangeRequested(*this, e);
        }
    }
    catch (const Exception& t) {
        notifyHandlerException(e, t);
    }
    catch (const std::exception& t) {
        notifyHandlerException(e, Exception(t.what()));
    }
    catch (...) {
        notifyHandlerException(e, Exception("unknown exception."));
    }
}

void ChannelPipeline::updateUpstreamList() {
    DefaultChannelHandlerContext* contextIndex = this->head;
    DefaultChannelHandlerContext* upstreamContextIndex = NULL;
    this->inboundHead = NULL;

    while (contextIndex) {
        if (contextIndex->canHandleUps) {
            if (!this->inboundHead) {
                this->inboundHead = contextIndex;
            }

            if (upstreamContextIndex) {
                upstreamContextIndex->nextUpstreamContext = contextIndex;
            }

            upstreamContextIndex = contextIndex;
        }

        contextIndex = contextIndex->next;
    }
}

void ChannelPipeline::updateDownstreamList() {
    DefaultChannelHandlerContext* contextIndex = this->tail;
    DefaultChannelHandlerContext* downstreamContextIndex = NULL;
    this->outboundHead = NULL;

    while (contextIndex) {
        if (contextIndex->canHandleDowns) {
            if (!this->outboundHead) {
                this->outboundHead = contextIndex;
            }

            if (downstreamContextIndex) {
                downstreamContextIndex->nextDownstreamContext = contextIndex;
            }

            downstreamContextIndex = contextIndex;
        }

        contextIndex = contextIndex->prev;
    }
}

void ChannelPipeline::setAttachment(const std::string& name, const boost::any& attachment) {
    attachments[name] = attachment;
}

boost::any ChannelPipeline::getAttachment(const std::string& name) const {
    AttachmentMap::const_iterator itr = attachments.find(name);

    if (itr != attachments.end()) {
        return itr->second;
    }

    return boost::any();
}


void ChannelPipeline::fireChannelCreated() {
    if (inboundHead) {
        inboundHead->fireChannelRegistered(*inboundHead);
    }
}

void ChannelPipeline::fireChannelActive() {
    firedChannelActive = true;
    head.fireChannelActive();
    if (fireInboundBufferUpdatedOnActivation) {
        fireInboundBufferUpdatedOnActivation = false;
        head.fireInboundBufferUpdated();
    }
}

void ChannelPipeline::fireChannelInactive() {
    // Some implementations such as EmbeddedChannel can trigger inboundBufferUpdated()
    // after deactivation, so it's safe not to revert the firedChannelActive flag here.
    // Also, all known transports never get re-activated.
    //firedChannelActive = false;
    outboundHead->fireChannelInactive(*outboundHead);
}

void ChannelPipeline::fireExceptionCaught(const ChannelException& cause) {
    head.fireExceptionCaught(cause);
}

void ChannelPipeline::fireEventTriggered(const ChannelEvent& event) {
    head.fireEventTriggered(event);
}

void ChannelPipeline::fireMessageUpdated() {
    if (!firedChannelActive) {
        fireInboundBufferUpdatedOnActivation = true;
        return;
    }
    head.fireInboundBufferUpdated();
}

ChannelFuturePtr ChannelPipeline::bind(const SocketAddress& localAddress) {
    return bind(localAddress, channel->newFuture());
}

const ChannelFuturePtr& ChannelPipeline::bind(const SocketAddress& localAddress, const ChannelFuturePtr& future) {
    if (outboundHead) {
        outboundHead->bind(*outboundHead, localAddress, future);
    }
}

ChannelFuturePtr ChannelPipeline::connect(const SocketAddress& remoteAddress) {
    return connect(remoteAddress, channel->newFuture());
}

ChannelFuturePtr ChannelPipeline::connect(const SocketAddress& remoteAddress, const SocketAddress& localAddress) {
    return connect(remoteAddress, localAddress, channel.newFuture());
}

const ChannelFuturePtr& ChannelPipeline::connect(const SocketAddress& remoteAddress, const ChannelFuturePtr& future) {
    return connect(remoteAddress, null, future);
}

const ChannelFuturePtr& ChannelPipeline::connect(const SocketAddress& remoteAddress, const SocketAddress& localAddress, const ChannelFuturePtr& future) {
    return connect(outboundHead, remoteAddress, localAddress, future);
}

ChannelFuturePtr ChannelPipeline::disconnect() {
    return disconnect(channel.newFuture());
}

const ChannelFuturePtr& ChannelPipeline::disconnect(const ChannelFuturePtr& future) {
    return disconnect(firstContext(DIR_OUTBOUND), future);
}


ChannelFuturePtr ChannelPipeline::close() {
    return close(channel.newFuture());
}

const ChannelFuturePtr& ChannelPipeline::close(const ChannelFuturePtr& future) {
    return close(firstContext(DIR_OUTBOUND), future);
}

ChannelFuturePtr ChannelPipeline::flush() {
    return flush(channel.newFuture());
}

const ChannelFuturePtr& ChannelPipeline::flush(const const ChannelFuturePtr&& future) {
    return flush(outboundHead, future);
}


ChannelFuturePtr ChannelPipeline::write(const ChannelMessage& message) {
    return write(message, channel.newFuture());
}

const ChannelFuturePtr& ChannelPipeline::write(Object message, const ChannelFuturePtr& future) {
    return write(tail, message, future);
}

}
}
