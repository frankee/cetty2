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

#include <cetty/channel/AbstractChannelHandler.h>
#include <cetty/channel/ChannelHandlerContext.h>

namespace cetty {
namespace channel {

void AbstractChannelHandler::beforeAdd(ChannelHandlerContext& ctx) {

}

void AbstractChannelHandler::afterAdd(ChannelHandlerContext& ctx) {

}

void AbstractChannelHandler::beforeRemove(ChannelHandlerContext& ctx) {

}

void AbstractChannelHandler::afterRemove(ChannelHandlerContext& ctx) {

}

void AbstractChannelHandler::exceptionCaught(ChannelHandlerContext& ctx, const ChannelException& cause) {

}

void AbstractChannelHandler::userEventTriggered(ChannelHandlerContext& ctx, const UserEvent& evt) {

}

ChannelHandlerContext* AbstractChannelHandler::createContext(const std::string& name, ChannelPipeline& pipeline, ChannelHandlerContext* prev, ChannelHandlerContext* next) {
    return new ChannelHandlerContext(name, pipeline, shared_from_this(), prev, next);
}

ChannelHandlerContext* AbstractChannelHandler::createContext(const std::string& name, const EventLoopPtr& eventLoop, ChannelPipeline& pipeline, ChannelHandlerContext* prev, ChannelHandlerContext* next) {
    return new ChannelHandlerContext(name, eventLoop, pipeline, shared_from_this(), prev, next);
}

}
}