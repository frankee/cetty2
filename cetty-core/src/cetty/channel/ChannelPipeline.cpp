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
      channel_(),
      head_(),
      tail_() {
}

ChannelPipeline::~ChannelPipeline() {
    //LOG_DEBUG << "ChannelPipeline dector";
    STLDeleteValues(&contexts_);
}

void ChannelPipeline::attach(const ChannelPtr& channel) {
    if (!channel) {
        LOG_WARN << "attach a null channel to pipeline, ignore.";
    }

    if (channel_) {
        LOG_WARN << "the pipeline has already attached, ignore.";
    }

    channel_ = channel;
    eventLoop_ = channel_->eventLoop();

    ChannelHandlerContext* ctx = head_;

    while (true) {
        if (ctx == tail_) {
            ctx->attach();
            break;
        }

        ctx->attach();
        ctx = ctx->next();
    }
}

void ChannelPipeline::detach() {
    ChannelHandlerContext* ctx = head_;

    while (true) {
        if (ctx == tail_) {
            ctx->detach();
            break;
        }

        ctx->detach();
        ctx = ctx->next();
    }

    channel_.reset();
    eventLoop_.reset();
}

bool ChannelPipeline::isAttached() const {
    return !!channel_;
}

void ChannelPipeline::addFirst(ChannelHandlerContext* ctx) {
    if (!ctx || ctx->name().empty()) { return; }

    boost::lock_guard<boost::recursive_mutex> guard(mutex);

    if (contexts_.empty()) {
        init(ctx);
    }
    else {
        checkDuplicateName(ctx->name());

        ChannelHandlerContext* oldHead = head_;
        ctx->setPipeline(this);
        
        callBeforeAdd(ctx);

        oldHead->setBefore(ctx);
        head_ = ctx;

        contexts_[ctx->name()] = ctx;

        callAfterAdd(ctx);
    }
}

void ChannelPipeline::addLast(ChannelHandlerContext* context) {
    boost::lock_guard<boost::recursive_mutex> guard(mutex);

    if (!handler) { return; }

    if (contexts_.empty()) {
        init(name, handler);
    }
    else {
        checkDuplicateName(name);
        ChannelHandlerContext* oldTail = this->tail_;
        ChannelHandlerContext* newTail =
            handler->createContext(name, *this, oldTail, NULL);

        callBeforeAdd(newTail);

        oldTail->next = newTail;
        this->tail_ = newTail;

        // for upstream and downstream single list.
        if (newTail->canHandleInboundMessage()) {
            updateInboundHandlerContextList();
        }

        if (newTail->canHandleOutboundMessage()) {
            updateOutboundHandlerContextList();
        }

        contexts_[name] = newTail;

        callAfterAdd(newTail);
    }
}

void ChannelPipeline::addBefore(const std::string& name, ChannelHandlerContext* context) {
    boost::lock_guard<boost::recursive_mutex> guard(mutex);

    if (!handler) { return; }

    ChannelHandlerContext* ctx = getContextOrDie(baseName);

    if (ctx == this->head_) {
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

        contexts_[name] = newCtx;

        callAfterAdd(newCtx);
    }
}

void ChannelPipeline::addAfter(const std::string& name, ChannelHandlerContext* context) {
    boost::lock_guard<boost::recursive_mutex> guard(mutex);

    if (!handler) { return; }

    ChannelHandlerContext* ctx = getContextOrDie(baseName);

    if (ctx == this->tail_) {
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

        contexts_[name] = newCtx;

        callAfterAdd(newCtx);
    }
}

const ChannelHandlerContext* ChannelPipeline::remove(const std::string& name) {
    ChannelHandlerContext* ctx = find(name);
    
    if (head_ == tail_) {
        head_ = tail_ = NULL;
        contexts_.clear();
    }
    else if (ctx == head_) {
        return removeFirst();
    }
    else if (ctx == tail_) {
        return removeLast();
    }
    else {
        callBeforeRemove(ctx);

        ChannelHandlerContext* prev = ctx->before();
        ChannelHandlerContext* next = ctx->next();
        
        prev->next = next;
        next->prev = prev;

        contexts_.erase(ctx->name());

        callAfterRemove(ctx);
    }

    ChannelHandlerPtr ptr = ctx->getHandler();
    delete ctx;
    return ptr;
}

ChannelHandlerPtr ChannelPipeline::removeFirst() {
    boost::lock_guard<boost::recursive_mutex> guard(mutex);

    if (contexts_.empty()) {
        throw RangeException("name2ctx is empty");
    }

    ChannelHandlerContext* oldHead = head_;

    if (oldHead == NULL) {
        throw RangeException("head is null");
    }

    callBeforeRemove(oldHead);

    if (oldHead->next == NULL) {
        head_ = tail_ = NULL;
        contexts_.clear();
    }
    else {
        oldHead->next->prev = NULL;
        head_ = oldHead->next;
        contexts_.erase(oldHead->name());
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

const ChannelHandlerContext* ChannelPipeline::removeFirst() {

}

ChannelHandlerPtr ChannelPipeline::removeLast() {
    boost::lock_guard<boost::recursive_mutex> guard(mutex);

    if (contexts_.empty()) {
        throw RangeException("name2ctx is empty");
    }

    ChannelHandlerContext* oldTail = tail_;

    if (oldTail == NULL) {
        throw RangeException("head is null");
    }

    callBeforeRemove(oldTail);

    if (oldTail->prev == NULL) {
        this->head_ = this->tail_ = NULL;
        contexts_.clear();
    }
    else {
        oldTail->prev->next = NULL;
        this->tail_ = oldTail->prev;
        contexts_.erase(oldTail->name());
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

const ChannelHandlerContext* ChannelPipeline::removeLast() {

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

    if (ctx == head_) {
        replacedHandle = removeFirst();
        addFirst(newName, newHandler);
    }
    else if (ctx == tail_) {
        replacedHandle = removeLast();
        addLast(newName, newHandler);
    }
    else {
        bool sameName = (ctx->name().compare(newName) == 0);

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

        if (!sameName) {
            contexts_.erase(ctx->name());
            contexts_[newName] = newCtx;
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

void ChannelPipeline::replace(const std::string& name, ChannelHandlerContext* context) {

}

ChannelHandlerContext* ChannelPipeline::first() const {
    return head_;
}

ChannelHandlerContext* ChannelPipeline::last() const {
    return tail_;
}

ChannelHandlerContext* ChannelPipeline::find(const std::string& name) const {
    ContextMap::const_iterator itr = contexts_.find(name);
    if (itr != contexts_.end()) {
        return itr->second;
    }

    return NULL;
}

std::string ChannelPipeline::toString() const {
    std::string buf("ChannelPipeline {");
    buf.reserve(1024);

    ChannelHandlerContext* ctx = this->head_;

    for (;;) {
        buf.append("(");
        buf.append(ctx->name());
        buf.append(" = ");
        buf.append(ctx->getHandler()->toString());
        buf.append(")");

        ctx = ctx->next();

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
        ctx->fireBeforeAdd();
    }
    catch (const Exception& e) {
        throw ChannelHandlerLifeCycleException(
            ".beforeAdd() has thrown an exception; not adding.", e);
    }
}

void ChannelPipeline::callAfterAdd(ChannelHandlerContext* ctx) {
    try {
        ctx->fireAfterAdd();
    }
    catch (const Exception& e) {
        LOG_WARN_E(e) << "call afterAdd has throw an exception, then try to remove";

        bool removed = false;

        try {
            remove((ChannelHandlerContext*)ctx);
            removed = true;
        }
        catch (const Exception& e) {
            LOG_WARN_E(e) << "Failed to remove a handler: " << ctx->name();

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
        ctx->fireBeforeRemove();
    }
    catch (const Exception& e) {
        throw ChannelHandlerLifeCycleException(
            ".beforeRemove() has thrown an exception; not removing.", e);
    }
}

void ChannelPipeline::callAfterRemove(ChannelHandlerContext* ctx) {
    try {
        ctx->fireAfterRemove();
    }
    catch (const Exception& e) {
        throw ChannelHandlerLifeCycleException(
            ".afterRemove() has thrown an exception.", e);
    }
}

void ChannelPipeline::init(ChannelHandlerContext* ctx) {
    if (!ctx || ctx->name().empty()) {
        return;
    }

    ctx->setPipeline(this);
    callBeforeAdd(ctx);

    head_ = tail_ = ctx;
    contexts_.clear();
    contexts_[ctx->name()] = ctx;

    callAfterAdd(ctx);
}

void ChannelPipeline::checkDuplicateName(const std::string& name) {
    if (contexts_.find(name) != contexts_.end()) {
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

ChannelHandlerContext* ChannelPipeline::getContextNoLock(const std::string& name) const {
    ContextMap::const_iterator itr = contexts_.find(name);

    if (itr == contexts_.end()) {
        return NULL;
    }

    return itr->second;
}

void ChannelPipeline::setAttachment(const std::string& name, const boost::any& attachment) {
    attachments_[name] = attachment;
}

boost::any ChannelPipeline::getAttachment(const std::string& name) const {
    AttachmentMap::const_iterator itr = attachments_.find(name);

    if (itr != attachments_.end()) {
        return itr->second;
    }

    return boost::any();
}

void ChannelPipeline::fireChannelOpen() {
    if (head_) {
        head_->fireChannelOpen(*head_);
    }
}

void ChannelPipeline::fireChannelActive() {
    firedChannelActive = true;

    if (head_) {
        head_->fireChannelActive(*head_);
    }

    if (fireInboundBufferUpdatedOnActivation) {
        fireInboundBufferUpdatedOnActivation = false;

        if (head_) {
            head_->fireMessageUpdated(*head_);
        }
    }
}

void ChannelPipeline::fireChannelInactive() {
    // Some implementations such as EmbeddedChannel can trigger inboundBufferUpdated()
    // after deactivation, so it's safe not to revert the firedChannelActive flag here.
    // Also, all known transports never get re-activated.
    //firedChannelActive = false;
    if (head_) {
        head_->fireChannelInactive(*head_);
    }
}

void ChannelPipeline::fireExceptionCaught(const ChannelException& cause) {
    if (head_) {
        head_->fireExceptionCaught(*head_, cause);
    }
}

void ChannelPipeline::fireUserEventTriggered(const boost::any& evt) {
    if (head_) {
        head_->fireUserEventTriggered(*head_, evt);
    }
}

void ChannelPipeline::fireMessageUpdated() {
    if (!firedChannelActive) {
        fireInboundBufferUpdatedOnActivation = true;
        return;
    }

    if (head_) {
        head_->fireMessageUpdated(*head_);
    }
}

ChannelFuturePtr ChannelPipeline::bind(const SocketAddress& localAddress) {
    return bind(localAddress, channel_->newFuture());
}

const ChannelFuturePtr& ChannelPipeline::bind(const SocketAddress& localAddress,
        const ChannelFuturePtr& future) {
    if (tail_) {
        return tail_->bind(*tail_, localAddress, future);
    }
    else {
        LOG_ERROR << "has no handler to handle to bind";
        future->setFailure(ChannelPipelineException("has no handler to handle to bind"));
        return future;
    }
}

ChannelFuturePtr ChannelPipeline::connect(const SocketAddress& remoteAddress) {
    return connect(remoteAddress, channel_->newFuture());
}

ChannelFuturePtr ChannelPipeline::connect(const SocketAddress& remoteAddress,
        const SocketAddress& localAddress) {
    return connect(remoteAddress, localAddress, channel_->newFuture());
}

const ChannelFuturePtr& ChannelPipeline::connect(const SocketAddress& remoteAddress,
        const ChannelFuturePtr& future) {
    return connect(remoteAddress, SocketAddress::NULL_ADDRESS, future);
}

const ChannelFuturePtr& ChannelPipeline::connect(const SocketAddress& remoteAddress,
        const SocketAddress& localAddress,
        const ChannelFuturePtr& future) {
    if (tail_) {
        return tail_->connect(*tail_, remoteAddress, localAddress, future);
    }
    else {
        LOG_ERROR << "has no handler to handle to connect";
        future->setFailure(ChannelPipelineException("has no handler to handle to connect"));
        return future;
    }
}

ChannelFuturePtr ChannelPipeline::disconnect() {
    return disconnect(channel_->newFuture());
}

const ChannelFuturePtr& ChannelPipeline::disconnect(const ChannelFuturePtr& future) {
    if (tail_) {
        return tail_->disconnect(*tail_, future);
    }
    else {
        LOG_ERROR << "has no handler to handle to disconnect";
        future->setFailure(ChannelPipelineException("has no handler to handle to disconnect"));
        return future;
    }
}

ChannelFuturePtr ChannelPipeline::close() {
    return close(channel_->newFuture());
}

const ChannelFuturePtr& ChannelPipeline::close(const ChannelFuturePtr& future) {
    if (tail_) {
        return tail_->close(*tail_, future);
    }
    else {
        LOG_ERROR << "has no handler to handle to close";
        future->setFailure(ChannelPipelineException("has no handler to handle to close"));
        return future;
    }
}

ChannelFuturePtr ChannelPipeline::flush() {
    return flush(channel_->newFuture());
}

const ChannelFuturePtr& ChannelPipeline::flush(const ChannelFuturePtr& future) {
    if (tail_) {
        return tail_->flush(*tail_, future);
    }
    else {
        LOG_ERROR << "has no handler to handle to close";
        future->setFailure(ChannelPipelineException("has no handler to handle to close"));
        return future;
    }
}

ChannelFuturePtr ChannelPipeline::newFuture(const ChannelPtr& channel) {
    return channel_->newFuture();
}

}
}
