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

#include <cetty/channel/ChannelSinkHandler.h>

#include <cetty/channel/ChannelFuture.h>
#include <cetty/channel/AbstractChannel.h>
#include <cetty/channel/ChannelPipeline.h>
#include <cetty/channel/ChannelException.h>
#include <cetty/channel/ChannelPipelineException.h>

#include <cetty/logging/LoggerHelper.h>

namespace cetty {
namespace channel {

void ChannelSinkHandler::bind(ChannelHandlerContext& ctx,
                              const SocketAddress& localAddress,
                              const ChannelFuturePtr& future) {
    ensureChannelSet(ctx);

    if (!ensureOpen(future)) {
        return;
    }

    try {
        bool wasActive = channel->isActive();
        channel->doBind(localAddress);
        future->setSuccess();

        if (!wasActive && channel->isActive()) {
            channel->getPipeline()->fireChannelActive();
        }
    }
    catch (const std::exception& e) {
        //future->setFailure(e);
        //channel.pipeline->fireExceptionCaught(t);
        closeIfClosed();
    }
}

void ChannelSinkHandler::connect(ChannelHandlerContext& ctx,
                                 const SocketAddress& remoteAddress,
                                 const SocketAddress& localAddress,
                                 const ChannelFuturePtr& future) {
    throw ChannelException("has not implement this method.");
}

void ChannelSinkHandler::disconnect(ChannelHandlerContext& ctx,
                                    const ChannelFuturePtr& future) {
    ensureChannelSet(ctx);

    try {
        bool wasActive = channel->isActive();
        channel->doDisconnect();
        future->setSuccess();

        if (wasActive && !channel->isActive()) {
            channel->getPipeline()->fireChannelInactive();
        }
    }
    catch (const std::exception& e) {
        //future.setFailure(t);
        closeIfClosed();
    }
}

void ChannelSinkHandler::close(ChannelHandlerContext& ctx,
                               const ChannelFuturePtr& future) {
    ensureChannelSet(ctx);
    bool wasActive = channel->isActive();

    if (channel->setClosed()) {
        try {
            channel->doClose();
            future->setSuccess();
        }
        catch (const std::exception& e) {
            //future->setFailure(t);
        }

        //if (closedChannelException != null) {
        //    closedChannelException = new ClosedChannelException();
        //}

        //notifyFlushFutures(closedChannelException);

        if (wasActive && !channel->isActive()) {
            //LOG_INFO(logger, "closed the socket channel, finally firing channel closed event.");
            channel->getPipeline()->fireChannelInactive();

            channel->closeFuture.reset();
            channel->succeededFuture.reset();

            ChannelPipelinePtr pipeline = channel->getPipeline();
            channel->pipeline.reset();
            pipeline->detach();
        }
    }
    else {
        // Closed already.
        future->setSuccess();
    }
}

void ChannelSinkHandler::flush(ChannelHandlerContext& ctx,
                               const ChannelFuturePtr& future) {
    throw ChannelException("has not implement this method.");
}

void ChannelSinkHandler::exceptionCaught(ChannelHandlerContext& ctx,
        const ChannelException& cause) {
    LOG_WARN << "no handler process the outbound exception";
}

void ChannelSinkHandler::userEventTriggered(ChannelHandlerContext& ctx,
        const boost::any& evt) {
    LOG_WARN << "no handler process the outbound user event";
}

cetty::channel::ChannelHandlerPtr ChannelSinkHandler::clone() {
    return ChannelHandlerPtr(new ChannelSinkHandler());
}

std::string ChannelSinkHandler::toString() const {
    return "HeadHandler";
}

bool ChannelSinkHandler::ensureOpen(const ChannelFuturePtr& future) {
    if (future->getChannel()->isOpen()) {
        return true;
    }

    //Exception e = new ClosedChannelException();
    //future.setFailure(e);
    //pipeline.fireExceptionCaught(e);
    return false;
}

void ChannelSinkHandler::closeIfClosed() {
    if (channel->isOpen()) {
        return;
    }

    //close(voidFuture());
}

void ChannelSinkHandler::ensureChannelSet(ChannelHandlerContext& ctx) {
    if (!channel) {
        channel = boost::dynamic_pointer_cast<AbstractChannel>(ctx.getChannel());
    }

    BOOST_ASSERT(channel);
}

}
}
