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
#include <string>

#include <boost/assert.hpp>
#include <boost/any.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>

#include <cetty/channel/Channel.h>
#include <cetty/channel/ChannelException.h>
#include <cetty/channel/ChannelInboundBufferHandler.h>
#include <cetty/channel/ChannelHandlerContext.h>
#include <cetty/buffer/ChannelBuffer.h>
#include <cetty/buffer/ChannelBuffers.h>
#include <cetty/util/Exception.h>
#include <cetty/util/Timer.h>
#include <cetty/util/Timeout.h>
#include <cetty/util/TimerFactory.h>
#include <cetty/logging/InternalLogger.h>

using namespace cetty::channel;
using namespace cetty::buffer;
using namespace cetty::util;
using namespace cetty::logging;

/**
 * Handler implementation for the echo client.  It initiates the ping-pong
 * traffic between the echo client and server by sending the first message to
 * the server.
 *
 * 
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 *
 * @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
 */
class EchoClientHandler : public ChannelInboundBufferHandler {
public:
    /**
     * Creates a client-side handler.
     */
    EchoClientHandler(int firstMessageSize, int intervalTime = 0)
        : transferredBytes(0),
          firstMessageSize(firstMessageSize),
          intervalTime(intervalTime) {

        if (firstMessageSize <= 0) {
            throw InvalidArgumentException("firstMessageSize must > 0.");
        }

        firstMessage = ChannelBuffers::buffer(firstMessageSize);
        BOOST_ASSERT(firstMessage);
        int capacity = firstMessage->writableBytes();

        for (int i = 0; i < capacity; i ++) {
            firstMessage->writeByte(i);
        }
    }

    virtual ~EchoClientHandler() {}

    int getTransferredBytes() const {
        return transferredBytes;
    }

    virtual void channelActive(ChannelHandlerContext& ctx);

    virtual void messageUpdated(ChannelInboundBufferHandlerContext& ctx);

    virtual void exceptionCaught(ChannelHandlerContext& ctx, const ChannelException& e) {
        // Close the connection when an exception is raised.
        logger->warn(
                "Unexpected exception from downstream.",
                e);
        ctx.close();
    }

    virtual ChannelHandlerPtr clone() {
        ChannelHandlerPtr ptr(
            new EchoClientHandler(firstMessageSize, intervalTime));
        return ptr;
    }

    virtual std::string toString() const {
        return "EchoClientHandler";
    }

    void delaySendMessage(Timeout& timeout, Channel& channel, const ChannelBufferPtr& buffer);

private:
    static InternalLogger* logger;
    
private:
    int firstMessageSize;
    int intervalTime;

    TimerPtr timer; 

    ChannelBufferPtr firstMessage;
    int transferredBytes;
};
