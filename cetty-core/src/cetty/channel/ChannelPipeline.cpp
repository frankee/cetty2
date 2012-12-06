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
#include <cetty/channel/DefaultChannelFuture.h>
#include <cetty/channel/ChannelHandlerContext.h>
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

ChannelPipeline::ChannelPipeline(const ChannelPtr& channel)
    : firedChannelActive_(false),
      fireMessageUpdatedOnActivation_(false),
      channel_(channel),
      eventLoop_(channel->eventLoop()),
      head_(),
      tail_() {
}

ChannelPipeline::~ChannelPipeline() {
    LOG_DEBUG << "ChannelPipeline dectr";
    STLDeleteValues(&contexts_);
}

bool ChannelPipeline::addFirst(ChannelHandlerContext* context) {
    if (!context) {
        return false;
    }

    const std::string& name = context->name();

    if (name.empty()) {
        return false;
    }

    if (contexts_.empty()) {
        return initWith(context);
    }

    if (duplicated(name)) {
        return false;
    }

    ChannelHandlerContext* oldHead = head_;
    context->initialize(this);

    callBeforeAdd(context);

    oldHead->setPrev(context);
    head_ = context;

    context->setNext(oldHead);

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

    context->initialize(this);

    if (contexts_.empty()) {
        return initWith(context);
    }

    if (duplicated(name)) {
        return false;
    }

    ChannelHandlerContext* oldTail = this->tail_;

    callBeforeAdd(context);

    oldTail->setNext(context);
    this->tail_ = context;

    context->setPrev(oldTail);

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

    if (ctx == this->head_) {
        return addFirst(context);
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

    if (ctx == this->tail_) {
        return addLast(context);
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

        ChannelHandlerContext* prev = ctx->prev();
        ChannelHandlerContext* next = ctx->next();

        prev->setNext(next);
        next->setPrev(prev);

        contexts_.erase(ctx->name());

        callAfterRemove(ctx);
    }

    delete ctx;
}

void ChannelPipeline::removeFirst() {
    boost::lock_guard<boost::recursive_mutex> guard(mutex_);

    if (contexts_.empty()) {
        LOG_WARN << "contexts is empty";
        return;
    }

    ChannelHandlerContext* oldHead = head_;

    if (!oldHead) {
        LOG_WARN << "head is null";
        return;
    }

    callBeforeRemove(oldHead);

    if (!oldHead->next()) {
        head_ = tail_ = NULL;
        contexts_.clear();
    }
    else {
        oldHead->next()->setPrev(NULL);
        head_ = oldHead->next();
        contexts_.erase(oldHead->name());
    }

    callAfterRemove(oldHead);

    delete oldHead;
}

void ChannelPipeline::removeLast() {
    boost::lock_guard<boost::recursive_mutex> guard(mutex_);

    if (contexts_.empty()) {
        LOG_WARN << "contexts is empty";
        return;
    }

    ChannelHandlerContext* oldTail = tail_;

    if (oldTail == NULL) {
        LOG_WARN << "ChannelHandlerContext is null";
        return;
    }

    callBeforeRemove(oldTail);

    if (!oldTail->prev()) {
        this->head_ = this->tail_ = NULL;
        contexts_.clear();
    }
    else {
        oldTail->prev()->setNext(NULL);
        this->tail_ = oldTail->prev();
        contexts_.erase(oldTail->name());
    }

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

    if (ctx == head_) {
        removeFirst();
        return addFirst(context);
    }
    else if (ctx == tail_) {
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

bool ChannelPipeline::initWith(ChannelHandlerContext* ctx) {
    if (!ctx || ctx->name().empty()) {
        return false;
    }

    ctx->initialize(this);
    callBeforeAdd(ctx);

    head_ = tail_ = ctx;
    contexts_.clear();
    contexts_[ctx->name()] = ctx;

    callAfterAdd(ctx);
    return true;
}

void ChannelPipeline::fireChannelOpen() {
    if (head_) {
        head_->fireChannelOpen(*head_);
    }
}

void ChannelPipeline::fireChannelActive() {
    firedChannelActive_ = true;

    if (head_) {
        head_->fireChannelActive(*head_);
    }

    if (fireMessageUpdatedOnActivation_) {
        fireMessageUpdatedOnActivation_ = false;

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
    //     if (!firedChannelActive_) {
    //         fireMessageUpdatedOnActivation_ = true;
    //         return;
    //     }

    if (head_) {
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
