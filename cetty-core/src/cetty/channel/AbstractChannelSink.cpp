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

#include <cetty/channel/AbstractChannelSink.h>

#include <cetty/channel/ChannelFuture.h>
#include <cetty/channel/AbstractChannel.h>
#include <cetty/channel/ChannelPipeline.h>
#include <cetty/channel/ChannelException.h>
#include <cetty/channel/ChannelPipelineException.h>

namespace cetty {
namespace channel {

AbstractChannelSink::AbstractChannelSink(AbstractChannel& channel)
    : channel(channel) {
}

AbstractChannelSink::~AbstractChannelSink() {
}

void AbstractChannelSink::bind(const SocketAddress& localAddress,
                               const ChannelFuturePtr& future) {
    if (!ensureOpen(future)) {
        return;
    }

    try {
        bool wasActive = channel.isActive();
        channel.doBind(localAddress);
        future->setSuccess();

        if (!wasActive && channel.isActive()) {
            channel.pipeline->fireChannelActive();
        }
    }
    catch (const std::exception& e) {
        //future->setFailure(e);
        //channel.pipeline->fireExceptionCaught(t);
        closeIfClosed();
    }
}

void AbstractChannelSink::disconnect(const ChannelFuturePtr& future) {
    try {
        bool wasActive = channel.isActive();
        channel.doDisconnect();
        future->setSuccess();

        if (wasActive && !channel.isActive()) {
            channel.pipeline->fireChannelInactive();
        }
    }
    catch (const std::exception& e) {
        //future.setFailure(t);
        closeIfClosed();
    }
}

void AbstractChannelSink::close(const ChannelFuturePtr& future) {
    bool wasActive = channel.isActive();

    if (channel.setClosed()) {
        try {
            channel.doClose();
            future->setSuccess();
        }
        catch (const std::exception& e) {
            //future->setFailure(t);
        }

        //if (closedChannelException != null) {
        //    closedChannelException = new ClosedChannelException();
        //}

        //notifyFlushFutures(closedChannelException);

        if (wasActive && !channel.isActive()) {
            //LOG_INFO(logger, "closed the socket channel, finally firing channel closed event.");
            channel.pipeline->fireChannelInactive();
        }
    }
    else {
        // Closed already.
        future->setSuccess();
    }
}

bool AbstractChannelSink::ensureOpen(const ChannelFuturePtr& future) {
    if (channel.isOpen()) {
        return true;
    }

    //Exception e = new ClosedChannelException();
    //future.setFailure(e);
    //pipeline.fireExceptionCaught(e);
    return false;
}

void AbstractChannelSink::closeIfClosed() {
    if (channel.isOpen()) {
        return;
    }

    //close(voidFuture());
}

void AbstractChannelSink::connect(const SocketAddress& remoteAddress, const SocketAddress& localAddress, const ChannelFuturePtr& future) {
    throw ChannelException("has not implement this method.");
}

void AbstractChannelSink::flush(const ChannelBufferPtr& buffer,
                                const ChannelFuturePtr& future) {
    throw ChannelException("has not implement this method.");
}

}
}