
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

#include <cetty/channel/ChannelOutboundBufferHandler.h>
#include <cetty/channel/ChannelHandlerContext.h>
#include <cetty/channel/ChannelOutboundBufferHandlerContext.h>

namespace cetty {
namespace channel {

ChannelOutboundBufferHandler::ChannelOutboundBufferHandler() {

}

ChannelOutboundBufferHandler::~ChannelOutboundBufferHandler() {

}

void ChannelOutboundBufferHandler::bind(ChannelHandlerContext& ctx,
                                        const SocketAddress& localAddress,
                                        const ChannelFuturePtr& future) {
    ctx.bind(localAddress, future);
}

void ChannelOutboundBufferHandler::connect(ChannelHandlerContext& ctx,
        const SocketAddress& remoteAddress,
        const SocketAddress& localAddress,
        const ChannelFuturePtr& future) {
    ctx.connect(remoteAddress, localAddress, future);
}

void ChannelOutboundBufferHandler::disconnect(ChannelHandlerContext& ctx,
        const ChannelFuturePtr& future) {
    ctx.disconnect(future);
}

void ChannelOutboundBufferHandler::close(ChannelHandlerContext& ctx,
        const ChannelFuturePtr& future) {
    ctx.close(future);
}

void ChannelOutboundBufferHandler::flush(ChannelHandlerContext& ctx,
        const ChannelFuturePtr& future) {
    ctx.flush(future);
}

void ChannelOutboundBufferHandler::beforeAdd(ChannelHandlerContext& ctx) {

}

void ChannelOutboundBufferHandler::afterAdd(ChannelHandlerContext& ctx) {

}

void ChannelOutboundBufferHandler::beforeRemove(ChannelHandlerContext& ctx) {

}

void ChannelOutboundBufferHandler::afterRemove(ChannelHandlerContext& ctx) {

}

void ChannelOutboundBufferHandler::exceptionCaught(ChannelHandlerContext& ctx,
        const ChannelException& cause) {
    ctx.fireExceptionCaught(cause);
}

void ChannelOutboundBufferHandler::userEventTriggered(ChannelHandlerContext& ctx,
        const UserEvent& evt) {
    ctx.fireUserEventTriggered(evt);
}

ChannelHandlerContext* ChannelOutboundBufferHandler::createContext(const std::string& name,
        ChannelPipeline& pipeline,
        ChannelHandlerContext* prev,
        ChannelHandlerContext* next) {
    return new ChannelOutboundBufferHandlerContext(name,
            pipeline,
            shared_from_this(),
            prev,
            next);
}

ChannelHandlerContext* ChannelOutboundBufferHandler::createContext(const std::string& name,
        const EventLoopPtr& eventLoop,
        ChannelPipeline& pipeline,
        ChannelHandlerContext* prev,
        ChannelHandlerContext* next) {
    return new ChannelOutboundBufferHandlerContext(name,
            eventLoop,
            pipeline,
            shared_from_this(),
            prev,
            next);
}

void ChannelOutboundBufferHandler::setOutboundChannelBuffer(const ChannelBufferPtr& buffer) {
    outboundBuffer = buffer;
}

}
}
