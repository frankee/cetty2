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
#include <boost/thread.hpp>

#include "cetty/channel/Channel.h"
#include "cetty/channel/ChannelMessage.h"
#include "cetty/channel/SimpleChannelUpstreamHandler.h"
#include "cetty/channel/ChannelHandlerContext.h"
#include "cetty/channel/MessageEvent.h"
#include "cetty/channel/SocketAddress.h"
#include "cetty/util/Random.h"

using namespace cetty::channel;
using namespace cetty::util;

/**
 * @author <a href="http://www.jboss.org/netty/">The Netty Project</a>
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 * @version $Rev: 2121 $, $Date: 2010-02-02 09:38:07 +0900 (Tue, 02 Feb 2010) $
 */
class QuoteOfTheMomentServerHandler : public SimpleChannelUpstreamHandler {
public:
    QuoteOfTheMomentServerHandler() {}
    virtual ~QuoteOfTheMomentServerHandler() {}

    virtual ChannelHandlerPtr clone() { return shared_from_this(); }
    virtual std::string toString() const { return "QuoteOfTheMomentServerHandler"; }

    virtual void messageReceived(ChannelHandlerContext& ctx, const MessageEvent& e) {
        const std::string& msg = e.getMessage().value<std::string>();
        if (msg.compare("QOTM?") == 0) {
            std::string qotem("QOTM: ");
            qotem += nextQuote();
            qotem += e.getRemoteAddress().toString();
            e.getChannel().write(qotem, e.getRemoteAddress());
        }
    }

    virtual void exceptionCaught(ChannelHandlerContext& ctx, const ExceptionEvent& e) {
        //e.getCause().printStackTrace();
        // We don't close the channel because we can keep serving requests.
    }

private:
    static const char* nextQuote();

private:
    static boost::mutex mutex;
    static Random random;
};
