#if !defined(TELNET_TELNETCLIENTHANDLER_H)
#define TELNET_TELNETCLIENTHANDLER_H

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

#include <cetty/channel/Channel.h>
#include <cetty/channel/ChannelHandlerContext.h>
#include <cetty/channel/ChannelInboundBufferHandlerAdapter.h>

#include <cetty/logging/LoggerHelper.h>

using namespace cetty::channel;

/**
 * Handles a client-side channel.
 *
 * @author <a href="http://www.jboss.org/netty/">The Netty Project</a>
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 *
 * @version $Rev: 2121 $, $Date: 2010-02-02 09:38:07 +0900 (Tue, 02 Feb 2010) $
 */

class TelnetClientHandler : public ChannelInboundBufferHandlerAdapter<> {
public:
    TelnetClientHandler() {}
    virtual ~TelnetClientHandler() {}

    virtual ChannelHandlerPtr clone() {
        return shared_from_this();
    }

    virtual std::string toString() const {
        return "TelnetClientHandler";
    }

    virtual void messageUpdated(ChannelHandlerContext& ctx) {
        // Print out the line received from the server.
        std::string str;
        getInboundChannelBuffer()->readBytes(&str);
        printf("%s\n->", str.c_str());
    }

    virtual void exceptionCaught(ChannelHandlerContext& ctx,
        const ChannelException& e) {
        LOG_WARN << "Unexpected exception (" << e.getMessage() << ") from inbound.";
        ctx.close();
    }
};

#endif //#if !defined(TELNET_TELNETCLIENTHANDLER_H)
