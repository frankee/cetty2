/*
 * Copyright 2009 Red Hat, Inc.
 *
 * Red Hat licenses this file to you under the Apache License, version 2.0
 * (the "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at:
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 */
/*
 * Copyright (c) 2010-2011 frankee zhou (frankee.zhou at gmail dot com)
 * Distributed under under the Apache License, version 2.0 (the "License").
 */

#include <boost/thread/mutex.hpp>
#include <boost/thread/locks.hpp>

#include "cetty/buffer/ChannelBuffer.h"

#include "cetty/channel/Channel.h"
#include "cetty/channel/ChannelState.h"
#include "cetty/channel/Channels.h"
#include "cetty/channel/ChannelMessage.h"
#include "cetty/channel/MessageEvent.h"
#include "cetty/channel/ChannelEvent.h"
#include "cetty/channel/ExceptionEvent.h"
#include "cetty/channel/ChannelStateEvent.h"
#include "cetty/channel/WriteCompletionEvent.h"
#include "cetty/channel/ChannelHandler.h"
#include "cetty/channel/ChannelHandlerContext.h"
#include "cetty/channel/SimpleChannelUpstreamHandler.h"

#include "cetty/util/Exception.h"
#include "cetty/util/Integer.h"
#include "cetty/logging/InternalLogger.h"


using namespace cetty::channel;
using namespace cetty::buffer;
using namespace cetty::util;
using namespace cetty::logging;

/**
 * Handles a client-side channel.
 *
 * @author <a href="http://www.jboss.org/netty/">The Netty Project</a>
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 *
 * @version $Rev: 2121 $, $Date: 2010-02-02 09:38:07 +0900 (Tue, 02 Feb 2010) $
 */
class DiscardClientHandler : public cetty::channel::SimpleChannelUpstreamHandler {
public:
    DiscardClientHandler(int messageSize) : transferredBytes(0) {
        if (messageSize <= 0) {
            throw InvalidArgumentException(std::string("messageSize: ") +
                                           Integer::toString(messageSize));
        }
        content = ChannelBuffers::buffer(messageSize);
    }
    virtual ~DiscardClientHandler() {}

    int getTransferredBytes() const {
        return transferredBytes;
    }

    virtual void channelStateChanged(ChannelHandlerContext& ctx, const ChannelStateEvent& e) {
        logger->info(e.toString());
        SimpleChannelUpstreamHandler::channelStateChanged(ctx, e);
    }

    virtual void channelConnected(ChannelHandlerContext& ctx, const ChannelStateEvent& e) {
        // Send the initial messages.
        generateTraffic(e);
    }

    virtual void channelInterestChanged(ChannelHandlerContext& ctx, const ChannelStateEvent& e) {
        // Keep sending messages whenever the current socket buffer has room.
        generateTraffic(e);
    }

    virtual void messageReceived(ChannelHandlerContext& ctx, const MessageEvent& e) {
        // Server is supposed to send nothing.  Therefore, do nothing.
    }

    virtual void writeCompleted(ChannelHandlerContext& ctx, const WriteCompletionEvent& e) {
        //boost::lock_guard<boost::mutex> guard(mutex);
        transferredBytes += e.getWrittenAmount();
    }

    virtual void exceptionCaught(ChannelHandlerContext& ctx, const ExceptionEvent& e) {
        // Close the connection when an exception is raised.
        logger->warn(
                "Unexpected exception from downstream.",
                e.getCause());
        e.getChannel().close();
    }

    virtual ChannelHandlerPtr clone() { return shared_from_this(); }
    virtual std::string toString() const { return "DiscardClientHandler"; }

private:
    void generateTraffic(const ChannelStateEvent& e) {
        // Keep generating traffic until the channel is unwritable.
        // A channel becomes unwritable when its internal buffer is full.
        // If you keep writing messages ignoring this property,
        // you will end up with an OutOfMemoryError.
        Channel& channel = e.getChannel();
        while (channel.isWritable()) {
            ChannelBufferPtr m = nextMessage();
            if (m) {
                channel.write(m, false);
            }
            else {
                break;
            }
        }
    }

    ChannelBufferPtr nextMessage() {
        content->setIndex(0, content->capacity());
        return content;
    }

private:
    static InternalLogger* logger;

    int transferredBytes;
    ChannelBufferPtr content;
};
