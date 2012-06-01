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

#include <cetty/channel/SimpleChannelUpstreamHandler.h>

#include <cetty/channel/Channel.h>
#include <cetty/channel/ChannelState.h>
#include <cetty/channel/ChannelPipeline.h>
#include <cetty/channel/ChannelHandlerContext.h>

#include <cetty/channel/MessageEvent.h>
#include <cetty/channel/ExceptionEvent.h>
#include <cetty/channel/ChannelStateEvent.h>
#include <cetty/channel/WriteCompletionEvent.h>
#include <cetty/channel/ChildChannelStateEvent.h>

#include <cetty/buffer/ChannelBuffer.h>
#include <cetty/logging/LoggerHelper.h>
#include <cetty/logging/InternalLoggerFactory.h>

namespace cetty {
namespace channel {

using namespace cetty::logging;

//TODO: how to let all derived class has a different log.
InternalLogger* SimpleChannelUpstreamHandler::logger = NULL;

SimpleChannelUpstreamHandler::SimpleChannelUpstreamHandler() {
    if (NULL == logger) {
        logger = InternalLoggerFactory::getInstance("SimpleChannelUpstreamHandler");
    }
}

void SimpleChannelUpstreamHandler::handleUpstream(ChannelHandlerContext& ctx,
        const ChannelEvent& e) {
    ctx.sendUpstream(e);
}

void SimpleChannelUpstreamHandler::messageReceived(ChannelHandlerContext& ctx,
        const MessageEvent& e) {
    ctx.sendUpstream(e);
}

void SimpleChannelUpstreamHandler::exceptionCaught(ChannelHandlerContext& ctx,
        const ExceptionEvent& e) {
    if (this == ctx.getPipeline()->getLast().get()) {
        LOG_WARN(logger,
            "EXCEPTION:%s, please implement SimpleChannelUpstreamHandler"
            ".exceptionCaught() for proper handling.", e.getCause().getMessage().c_str());
    }

    ctx.sendUpstream(e);
}

void SimpleChannelUpstreamHandler::channelStateChanged(ChannelHandlerContext& ctx,
        const ChannelStateEvent& e) {
    const boost::any& value = e.getValue();
    const ChannelState& state = e.getState();

    if (ChannelState::OPEN == state) {
        if (value.empty()) {
            channelClosed(ctx, e);
        }
        else {
            channelOpen(ctx, e);
        }
    }
    else if (ChannelState::BOUND == state) {
        if (value.empty()) {
            channelUnbound(ctx, e);
        }
        else {
            channelBound(ctx, e);
        }
    }
    else if (ChannelState::CONNECTED == state) {
        if (value.empty()) {
            channelDisconnected(ctx, e);
        }
        else {
            channelConnected(ctx, e);
        }
    }
    else if (ChannelState::INTEREST_OPS == state) {
        channelInterestChanged(ctx, e);
    }
    else {
        ctx.sendUpstream(e);
    }
}

void SimpleChannelUpstreamHandler::childChannelStateChanged(ChannelHandlerContext& ctx,
        const ChildChannelStateEvent& e) {
    if (e.getChildChannel()->isOpen()) {
        childChannelOpen(ctx, e);
    }
    else {
        childChannelClosed(ctx, e);
    }
}

void SimpleChannelUpstreamHandler::channelOpen(ChannelHandlerContext& ctx,
        const ChannelStateEvent& e) {
    ctx.sendUpstream(e);
}

void SimpleChannelUpstreamHandler::channelBound(ChannelHandlerContext& ctx,
        const ChannelStateEvent& e) {
    ctx.sendUpstream(e);
}

void SimpleChannelUpstreamHandler::channelConnected(ChannelHandlerContext& ctx,
        const ChannelStateEvent& e) {
    ctx.sendUpstream(e);
}

void SimpleChannelUpstreamHandler::channelInterestChanged(ChannelHandlerContext& ctx,
        const ChannelStateEvent& e) {
    ctx.sendUpstream(e);
}

void SimpleChannelUpstreamHandler::channelDisconnected(ChannelHandlerContext& ctx,
        const ChannelStateEvent& e) {
    ctx.sendUpstream(e);
}

void SimpleChannelUpstreamHandler::channelUnbound(ChannelHandlerContext& ctx,
        const ChannelStateEvent& e) {
    ctx.sendUpstream(e);
}

void SimpleChannelUpstreamHandler::channelClosed(ChannelHandlerContext& ctx,
        const ChannelStateEvent& e) {
    ctx.sendUpstream(e);
}

void SimpleChannelUpstreamHandler::writeCompleted(ChannelHandlerContext& ctx,
        const WriteCompletionEvent& e) {
    ctx.sendUpstream(e);
}

void SimpleChannelUpstreamHandler::childChannelOpen(ChannelHandlerContext& ctx,
        const ChildChannelStateEvent& e) {
    ctx.sendUpstream(e);
}

void SimpleChannelUpstreamHandler::childChannelClosed(ChannelHandlerContext& ctx,
        const ChildChannelStateEvent& e) {
    ctx.sendUpstream(e);
}

cetty::channel::ChannelHandlerPtr SimpleChannelUpstreamHandler::clone() {
    ChannelHandlerPtr ptr(new SimpleChannelUpstreamHandler);
    return ptr;
}

std::string SimpleChannelUpstreamHandler::toString() const {
    return std::string("SimpleChannelUpstreamHandler");
}

SimpleChannelUpstreamHandler::~SimpleChannelUpstreamHandler() {

}


}
}