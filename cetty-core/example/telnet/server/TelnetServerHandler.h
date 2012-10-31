#if !defined(TELNET_TELNETSERVERHANDLER_H)
#define TELNET_TELNETSERVERHANDLER_H

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

#include <boost/date_time/posix_time/ptime.hpp>

#include <cetty/buffer/Unpooled.h>
#include <cetty/channel/Channel.h>
#include <cetty/channel/ChannelInboundBufferHandlerAdapter.h>
#include <cetty/channel/ChannelFutureListener.h>
#include <cetty/channel/SocketAddress.h>
#include <cetty/logging/LoggerHelper.h>
#include <cetty/util/StringUtil.h>

using namespace cetty::channel;
using namespace cetty::util;
using namespace cetty::buffer;

/**
 * Handles a server-side channel.
 *
 * @author <a href="http://www.jboss.org/netty/">The Netty Project</a>
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 *
 * @version $Rev: 2121 $, $Date: 2010-02-02 09:38:07 +0900 (Tue, 02 Feb 2010) $
 */
class TelnetServerHandler : public ChannelInboundBufferHandlerAdapter<> {
public:
    TelnetServerHandler() {}
    virtual ~TelnetServerHandler() {}

    virtual void channelActive(ChannelHandlerContext& ctx) {
        // Send greeting for a new connection.
        std::string str;
        StringUtil::strprintf(&str, "Welcome to %s !\r\n",
                              ctx.getChannel()->getLocalAddress().toString().c_str());

        outboundTransfer.write(Unpooled::copiedBuffer(str),
                               ctx.getChannel()->newSucceededFuture());

        boost::posix_time::ptime now = boost::posix_time::second_clock::local_time();
        str.clear();
        StringUtil::strprintf(&str, "It is %s now.\r\n",
                              boost::posix_time::to_iso_string(now).c_str());

        outboundTransfer.write(Unpooled::copiedBuffer(str),
                               ctx.getChannel()->newSucceededFuture());
    }

    virtual void messageUpdated(ChannelHandlerContext& ctx) {

        ChannelBufferPtr buff = getInboundChannelBuffer();
        std::string request;
        buff->readBytes(&request);

        // Generate and write a response.
        std::string response;
        bool close = false;

        if (request.empty()) {
            response = "Please type something.\r\n";
        }
        else if (request.compare("bye") == 0) {
            response = "Have a good day!\r\n";
            close = true;
        }
        else {
            response = "Did you say '";
            response += request;
            response += "'?\r\n";
        }

        // We do not need to write a ChannelBuffer here.
        // We know the encoder inserted at TelnetPipelineFactory will do the conversion.
        ChannelFuturePtr future = ctx.getChannel()->newFuture();
        outboundTransfer.write(Unpooled::copiedBuffer(response),
                               future);

        // Close the connection after sending 'Have a good day!'
        // if the client has sent 'bye'.
        if (close) {
            future->addListener(ChannelFutureListener::CLOSE);
        }
    }

    virtual void exceptionCaught(ChannelHandlerContext& ctx) {
        LOG_WARN_E(e) << "Unexpected exception from downstream.";
        ctx.getChannel()->close();
    }

    virtual ChannelHandlerPtr clone() {
        return shared_from_this();
    }

    virtual std::string toString() const {
        return "TelentServerHandler";
    }
};


#endif //#if !defined(TELNET_TELNETSERVERHANDLER_H)
