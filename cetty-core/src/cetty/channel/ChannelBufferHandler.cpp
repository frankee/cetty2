
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

#include <cetty/channel/channelBufferHandler.h>
#include <cetty/channel/ChannelBufferHandlerContext.h>

namespace cetty {
namespace channel {

ChannelBufferHandler::~ChannelBufferHandler() {

}

void ChannelBufferHandler::beforeAdd(ChannelHandlerContext& ctx) {

}

void ChannelBufferHandler::afterAdd(ChannelHandlerContext& ctx) {

}

void ChannelBufferHandler::beforeRemove(ChannelHandlerContext& ctx) {

}

void ChannelBufferHandler::afterRemove(ChannelHandlerContext& ctx) {

}

void ChannelBufferHandler::exceptionCaught(ChannelHandlerContext& ctx,
        const ChannelException& cause) {

}

void ChannelBufferHandler::userEventTriggered(ChannelHandlerContext& ctx,
        const UserEvent& evt) {

}

ChannelHandlerContext* ChannelBufferHandler::createContext(
    const std::string& name,
    ChannelPipeline& pipeline,
    ChannelHandlerContext* prev,
    ChannelHandlerContext* next) {
        return new ChannelBufferHandlerContext(name, pipeline, shared_from_this(), prev, next);
}

ChannelHandlerContext* ChannelBufferHandler::createContext(
    const std::string& name,
    const EventLoopPtr& eventLoop,
    ChannelPipeline& pipeline,
    ChannelHandlerContext* prev,
    ChannelHandlerContext* next) {
        return new ChannelBufferHandlerContext(name, eventLoop, pipeline, shared_from_this(), prev, next);
}

}
}