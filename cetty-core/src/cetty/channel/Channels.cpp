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

#include <cetty/channel/Channels.h>

#include <stdarg.h>
#include <boost/assert.hpp>

#include <cetty/util/Integer.h>
#include <cetty/util/Exception.h>
#include <cetty/util/internal/ConversionUtil.h>

#include <cetty/channel/Channel.h>
#include <cetty/channel/ChannelFactory.h>
#include <cetty/channel/ChannelHandlerContext.h>
#include <cetty/channel/ChannelPipelineFactory.h>
#include <cetty/channel/ChannelState.h>
#include <cetty/channel/SocketAddress.h>
#include <cetty/channel/UpstreamChannelStateEvent.h>
#include <cetty/channel/DownstreamChannelStateEvent.h>
#include <cetty/channel/UpstreamMessageEvent.h>
#include <cetty/channel/DownstreamMessageEvent.h>
#include <cetty/channel/DefaultChannelFuture.h>
#include <cetty/channel/DefaultChannelPipeline.h>
#include <cetty/channel/DefaultChildChannelStateEvent.h>
#include <cetty/channel/DefaultWriteCompletionEvent.h>
#include <cetty/channel/DefaultExceptionEvent.h>
#include <cetty/channel/FailedChannelFuture.h>
#include <cetty/channel/AbstractChannel.h>
#include <cetty/channel/SucceededChannelFuture.h>

namespace cetty {
namespace channel {

using namespace cetty::util;
using namespace cetty::util::internal;

class CloneChannelPipelineFactory : public ChannelPipelineFactory {
public:
    CloneChannelPipelineFactory(const ChannelPipelinePtr pipeline)
        : cloneFromPipeline(pipeline) {
    }
    virtual ~CloneChannelPipelineFactory() {}

    ChannelPipelinePtr getPipeline() {
        return Channels::pipeline(cloneFromPipeline);
    }

private:
    ChannelPipelinePtr cloneFromPipeline;
};

ChannelPipelinePtr Channels::pipeline() {
    return new DefaultChannelPipeline();
}

ChannelPipelinePtr Channels::pipeline(const ChannelPipelinePtr& pipeline) {
    ChannelPipelinePtr newPipeline = Channels::pipeline();
    BOOST_ASSERT(newPipeline);

    ChannelPipeline::ChannelHandlers handlers = pipeline->getChannelHandles();
    ChannelPipeline::ChannelHandlers::iterator itr = handlers.begin();

    for (; itr != handlers.end(); ++itr) {
        newPipeline->addLast(itr->first, itr->second->clone());
    }

    return newPipeline;
}

ChannelPipelinePtr Channels::pipeline(const ChannelHandlerPtr& handler) {
    ChannelPipelinePtr newPipeline = pipeline();

    if (handler) {
        newPipeline->addLast("0", handler);
    }

    return newPipeline;
}

ChannelPipelinePtr Channels::pipeline(const ChannelHandlerPtr& handler0,
                                    const ChannelHandlerPtr& handler1) {
    ChannelPipelinePtr newPipeline = pipeline();

    if (handler0) { newPipeline->addLast("0", handler0); }
    if (handler1) { newPipeline->addLast("1", handler1); }

    return newPipeline;
}

ChannelPipelinePtr Channels::pipeline(const ChannelHandlerPtr& handler0,
                                    const ChannelHandlerPtr& handler1,
                                    const ChannelHandlerPtr& handler2) {
    ChannelPipelinePtr newPipeline = pipeline();

    if (handler0) { newPipeline->addLast("0", handler0); }
    if (handler1) { newPipeline->addLast("1", handler1); }
    if (handler1) { newPipeline->addLast("2", handler2); }

    return newPipeline;
}

ChannelPipelinePtr Channels::pipeline(const ChannelHandlerPtr& handler0,
                                    const ChannelHandlerPtr& handler1,
                                    const ChannelHandlerPtr& handler2,
                                    const ChannelHandlerPtr& handler3) {
    ChannelPipelinePtr newPipeline = pipeline();

    if (handler0) { newPipeline->addLast("0", handler0); }
    if (handler1) { newPipeline->addLast("1", handler1); }
    if (handler1) { newPipeline->addLast("2", handler2); }
    if (handler1) { newPipeline->addLast("3", handler3); }

    return newPipeline;
}

ChannelPipelinePtr Channels::pipeline(const std::vector<ChannelHandlerPtr>& handlers) {
    ChannelPipelinePtr newPipeline = Channels::pipeline();
    BOOST_ASSERT(newPipeline);

    for (size_t i = 0; i < handlers.size(); ++i) {
        const ChannelHandlerPtr& h = handlers[i];

        if (!h) {
            continue;
        }

        newPipeline->addLast(ConversionUtil::toString((int)i), handlers[i]);
    }

    return newPipeline;
}

ChannelPipelineFactoryPtr Channels::pipelineFactory(const ChannelPipelinePtr& pipeline) {
    return new CloneChannelPipelineFactory(pipeline);
}

ChannelFuturePtr Channels::future(const ChannelPtr& channel, bool cancellable) {
    return new DefaultChannelFuture(channel, cancellable);
}

ChannelFuturePtr Channels::failedFuture(const ChannelPtr& channel, const Exception& cause) {
    return ChannelFuturePtr(new FailedChannelFuture(channel, cause));
}

void Channels::fireChannelOpen(const ChannelPtr& channel) {
    // Notify the parent handler.
    const ChannelPtr& parent = channel->getParent();

    if (parent) {
        fireChildChannelStateChanged(parent, channel);
    }

    channel->getPipeline()->sendUpstream(UpstreamChannelStateEvent(
                                           channel, ChannelState::OPEN, boost::any(true)));
}

void Channels::fireChannelOpen(ChannelHandlerContext& ctx) {
    ctx.sendUpstream(UpstreamChannelStateEvent(
                         ctx.getChannel(), ChannelState::OPEN, boost::any(true)));
}

void Channels::fireChannelBound(const ChannelPtr& channel, const SocketAddress& localAddress) {
    channel->getPipeline()->sendUpstream(UpstreamChannelStateEvent(
                                           channel, ChannelState::BOUND, boost::any(&localAddress)));
}

void Channels::fireChannelBound(ChannelHandlerContext& ctx, const SocketAddress& localAddress) {
    ctx.sendUpstream(UpstreamChannelStateEvent(
                         ctx.getChannel(), ChannelState::BOUND, boost::any(&localAddress)));
}

void Channels::fireChannelConnected(const ChannelPtr& channel, const SocketAddress& remoteAddress) {
    channel->getPipeline()->sendUpstream(UpstreamChannelStateEvent(
                                           channel, ChannelState::CONNECTED, boost::any(&remoteAddress)));
}

void Channels::fireChannelConnected(ChannelHandlerContext& ctx, const SocketAddress& remoteAddress) {
    ctx.sendUpstream(UpstreamChannelStateEvent(
                         ctx.getChannel(), ChannelState::CONNECTED, boost::any(&remoteAddress)));
}

void Channels::fireMessageReceived(const ChannelPtr& channel, const ChannelMessage& message) {
    channel->getPipeline()->sendUpstream(
        UpstreamMessageEvent(channel, message, channel->getRemoteAddress()));
}

void Channels::fireMessageReceived(const ChannelPtr& channel, const ChannelMessage& message, const SocketAddress& remoteAddress) {
    channel->getPipeline()->sendUpstream(
        UpstreamMessageEvent(channel, message, remoteAddress));
}

void Channels::fireMessageReceived(ChannelHandlerContext& ctx, const ChannelMessage& message) {
    const ChannelPtr& channel = ctx.getChannel();
    ctx.sendUpstream(
        UpstreamMessageEvent(channel, message, channel->getRemoteAddress()));
}

void Channels::fireMessageReceived(ChannelHandlerContext& ctx, const ChannelMessage& message, const SocketAddress& remoteAddress) {
    ctx.sendUpstream(
        UpstreamMessageEvent(ctx.getChannel(), message, remoteAddress));
}

void Channels::fireWriteCompleted(const ChannelPtr& channel, long amount) {
    if (amount == 0) { return; }

    channel->getPipeline()->sendUpstream(DefaultWriteCompletionEvent(channel, amount));
}

void Channels::fireWriteCompleted(ChannelHandlerContext& ctx, long amount) {
    ctx.sendUpstream(DefaultWriteCompletionEvent(ctx.getChannel(), amount));
}

void Channels::fireChannelInterestChanged(const ChannelPtr& channel, int interestOps) {
    channel->getPipeline()->sendUpstream(UpstreamChannelStateEvent(
                                           channel, ChannelState::INTEREST_OPS, boost::any(interestOps)));
}

void Channels::fireChannelInterestChanged(ChannelHandlerContext& ctx, int interestOps) {
    ctx.sendUpstream(UpstreamChannelStateEvent(
                         ctx.getChannel(), ChannelState::INTEREST_OPS, boost::any(interestOps)));
}

void Channels::fireChannelDisconnected(const ChannelPtr& channel) {
    channel->getPipeline()->sendUpstream(UpstreamChannelStateEvent(
                                           channel, ChannelState::CONNECTED, boost::any()));
}

void Channels::fireChannelDisconnected(ChannelHandlerContext& ctx) {
    ctx.sendUpstream(UpstreamChannelStateEvent(
                         ctx.getChannel(), ChannelState::CONNECTED, boost::any()));
}

void Channels::fireChannelUnbound(const ChannelPtr& channel) {
    channel->getPipeline()->sendUpstream(UpstreamChannelStateEvent(
                                           channel, ChannelState::BOUND, boost::any()));
}

void Channels::fireChannelUnbound(ChannelHandlerContext& ctx) {
    ctx.sendUpstream(UpstreamChannelStateEvent(
                         ctx.getChannel(), ChannelState::BOUND, boost::any()));
}

void Channels::fireChannelClosed(const ChannelPtr& channel) {
    channel->getPipeline()->sendUpstream(UpstreamChannelStateEvent(
                                           channel, ChannelState::OPEN, boost::any()));

    // Notify the parent handler.
    const ChannelPtr& parent = channel->getParent();

    if (parent != NULL) {
        fireChildChannelStateChanged(parent, channel);
    }
}

void Channels::fireChannelClosed(ChannelHandlerContext& ctx) {
    ctx.sendUpstream(UpstreamChannelStateEvent(
                         ctx.getChannel(), ChannelState::OPEN, boost::any()));
}

void Channels::fireExceptionCaught(const ChannelPtr& channel, const Exception& cause) {
    channel->getPipeline()->sendUpstream(
        DefaultExceptionEvent(channel, cause));
}

void Channels::fireExceptionCaught(ChannelHandlerContext& ctx, const Exception& cause) {
    ctx.sendUpstream(DefaultExceptionEvent(ctx.getChannel(), cause));
}

ChannelFuturePtr Channels::bind(const ChannelPtr& channel, const SocketAddress& localAddress) {
    ChannelFuturePtr future = Channels::future(channel);
    channel->getPipeline()->sendDownstream(DownstreamChannelStateEvent(
            channel, future, ChannelState::BOUND, boost::any(localAddress)));
    return future;
}

void Channels::bind(ChannelHandlerContext& ctx, const ChannelFuturePtr& future, const SocketAddress& localAddress) {
    ctx.sendDownstream(DownstreamChannelStateEvent(
                           ctx.getChannel(), future, ChannelState::BOUND, boost::any(localAddress)));
}

ChannelFuturePtr Channels::unbind(const ChannelPtr& channel) {
    return channel->unbind();
}

void Channels::unbind(ChannelHandlerContext& ctx, const ChannelFuturePtr& future) {
    ctx.sendDownstream(DownstreamChannelStateEvent(
                           ctx.getChannel(), future, ChannelState::BOUND));
}

ChannelFuturePtr Channels::connect(const ChannelPtr& channel, const SocketAddress& remoteAddress) {
    ChannelFuturePtr future = Channels::future(channel, true);

    channel->getPipeline()->sendDownstream(DownstreamChannelStateEvent(
            channel, future, ChannelState::CONNECTED, boost::any(remoteAddress)));

    return future;
}

void Channels::connect(ChannelHandlerContext& ctx, const ChannelFuturePtr& future, const SocketAddress& remoteAddress) {
    ctx.sendDownstream(DownstreamChannelStateEvent(
                           ctx.getChannel(), future, ChannelState::CONNECTED, boost::any(remoteAddress)));
}

ChannelFuturePtr Channels::write(const ChannelPtr& channel,
                                 const ChannelMessage& message) {
    return channel->write(message);
}

void Channels::write(ChannelHandlerContext& ctx,
                     const ChannelFuturePtr& future,
                     const ChannelMessage& message) {
    write(ctx, future, message, ctx.getChannel()->getRemoteAddress());
}

ChannelFuturePtr Channels::write(const ChannelPtr& channel,
                                 const ChannelMessage& message,
                                 const SocketAddress& remoteAddress) {
    return channel->write(message, remoteAddress);
}

void Channels::write(ChannelHandlerContext& ctx,
                     const ChannelFuturePtr& future,
                     const ChannelMessage& message,
                     const SocketAddress& remoteAddress) {
    ctx.sendDownstream(
        DownstreamMessageEvent(ctx.getChannel(), future, message, remoteAddress));
}

ChannelFuturePtr Channels::setInterestOps(const ChannelPtr& channel, int interestOps) {
    return channel->setInterestOps(interestOps);
}

void Channels::setInterestOps(ChannelHandlerContext& ctx,
                              const ChannelFuturePtr& future,
                              int interestOps) {
    validateInterestOps(interestOps);
    interestOps = filterDownstreamInterestOps(interestOps);

    ctx.sendDownstream(DownstreamChannelStateEvent(
                           ctx.getChannel(), future, ChannelState::INTEREST_OPS, boost::any(interestOps)));
}

ChannelFuturePtr Channels::disconnect(const ChannelPtr& channel) {
    return channel->disconnect();
}

void Channels::disconnect(ChannelHandlerContext& ctx, const ChannelFuturePtr& future) {
    ctx.sendDownstream(DownstreamChannelStateEvent(
                           ctx.getChannel(), future, ChannelState::CONNECTED));
}

ChannelFuturePtr Channels::close(const ChannelPtr& channel) {
    return channel->close();
}

void Channels::close(ChannelHandlerContext& ctx, const ChannelFuturePtr& future) {
    ctx.sendDownstream(DownstreamChannelStateEvent(
                           ctx.getChannel(), future, ChannelState::OPEN));
}

void Channels::fireChildChannelStateChanged(const ChannelPtr& channel, const ChannelPtr& childChannel) {
    channel->getPipeline()->sendUpstream(
        DefaultChildChannelStateEvent(channel, childChannel));
}

void Channels::validateInterestOps(int interestOps) {
    switch (interestOps) {
    case Channel::OP_NONE:
    case Channel::OP_READ:
    case Channel::OP_WRITE:
    case Channel::OP_READ_WRITE:
        break;

    default:
        throw InvalidArgumentException(
            std::string("Invalid interestOps: ") + Integer::toString(interestOps));
    }
}

int Channels::filterDownstreamInterestOps(int interestOps) {
    return interestOps & (~Channel::OP_WRITE);
}

int Channels::validateAndFilterDownstreamInteresOps(int interestOps) {
    validateInterestOps(interestOps);
    return filterDownstreamInterestOps(interestOps);
}


}
}