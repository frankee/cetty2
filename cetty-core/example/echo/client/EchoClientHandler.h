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
#include <cetty/channel/ChannelInboundBufferHandlerAdapter.h>
#include <cetty/channel/ChannelHandlerContext.h>
#include <cetty/buffer/ChannelBuffer.h>
#include <cetty/buffer/ChannelBuffers.h>
#include <cetty/util/Exception.h>
#include <cetty/logging/LoggerHelper.h>

using namespace cetty::channel;
using namespace cetty::buffer;
using namespace cetty::util;

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
class EchoClientHandler : public ChannelInboundBufferHandlerAdapter<> {
public:
    /**
     * Creates a client-side handler.
     */
    EchoClientHandler(int firstMessageSize);
    virtual ~EchoClientHandler() {}

    int getTransferredBytes() const {
        return transferredBytes;
    }

    virtual void channelActive(ChannelHandlerContext& ctx);

    virtual void messageUpdated(ChannelHandlerContext& ctx);

    virtual void exceptionCaught(ChannelHandlerContext& ctx, const ChannelException& e);

    virtual ChannelHandlerPtr clone();

    virtual std::string toString() const;

private:
    int firstMessageSize;

    ChannelBufferPtr firstMessage;
    int transferredBytes;
};
