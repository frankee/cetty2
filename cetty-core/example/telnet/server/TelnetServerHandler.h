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
#include <time.h>

#include "cetty/channel/Channel.h"
#include "cetty/channel/Channels.h"
#include "cetty/channel/ChannelMessage.h"
#include "cetty/channel/ChannelEvent.h"
#include "cetty/channel/MessageEvent.h"
#include "cetty/channel/ChannelStateEvent.h"
#include "cetty/channel/ExceptionEvent.h"
#include "cetty/channel/SimpleChannelUpstreamHandler.h"
#include "cetty/channel/ChannelFutureListener.h"
#include "cetty/channel/SocketAddress.h"
#include "cetty/logging/InternalLogger.h"

using namespace cetty::channel;
using namespace cetty::logging;

/**
 * Handles a server-side channel.
 *
 * @author <a href="http://www.jboss.org/netty/">The Netty Project</a>
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 *
 * @version $Rev: 2121 $, $Date: 2010-02-02 09:38:07 +0900 (Tue, 02 Feb 2010) $
 */
class TelnetServerHandler : public SimpleChannelUpstreamHandler {
public:
    TelnetServerHandler() {}
    virtual ~TelnetServerHandler() {}

    virtual void channelStateChanged(ChannelHandlerContext& ctx, const ChannelStateEvent& e) {
        logger->info(e.toString());
        SimpleChannelUpstreamHandler::channelStateChanged(ctx, e);
    }

    virtual void channelConnected(
            ChannelHandlerContext& ctx, const ChannelStateEvent& e) {
        // Send greeting for a new connection.
        std::string str("Welcome to ");
        str.append(e.getChannel().getLocalAddress().toString());
        str.append("!\r\n");
        e.getChannel().write(str);

        time_t now = time(0);
        str.clear();
        str += "It is ";
        str += ctime(&now);
		str[str.size() - 1] = ' '; //eat the last '\n' of the ctime string.
        str += "now.\r\n";
        e.getChannel().write(str);
    }

    virtual void messageReceived(
            ChannelHandlerContext& ctx, const MessageEvent& e) {

        // Cast to a String first.
        // We know it is a String because we put some codec in TelnetPipelineFactory.
        if (!e.getMessage().isString()) {
            // log here.
        }

        std::string& request = e.getMessage().value<std::string>();

        // Generate and write a response.
        std::string response;
        bool close = false;
        
        if (request.length() == 0) {
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
        ChannelFuturePtr future = e.getChannel().write(response);

        // Close the connection after sending 'Have a good day!'
        // if the client has sent 'bye'.
        if (close) {
            future->addListener(ChannelFutureListener::CLOSE);
        }
    }

    virtual void exceptionCaught(
            ChannelHandlerContext& ctx, const ExceptionEvent& e) {
        logger->warn(
                "Unexpected exception from downstream.",
                e.getCause());
        e.getChannel().close();
    }

private:
    static InternalLogger* logger;
};


#endif //#if !defined(TELNET_TELNETSERVERHANDLER_H)
