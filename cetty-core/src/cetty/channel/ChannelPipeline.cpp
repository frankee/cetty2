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
#include <cetty/channel/VoidChannelFuture.h>
#include <cetty/channel/DefaultChannelFuture.h>
#include <cetty/channel/ChannelHandlerContext.h>
#include <cetty/channel/ChannelMessageHandlerContext.h>
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

class DummyHandler : private boost::noncopyable {
public:
    typedef ChannelMessageHandlerContext<DummyHandler,
            VoidMessage,
            VoidMessage,
            VoidMessage,
            VoidMessage,
            VoidMessageContainer,
            VoidMessageContainer,
            VoidMessageContainer,
            VoidMessageContainer> Context;

    typedef ChannelHandlerWrapper<DummyHandler>::Handler Handler;
    typedef ChannelHandlerWrapper<DummyHandler>::HandlerPtr HandlerPtr;

public:
    void registerTo(Context& ctx) {}
};

ChannelPipeline::ChannelPipeline(const ChannelPtr& channel)
    : firedChannelActive_(false),
      fireMessageUpdatedOnActivation_(false),
      channel_(channel),
      eventLoop_(channel->eventLoop()),
      head_(),
      tail_() {

    head_ = tail_ = new DummyHandler::Context("dummyHeader",
            DummyHandler::HandlerPtr(new DummyHandler));

    voidFuture_ = new VoidChannelFuture(channel_);
}

ChannelPipeline::~ChannelPipeline() {
    LOG_DEBUG << "ChannelPipeline dctr";

    STLDeleteValues(&contexts_);

    if (head_) {
        delete head_;
    }
}

void ChannelPipeline::setHead(ChannelHandlerContext* ctx) {
    ChannelHandlerContext* newHead = ctx;

    if (!newHead) {
        newHead = new DummyHandler::Context("dummyHeader",
            DummyHandler::HandlerPtr(new DummyHandler));
    }

    newHead->initialize(this);

    if (head_) {
        ChannelHandlerContext* headNext = head_->next();
        delete head_;
        head_ = ctx;

        if (headNext) {
            head_->setNext(headNext);
            headNext->setPrev(head_);
        }
        else {
            tail_ = head_;
        }
    }
    else {
        head_ = newHead;
    }
}

bool ChannelPipeline::addFirst(ChannelHandlerContext* context) {
    if (!context) {
        return false;
    }

    const std::string& name = context->name();

    if (name.empty()) {
        return false;
    }

    if (duplicated(name)) {
        return false;
    }

    context->initialize(this);
    callBeforeAdd(context);

    ChannelHandlerContext* nextCtx = head_->next();

    if (nextCtx) {
        nextCtx->setPrev(context);
    }

    head_->setNext(context);

    contexts_[name] = context;

    callAfterAdd(context);

    return true;
}

bool ChannelPipeline::addLast(ChannelHandlerContext* context) {
    if (!context) {
        return false;
    }

    const std::string& name = context->name();

    if (name.empty()) {
        return false;
    }

    if (duplicated(name)) {
        return false;
    }

    context->initialize(this);
    callBeforeAdd(context);

    ChannelHandlerContext* oldTail = tail_;
    oldTail->setNext(context);
    context->setPrev(oldTail);

    tail_ = context;

    contexts_[name] = context;

    callAfterAdd(context);

    return true;
}

bool ChannelPipeline::addBefore(const std::string& name, ChannelHandlerContext* context) {
    if (!context) {
        return false;
    }

    const std::string& newName = context->name();

    if (name.empty()) {
        return false;
    }

    ChannelHandlerContext* ctx = find(name);

    if (!ctx) {
        return false;
    }

    if (duplicated(newName)) {
        return false;
    }

    context->initialize(this);
    callBeforeAdd(context);

    ctx->prev()->setNext(context);
    ctx->setPrev(context);

    context->setPrev(ctx->prev());
    context->setNext(ctx);

    contexts_[newName] = context;

    callAfterAdd(context);
    return true;
}

bool ChannelPipeline::addAfter(const std::string& name, ChannelHandlerContext* context) {
    if (!context) {
        return false;
    }

    const std::string& newName = context->name();

    if (name.empty()) {
        return false;
    }

    ChannelHandlerContext* ctx = find(name);

    if (!ctx) {
        return false;
    }

    if (duplicated(newName)) {
        return false;
    }

    context->initialize(this);
    callBeforeAdd(context);

    ctx->next()->setPrev(context);
    ctx->setNext(context);

    context->setPrev(ctx);
    context->setNext(ctx->next());

    contexts_[newName] = context;

    callAfterAdd(context);

    return true;
}

void ChannelPipeline::remove(const std::string& name) {
    ChannelHandlerContext* ctx = find(name);

    if (!ctx) {
        LOG_WARN << "no such context (" << name << "), do nothing";
        return;
    }

    callBeforeRemove(ctx);

    ChannelHandlerContext* prev = ctx->prev();
    ChannelHandlerContext* next = ctx->next();

    prev->setNext(next);
    next->setPrev(prev);

    contexts_.erase(ctx->name());

    callAfterRemove(ctx);

    delete ctx;
}

void ChannelPipeline::removeFirst() {
    if (head_->next()) {
        remove(head_->next()->name());
    }
}

void ChannelPipeline::removeLast() {
    ChannelHandlerContext* oldTail = tail_;
    callBeforeRemove(oldTail);

    oldTail->prev()->setNext(NULL);
    tail_ = oldTail->prev();
    contexts_.erase(oldTail->name());

    callAfterRemove(oldTail);

    delete oldTail;
}

bool ChannelPipeline::replace(const std::string& name, ChannelHandlerContext* context) {
    if (name.empty() || !context) {
        LOG_WARN << "input parameters is invalid.";
        return false;
    }

    ChannelHandlerContext* ctx = find(name);

    if (!ctx) {
        LOG_WARN << "can't find the src context: " << name;
        return false;
    }

    if (ctx == tail_) {
        removeLast();
        return addLast(context);
    }

    const std::string& oldName = ctx->name();
    const std::string& newName = context->name();

    ChannelHandlerContext* prev = ctx->prev();
    ChannelHandlerContext* next = ctx->next();

    context->initialize(this);
    callBeforeRemove(ctx);
    callBeforeAdd(context);

    prev->setNext(context);
    next->setPrev(context);

    context->setPrev(prev);
    context->setNext(next);

    contexts_.erase(ctx->name());
    contexts_[newName] = context;

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
        callAfterAdd(context);
        added = true;
    }
    catch (const ChannelHandlerLifeCycleException& e) {
        addException = e;
    }

    if (!removed && !added) {
        LOG_WARN << "Both " << ctx->toString() << " fireAfterRemove and "
                 << context->toString() << " fireAfterAdd failed; see logs.";
    }
    else if (!removed) {
        removeException.rethrow();
    }
    else if (!added) {
        addException.rethrow();
    }
    else {
        delete ctx;
        return true;
    }

    return true;
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
        buf.append(ctx->toString());

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

bool ChannelPipeline::callBeforeAdd(ChannelHandlerContext* ctx) {
    try {
        ctx->fireBeforeAdd();
        return true;
    }
    catch (const Exception& e) {
        LOG_WARN << "fireBeforeAdd has thrown an exception: "
                 << e.what() << ", not adding.";
    }

    return false;
}

bool ChannelPipeline::callAfterAdd(ChannelHandlerContext* ctx) {
    try {
        ctx->fireAfterAdd();
        return true;
    }
    catch (const Exception& e) {
        LOG_WARN << "call fireAfterAdd has throw an exception:"
                 << e.what() << ", then try to remove";

        bool removed = false;

        try {
            remove(ctx->name());
            removed = true;
        }
        catch (const Exception& e) {
            LOG_WARN << "Failed to remove a handler: "
                     << ctx->name()
                     << ", reason: " << e.what();

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

    return false;
}

bool ChannelPipeline::callBeforeRemove(ChannelHandlerContext* ctx) {
    try {
        ctx->fireBeforeRemove();
        return true;
    }
    catch (const Exception& e) {
        LOG_WARN << "fireBeforeRemove has thrown an exception: "
                 << e.what() << ", not removing.";
    }

    return false;
}

bool ChannelPipeline::callAfterRemove(ChannelHandlerContext* ctx) {
    try {
        ctx->fireAfterRemove();
        return true;
    }
    catch (const Exception& e) {
        LOG_WARN << "fireAfterRemove has thrown an exception: " << e.what();
    }

    return false;
}

void ChannelPipeline::fireChannelOpen() {
    if (!head_->channelOpenCallback()) {
        head_->fireChannelOpen();
    }
    else {
        head_->fireChannelOpen(*head_);
    }
}

void ChannelPipeline::fireChannelActive() {
    firedChannelActive_ = true;

    if (!head_->channelActiveCallback()) {
        head_->fireChannelActive();
    }
    else {
        head_->fireChannelActive(*head_);
    }

    if (fireMessageUpdatedOnActivation_) {
        fireMessageUpdatedOnActivation_ = false;

        head_->fireMessageUpdated(*head_);
    }
}

void ChannelPipeline::fireChannelInactive() {
    // Some implementations such as EmbeddedChannel can trigger inboundBufferUpdated()
    // after deactivation, so it's safe not to revert the firedChannelActive flag here.
    // Also, all known transports never get re-activated.
    //firedChannelActive = false;
    if (!head_->channelInactiveCallback()) {
        head_->fireChannelInactive();
    }
    else {
        head_->fireChannelInactive(*head_);
    }
}

void ChannelPipeline::fireExceptionCaught(const ChannelException& cause) {
    if (!head_->exceptionCallback()) {
        head_->fireExceptionCaught(cause);
    }
    else {
        head_->fireExceptionCaught(*head_, cause);
    }
}

void ChannelPipeline::fireUserEventTriggered(const boost::any& evt) {
    if (!head_->userEventCallback()) {
        head_->fireUserEventTriggered(evt);
    }
    else {
        head_->fireUserEventTriggered(*head_, evt);
    }
}

void ChannelPipeline::fireMessageUpdated() {
    //     if (!firedChannelActive_) {
    //         fireMessageUpdatedOnActivation_ = true;
    //         return;
    //     }
    if (!head_->channelMessageUpdatedCallback()) {
        head_->fireMessageUpdated();
    }
    else {
        head_->fireMessageUpdated(*head_);
    }
}

ChannelFuturePtr ChannelPipeline::bind(const SocketAddress& localAddress) {
    return bind(localAddress, channel_.lock()->newFuture());
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
    return connect(remoteAddress,
                   SocketAddress::NULL_ADDRESS,
                   channel_.lock()->newFuture());
}

ChannelFuturePtr ChannelPipeline::connect(const SocketAddress& remoteAddress,
        const SocketAddress& localAddress) {
    return connect(remoteAddress,
                   localAddress,
                   channel_.lock()->newFuture());
}

const ChannelFuturePtr& ChannelPipeline::connect(const SocketAddress& remoteAddress,
        const ChannelFuturePtr& future) {
    return connect(remoteAddress,
                   SocketAddress::NULL_ADDRESS,
                   future);
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
    return disconnect(channel_.lock()->newFuture());
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
    return close(channel_.lock()->newFuture());
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
    return flush(channel_.lock()->newFuture());
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

ChannelFuturePtr ChannelPipeline::newFuture() {
    return new DefaultChannelFuture(channel_, false);
    //return channel_.lock()->newFuture();
}

}
}
