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

#include <cetty/handler/logging/LoggingHandler.h>
#include <cetty/buffer/ChannelBuffer.h>
#include <cetty/channel/Channel.h>
#include <cetty/channel/InetAddress.h>

#include <cetty/logging/Logger.h>

namespace cetty {
namespace handler {
namespace logging {

using namespace cetty::logging;
using namespace cetty::buffer;
using namespace cetty::channel;


void LoggingHandler::channelOpen(ChannelHandlerContext& ctx) {
    if (Logger::isEnabled(level_)) {
        Logger("loggerHander", 0, level_).stream()
                << ctx.channel()->toString()
                << " OPEN";
    }
}

void LoggingHandler::channelActive(ChannelHandlerContext& ctx) {
    if (Logger::isEnabled(level_)) {
        Logger("loggerHander", 0, level_).stream()
                << ctx.channel()->toString()
                << " ACTIVE";
    }
}

void LoggingHandler::channelInactive(ChannelHandlerContext& ctx) {
    if (Logger::isEnabled(level_)) {
        Logger("loggerHander", 0, level_).stream()
                << ctx.channel()->toString()
                << " INACTIVE";
    }
}

void LoggingHandler::exceptionCaught(ChannelHandlerContext& ctx,
                                     const ChannelException& cause) {
    if (Logger::isEnabled(level_)) {
        Logger("loggerHander", 0, level_).stream()
                << ctx.channel()->toString()
                << " EXCEPTION: "
                << cause.getMessage();
    }
}

void LoggingHandler::userEventTriggered(ChannelHandlerContext& ctx,
                                        const boost::any& evt) {
    if (Logger::isEnabled(level_)) {
        Logger("loggerHander", 0, level_).stream()
                << ctx.channel()->toString()
                << " USER_EVENT: ";
    }
}

void LoggingHandler::bind(ChannelHandlerContext& ctx,
                          const InetAddress& localAddress,
                          const ChannelFuturePtr& future) {
    if (Logger::isEnabled(level_)) {
        Logger("loggerHander", 0, level_).stream()
                << ctx.channel()->toString()
                << " BIND("
                << localAddress.toString()
                << ")";
    }
}

void LoggingHandler::connect(ChannelHandlerContext& ctx,
                             const InetAddress& remoteAddress,
                             const InetAddress& localAddress,
                             const ChannelFuturePtr& future) {
    if (Logger::isEnabled(level_)) {
        Logger("loggerHander", 0, level_).stream()
                << ctx.channel()->toString()
                << " CONNECT("
                << remoteAddress.toString()
                << ", "
                << localAddress.toString()
                << ")";
    }
}

void LoggingHandler::disconnect(ChannelHandlerContext& ctx, const ChannelFuturePtr& future) {
    if (Logger::isEnabled(level_)) {
        Logger("loggerHander", 0, level_).stream()
                << ctx.channel()->toString()
                << " DISCONNECT()";
    }
}

void LoggingHandler::close(ChannelHandlerContext& ctx,
                           const ChannelFuturePtr& future) {
    if (Logger::isEnabled(level_)) {
        Logger("loggerHander", 0, level_).stream()
                << ctx.channel()->toString()
                << " CLOSE()";
    }
}

}
}
}
