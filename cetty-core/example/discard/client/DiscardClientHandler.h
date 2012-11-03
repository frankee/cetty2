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

#include <cetty/buffer/Unpooled.h>
#include <cetty/buffer/ChannelBuffer.h>

#include <cetty/channel/Channel.h>
#include <cetty/channel/ChannelInboundBufferHandlerAdapter.h>

#include <cetty/util/Exception.h>
#include <cetty/util/StringUtil.h>

#include <cetty/logging/LoggerHelper.h>

using namespace cetty::channel;
using namespace cetty::buffer;
using namespace cetty::util;

/**
 * Handles a client-side channel.
 *
 * @author <a href="http://www.jboss.org/netty/">The Netty Project</a>
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 *
 * @version $Rev: 2121 $, $Date: 2010-02-02 09:38:07 +0900 (Tue, 02 Feb 2010) $
 */
class DiscardClientHandler
        : public ChannelInboundBufferHandlerAdapter<> {
public:
    DiscardClientHandler(int messageSize)
        : context() {
        if (messageSize <= 0) {
            throw InvalidArgumentException(
                StringUtil::strprintf("messageSize: %d", messageSize));
        }

        content = Unpooled::buffer(messageSize);
    }
    virtual ~DiscardClientHandler() {}

    virtual void channelActive(ChannelHandlerContext& ctx) {
        this->context = &ctx;
        generateTraffic();
    }

    virtual void messageUpdated(ChannelHandlerContext& ctx) {
        // Server is supposed to send nothing.  Therefore, do nothing.
    }

    virtual void exceptionCaught(ChannelHandlerContext ctx,
                                 const ChannelException& cause) {
        // Close the connection when an exception is raised.
        LOG_WARN << "Unexpected exception (" << cause.what() << ") from downstream.",
                 ctx.close();
    }

    //virtual void writeCompleted(ChannelHandlerContext& ctx) {
    //    generateTraffic();
    //}

    virtual ChannelHandlerPtr clone() {
        return new DiscardClientHandler(content->readableBytes());
    }

    virtual std::string toString() const { return "DiscardClientHandler"; }

private:
    void generateTraffic() {
        // Fill the outbound buffer up to 64KiB
        ChannelBufferPtr out = Unpooled::buffer(65536);

        while (out->readableBytes() < 65536) {
            out->writeBytes(content);
        }

        // Flush the outbound buffer to the socket.
        // Once flushed, generate the same amount of traffic again.
        context->flush();
    }

private:
    int transferredBytes;
    ChannelBufferPtr content;
    ChannelHandlerContext* context;
};
