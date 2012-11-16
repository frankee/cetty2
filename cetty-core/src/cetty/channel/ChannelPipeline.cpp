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
#include <cetty/channel/SocketAddress.h>
#include <cetty/channel/ChannelFuture.h>
#include <cetty/channel/ChannelHandler.h>
#include <cetty/channel/ChannelHandlerContext.h>
#include <cetty/channel/ChannelInboundHandler.h>
#include <cetty/channel/ChannelOutboundHandler.h>
#include <cetty/channel/ChannelOutboundBufferHandler.h>
#include <cetty/channel/ChannelInboundBufferHandlerContext.h>
#include <cetty/channel/ChannelOutboundBufferHandlerContext.h>
#include <cetty/channel/ChannelPipelineException.h>
#include <cetty/channel/ChannelHandlerLifeCycleException.h>

#include <cetty/buffer/ChannelBuffer.h>
#include <cetty/buffer/Unpooled.h>

#include <cetty/logging/LoggerHelper.h>

#include <cetty/util/Exception.h>
#include <cetty/util/StlUtil.h>

namespace cetty {
namespace channel {

using namespace cetty::buffer;
using namespace cetty::util;

static const char* SINK_HANDLER_NAME = "_sink";

ChannelPipeline::ChannelPipeline()
    : firedChannelActive(false),
      fireInboundBufferUpdatedOnActivation(false),
      channel(),
      head(NULL),
      tail(NULL),
      inboundHead(NULL),
      outboundHead(NULL),
      receiveBuffer(Unpooled::buffer(1024*16)) {
}

ChannelPipeline::~ChannelPipeline() {
    //LOG_DEBUG << "ChannelPipeline dector";

    STLDeleteValues(&name2ctx);
}

void ChannelPipeline::attach(const ChannelPtr& channel) {
    if (!channel) {
        throw NullPointerException("channel");
    }

    if (!sinkHandler) {
        throw NullPointerException("should setSinkHandler first.");
    }

    if (this->channel) {
        throw IllegalStateException("attached already");
    }

    this->channel = channel;
    this->eventLoop = channel->getEventLoop();

    addFirst(SINK_HANDLER_NAME, sinkHandler);

    ChannelHandlerContext* ctx = head;

    while (true) {
        if (ctx == tail) {
            ctx->attach();
            break;
        }

        ctx->attach();
        ctx = ctx->next;
    }
}

void ChannelPipeline::detach() {
    ChannelHandlerContext* ctx = head;

    while (true) {
        if (ctx == tail) {
            ctx->detach();
            break;
        }

        ctx->detach();
        ctx = ctx->next;
    }

    this->channel.reset();
    this->eventLoop.reset();

    remove(SINK_HANDLER_NAME);
    this->sinkHandler.reset();
}

bool ChannelPipeline::isAttached() const {
    return !!channel;
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
        ChannelHandlerContext* oldHead = this->head;
        ChannelHandlerContext* newHead =
            handler->createContext(name, *this, NULL, oldHead);

        callBeforeAdd(newHead);

        oldHead->prev = newHead;
        this->head = newHead;

        // for upstream and downstream single list.
        if (newHead->canHandleInboundMessage()) {
            updateInboundHandlerContextList();
        }

        if (newHead->canHandleOutboundMessage()) {
            updateOutboundHandlerContextList();
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
        ChannelHandlerContext* oldTail = this->tail;
        ChannelHandlerContext* newTail =
            handler->createContext(name, *this, oldTail, NULL);

        callBeforeAdd(newTail);

        oldTail->next = newTail;
        this->tail = newTail;

        // for upstream and downstream single list.
        if (newTail->canHandleInboundMessage()) {
            updateInboundHandlerContextList();
        }

        if (newTail->canHandleOutboundMessage()) {
            updateOutboundHandlerContextList();
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

    ChannelHandlerContext* ctx = getContextOrDie(baseName);

    if (ctx == this->head) {
        addFirst(name, handler);
    }
    else {
        checkDuplicateName(name);
        ChannelHandlerContext* newCtx =
            handler->createContext(name, *this, ctx->prev, ctx);

        callBeforeAdd(newCtx);

        ctx->prev->next = newCtx;
        ctx->prev = newCtx;

        // for upstream and downstream single list.
        if (newCtx->canHandleInboundMessage()) {
            updateInboundHandlerContextList();
        }

        if (newCtx->canHandleOutboundMessage()) {
            updateOutboundHandlerContextList();
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

    ChannelHandlerContext* ctx = getContextOrDie(baseName);

    if (ctx == this->tail) {
        addLast(name, handler);
    }
    else {
        checkDuplicateName(name);
        ChannelHandlerContext* newCtx =
            handler->createContext(name, *this, ctx, ctx->next);

        callBeforeAdd(newCtx);

        ctx->next->prev = newCtx;
        ctx->next = newCtx;

        // for upstream and downstream single list.
        if (newCtx->canHandleInboundMessage()) {
            updateInboundHandlerContextList();
        }

        if (newCtx->canHandleOutboundMessage()) {
            updateOutboundHandlerContextList();
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
ChannelPipeline::remove(ChannelHandlerContext* ctx) {
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

        ChannelHandlerContext* prev = ctx->prev;
        ChannelHandlerContext* next = ctx->next;
        prev->next = next;
        next->prev = prev;

        // for upstream and downstream single list.
        if (ctx->canHandleInboundMessage()) {
            updateInboundHandlerContextList();
        }

        if (ctx->canHandleOutboundMessage()) {
            updateOutboundHandlerContextList();
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

    ChannelHandlerContext* oldHead = head;

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
    if (oldHead->canHandleInboundMessage()) {
        updateInboundHandlerContextList();
    }

    if (oldHead->canHandleOutboundMessage()) {
        updateOutboundHandlerContextList();
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

    ChannelHandlerContext* oldTail = tail;

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
    if (oldTail->canHandleInboundMessage()) {
        updateInboundHandlerContextList();
    }

    if (oldTail->canHandleOutboundMessage()) {
        updateOutboundHandlerContextList();
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
ChannelPipeline::replace(ChannelHandlerContext* ctx,
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

        ChannelHandlerContext* prev = ctx->prev;
        ChannelHandlerContext* next = ctx->next;
        ChannelHandlerContext* newCtx =
            newHandler->createContext(newName, *this, prev, next);

        callBeforeRemove(ctx);
        callBeforeAdd(newCtx);

        prev->next = newCtx;
        next->prev = newCtx;

        // for upstream and downstream single list.
        if (newCtx->canHandleInboundMessage() || ctx->canHandleInboundMessage()) {
            updateInboundHandlerContextList();
        }

        if (newCtx->canHandleOutboundMessage() || ctx->canHandleOutboundMessage()) {
            updateOutboundHandlerContextList();
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
            LOG_WARN_E(removeException);
            LOG_WARN_E(addException);
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

    ChannelHandlerContext* head = this->head;

    if (head == NULL) {
        return ChannelHandlerPtr();
    }

    return head->getHandler();
}

ChannelHandlerPtr ChannelPipeline::getLast() const {
    boost::lock_guard<boost::recursive_mutex> guard(mutex);

    ChannelHandlerContext* tail = this->tail;

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

    ChannelHandlerContext* ctx = head;

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

    ChannelHandlerContext* ctx = this->head;

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

void ChannelPipeline::notifyHandlerException(const Exception& e) {
    if (e.getNested()) {
        LOG_WARN << "exception was thrown by a user handler"
                 "while handling an exception ( ... )";
        return;
    }

    ChannelPipelineException pe(e.getMessage(), e.getCode());
    fireExceptionCaught(pe);
}

void ChannelPipeline::callBeforeAdd(ChannelHandlerContext* ctx) {
    try {
        ctx->getHandler()->beforeAdd(*ctx);
    }
    catch (const Exception& e) {
        throw ChannelHandlerLifeCycleException(
            ".beforeAdd() has thrown an exception; not adding.", e);
    }
}

void ChannelPipeline::callAfterAdd(ChannelHandlerContext* ctx) {
    try {
        ctx->getHandler()->afterAdd(*ctx);
    }
    catch (const Exception& e) {
        LOG_WARN_E(e) << "call afterAdd has throw an exception, then try to remove";

        bool removed = false;

        try {
            remove((ChannelHandlerContext*)ctx);
            removed = true;
        }
        catch (const Exception& e) {
            LOG_WARN_E(e) << "Failed to remove a handler: " << ctx->getName();

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
    try {
        ctx->getHandler()->beforeRemove(*ctx);
    }
    catch (const Exception& e) {
        throw ChannelHandlerLifeCycleException(
            ".beforeRemove() has thrown an exception; not removing.", e);
    }
}

void ChannelPipeline::callAfterRemove(ChannelHandlerContext* ctx) {
    try {
        ctx->getHandler()->afterRemove(*ctx);
    }
    catch (const Exception& e) {
        throw ChannelHandlerLifeCycleException(
            ".afterRemove() has thrown an exception.", e);
    }
}

void ChannelPipeline::init(const std::string& name, const ChannelHandlerPtr& handler) {
    ChannelHandlerContext* ctx =
        handler->createContext(name, *this, NULL, NULL);

    callBeforeAdd(ctx);

    this->head = this->tail = ctx;

    // for upstream and downstream single list.
    if (ctx->canHandleInboundMessage()) {
        inboundHead = ctx;
    }

    if (ctx->canHandleOutboundMessage()) {
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

ChannelHandlerContext* ChannelPipeline::getContextOrDie(const std::string& name) {
    ChannelHandlerContext* ctx = getContextNoLock(name);

    if (ctx == NULL) {
        throw RangeException(name);
    }
    else {
        return ctx;
    }
}

ChannelHandlerContext* ChannelPipeline::getContextOrDie(const ChannelHandlerPtr& handler) {
    ChannelHandlerContext* ctx = getContextNoLock(handler);

    if (ctx == NULL) {
        throw RangeException("found no handle context");
    }
    else {
        return ctx;
    }
}

ChannelHandlerContext* ChannelPipeline::getContextNoLock(const ChannelHandlerPtr& handler) const {
    if (!handler) {
        throw NullPointerException("handler");
    }

    if (name2ctx.empty()) {
        return NULL;
    }

    ChannelHandlerContext* ctx = head;

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

ChannelHandlerContext* ChannelPipeline::getContextNoLock(const std::string& name) const {
    ContextMap::const_iterator itr = name2ctx.find(name);

    if (itr == name2ctx.end()) {
        return NULL;
    }

    return itr->second;
}

void ChannelPipeline::updateInboundHandlerContextList() {
    ChannelHandlerContext* contextIndex = this->head;
    ChannelHandlerContext* inboundContextIndex = NULL;
    this->inboundHead = NULL;

    while (contextIndex) {
        if (contextIndex->canHandleInboundMessage()) {
            if (!this->inboundHead) {
                this->inboundHead = contextIndex;
            }

            if (inboundContextIndex) {
                inboundContextIndex->nextInboundContext = contextIndex;
            }

            inboundContextIndex = contextIndex;
        }

        contextIndex = contextIndex->next;
    }
}

void ChannelPipeline::updateOutboundHandlerContextList() {
    ChannelHandlerContext* contextIndex = this->tail;
    ChannelHandlerContext* outboundContextIndex = NULL;
    this->outboundHead = NULL;

    while (contextIndex) {
        if (contextIndex->canHandleOutboundMessage()) {
            if (!this->outboundHead) {
                this->outboundHead = contextIndex;
            }

            if (outboundContextIndex) {
                outboundContextIndex->nextOutboundContext = contextIndex;
            }

            outboundContextIndex = contextIndex;
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

void ChannelPipeline::fireChannelOpen() {
    if (inboundHead) {
        inboundHead->fireChannelOpen(*inboundHead);
    }
}

void ChannelPipeline::fireChannelActive() {
    firedChannelActive = true;

    if (inboundHead) {
        inboundHead->fireChannelActive(*inboundHead);
    }

    if (fireInboundBufferUpdatedOnActivation) {
        fireInboundBufferUpdatedOnActivation = false;

        if (inboundHead) {
            inboundHead->fireMessageUpdated(*inboundHead);
        }
    }
}

void ChannelPipeline::fireChannelInactive() {
    // Some implementations such as EmbeddedChannel can trigger inboundBufferUpdated()
    // after deactivation, so it's safe not to revert the firedChannelActive flag here.
    // Also, all known transports never get re-activated.
    //firedChannelActive = false;
    if (inboundHead) {
        inboundHead->fireChannelInactive(*inboundHead);
    }
}

void ChannelPipeline::fireExceptionCaught(const ChannelException& cause) {
    if (head) {
        head->fireExceptionCaught(cause);
    }
}

void ChannelPipeline::fireUserEventTriggered(const boost::any& evt) {
    if (head) {
        head->fireUserEventTriggered(evt);
    }
}

void ChannelPipeline::fireMessageUpdated() {
    if (!firedChannelActive) {
        fireInboundBufferUpdatedOnActivation = true;
        return;
    }

    if (inboundHead) {
        inboundHead->fireMessageUpdated(*inboundHead);
    }
}

ChannelFuturePtr ChannelPipeline::bind(const SocketAddress& localAddress) {
    return bind(localAddress, channel->newFuture());
}

const ChannelFuturePtr& ChannelPipeline::bind(const SocketAddress& localAddress,
        const ChannelFuturePtr& future) {
    if (outboundHead) {
        return outboundHead->bind(*outboundHead, localAddress, future);
    }
    else {
        LOG_ERROR << "has no handler to handle to bind";
        future->setFailure(ChannelPipelineException("has no handler to handle to bind"));
        return future;
    }
}

ChannelFuturePtr ChannelPipeline::connect(const SocketAddress& remoteAddress) {
    return connect(remoteAddress, channel->newFuture());
}

ChannelFuturePtr ChannelPipeline::connect(const SocketAddress& remoteAddress,
        const SocketAddress& localAddress) {
    return connect(remoteAddress, localAddress, channel->newFuture());
}

const ChannelFuturePtr& ChannelPipeline::connect(const SocketAddress& remoteAddress,
        const ChannelFuturePtr& future) {
    return connect(remoteAddress, SocketAddress::NULL_ADDRESS, future);
}

const ChannelFuturePtr& ChannelPipeline::connect(const SocketAddress& remoteAddress,
        const SocketAddress& localAddress,
        const ChannelFuturePtr& future) {
    if (outboundHead) {
        return outboundHead->connect(*outboundHead, remoteAddress, localAddress, future);
    }
    else {
        LOG_ERROR << "has no handler to handle to connect";
        future->setFailure(ChannelPipelineException("has no handler to handle to connect"));
        return future;
    }
}

ChannelFuturePtr ChannelPipeline::disconnect() {
    return disconnect(channel->newFuture());
}

const ChannelFuturePtr& ChannelPipeline::disconnect(const ChannelFuturePtr& future) {
    if (outboundHead) {
        return outboundHead->disconnect(*outboundHead, future);
    }
    else {
        LOG_ERROR << "has no handler to handle to disconnect";
        future->setFailure(ChannelPipelineException("has no handler to handle to disconnect"));
        return future;
    }
}

ChannelFuturePtr ChannelPipeline::close() {
    return close(channel->newFuture());
}

const ChannelFuturePtr& ChannelPipeline::close(const ChannelFuturePtr& future) {
    if (outboundHead) {
        return outboundHead->close(*outboundHead, future);
    }
    else {
        LOG_ERROR << "has no handler to handle to close";
        future->setFailure(ChannelPipelineException("has no handler to handle to close"));
        return future;
    }
}

ChannelFuturePtr ChannelPipeline::flush() {
    return flush(channel->newFuture());
}

const ChannelFuturePtr& ChannelPipeline::flush(const ChannelFuturePtr& future) {
    if (outboundHead) {
        return outboundHead->flush(*outboundHead, future);
    }
    else {
        LOG_ERROR << "has no handler to handle to close";
        future->setFailure(ChannelPipelineException("has no handler to handle to close"));
        return future;
    }
}

ChannelFuturePtr ChannelPipeline::newFuture(const ChannelPtr& channel) {
    return channel->newFuture();
}

void ChannelPipeline::setOutboundChannelBuffer(const ChannelBufferPtr& buffer) {
    ChannelOutboundBufferHandlerContext* context
        = outboundHead->nextOutboundBufferHandlerContext(outboundHead);

    if (context) {
        context->setOutboundChannelBuffer(buffer);
    }
}

void ChannelPipeline::setInboundChannelBuffer(const ChannelBufferPtr& buffer) {
    ChannelInboundBufferHandlerContext* context
        = inboundHead->nextInboundBufferHandlerContext(inboundHead);

    if (context) {
        context->setInboundChannelBuffer(buffer);
    }
}

const ChannelHandlerPtr& ChannelPipeline::getSinkHandler() const {
    return this->sinkHandler;
}

void ChannelPipeline::setSinkHandler(const ChannelHandlerPtr& handler) {
    if (handler) {
        this->sinkHandler = handler;
    }
    else {
        LOG_WARN << "set an empty sink handler";
    }
}

}
}
