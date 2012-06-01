#if !defined(QOTM_CLIENT_QUOTEOFTHEMOMENTCLIENTHANDLER_H)
#define QOTM_CLIENT_QUOTEOFTHEMOMENTCLIENTHANDLER_H

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

#include "cetty/channel/Channel.h"
#include "cetty/channel/ChannelMessage.h"
#include "cetty/channel/MessageEvent.h"
#include "cetty/channel/ExceptionEvent.h"
#include "cetty/channel/SimpleChannelUpstreamHandler.h"

using namespace cetty::channel;

/**
 * @author <a href="http://www.jboss.org/netty/">The Netty Project</a>
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 * @version $Rev: 2121 $, $Date: 2010-02-02 09:38:07 +0900 (Tue, 02 Feb 2010) $
 */

class QuoteOfTheMomentClientHandler : public cetty::channel::SimpleChannelUpstreamHandler {
public:
    QuoteOfTheMomentClientHandler() {}
    virtual ~QuoteOfTheMomentClientHandler() {}

    virtual void messageReceived(ChannelHandlerContext& ctx, const MessageEvent& e) {
        const std::string& msg = e.getMessage().value<std::string>();
        if (msg.find("QOTM: ") == 0) {
            printf("Quote of the Moment: %s\n", msg.substr(6).c_str());
            e.getChannel().close();
        }
    }

    virtual void exceptionCaught(ChannelHandlerContext& ctx, const ExceptionEvent& e) {
        //e.getCause().printStackTrace();
        e.getChannel().close();
    }

    virtual ChannelHandlerPtr clone() { return shared_from_this(); }
    virtual std::string toString() const { return "QuoteOfTheMomentClientHandler"; }
};

#endif //#if !defined(QOTM_CLIENT_QUOTEOFTHEMOMENTCLIENTHANDLER_H)
