
/*
 * Copyright (c) 2010-2012 frankee zhou (frankee.zhou at gmail dot com)
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

#include <cetty/channel/ChannelInboundBufferHandler.h>

#include <cetty/channel/ChannelException.h>
#include <cetty/channel/ChannelHandlerContext.h>
#include <cetty/channel/ChannelInboundBufferHandlerContext.h>

namespace cetty {
namespace channel {

ChannelInboundBufferHandler::ChannelInboundBufferHandler() {

}

ChannelInboundBufferHandler::~ChannelInboundBufferHandler() {

}

void ChannelInboundBufferHandler::channelCreated(ChannelHandlerContext& ctx) {
    ctx.fireChannelCreated();
}

void ChannelInboundBufferHandler::channelActive(ChannelHandlerContext& ctx) {
    ctx.fireChannelActive();
}

void ChannelInboundBufferHandler::channelInactive(ChannelHandlerContext& ctx) {
    ctx.fireChannelInactive();
}

void ChannelInboundBufferHandler::messageUpdated(ChannelHandlerContext& ctx) {
    try {
        messageReceived(ctx, inboundBuffer);
    }
    catch (const ChannelException& e) {
        ctx.fireExceptionCaught(e);
    }
    catch (const std::exception& e) {
        ctx.fireExceptionCaught(ChannelException(e.what()));
    }

    ctx.fireMessageUpdated();
}

void ChannelInboundBufferHandler::writeCompleted(ChannelHandlerContext& ctx) {
    ctx.fireWriteCompleted();
}

void ChannelInboundBufferHandler::beforeAdd(ChannelHandlerContext& ctx) {

}

void ChannelInboundBufferHandler::afterAdd(ChannelHandlerContext& ctx) {

}

void ChannelInboundBufferHandler::beforeRemove(ChannelHandlerContext& ctx) {

}

void ChannelInboundBufferHandler::afterRemove(ChannelHandlerContext& ctx) {

}

void ChannelInboundBufferHandler::exceptionCaught(ChannelHandlerContext& ctx,
        const ChannelException& cause) {
    ctx.fireExceptionCaught(cause);
}

void ChannelInboundBufferHandler::userEventTriggered(ChannelHandlerContext& ctx,
        const boost::any& evt) {
    ctx.fireUserEventTriggered(evt);
}

ChannelHandlerContext* ChannelInboundBufferHandler::createContext(const std::string& name,
        ChannelPipeline& pipeline,
        ChannelHandlerContext* prev,
        ChannelHandlerContext* next) {
    return new ChannelInboundBufferHandlerContext(name,
            pipeline,
            shared_from_this(),
            prev,
            next);
}

ChannelHandlerContext* ChannelInboundBufferHandler::createContext(const std::string& name,
        const EventLoopPtr& eventLoop,
        ChannelPipeline& pipeline,
        ChannelHandlerContext* prev,
        ChannelHandlerContext* next) {
    return new ChannelInboundBufferHandlerContext(name,
            eventLoop,
            pipeline,
            shared_from_this(),
            prev,
            next);
}

void ChannelInboundBufferHandler::setInboundChannelBuffer(const ChannelBufferPtr& buffer) {
    inboundBuffer = buffer;
}

void ChannelInboundBufferHandler::messageReceived(ChannelHandlerContext& ctx, const ChannelBufferPtr& msg) {
    throw ChannelException("you must implement the messageReceived method.");
}

}
}
