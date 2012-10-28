#if !defined(DISCARD_DISCARDSERVERHANDLER_H)
#define DISCARD_DISCARDSERVERHANDLER_H

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

#include <cetty/channel/Channel.h>
#include <cetty/channel/ChannelConfig.h>
#include <cetty/channel/ChannelInboundBufferHandler.h>
#include <cetty/channel/ChannelInboundBufferHandlerContext.h>
#include <cetty/channel/ChannelInboundBufferHandlerAdapter.h>
#include <cetty/buffer/ChannelBuffer.h>

using namespace cetty::channel;
using namespace cetty::buffer;

/**
 * Handles a server-side channel.
 *
 * @author <a href="http://www.jboss.org/netty/">The Netty Project</a>
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 *
 * @version $Rev: 2121 $, $Date: 2010-02-02 09:38:07 +0900 (Tue, 02 Feb 2010) $
 */
class DiscardServerHandler : public ChannelInboundBufferHandlerAdapter<> {
public:
    DiscardServerHandler() {}
    virtual ~DiscardServerHandler() {}

    virtual ChannelHandlerPtr clone() {
        return shared_from_this();
    }

    virtual void messageReceived(ChannelHandlerContext& ctx) {
        // Discard received data silently by doing nothing.
        getInboundChannelBuffer()->clear();
    }

    virtual void exceptionCaught(ChannelHandlerContext& ctx, const ChannelException& e) {
        // Close the connection when an exception is raised.
        LOG_WARN << "Unexpected exception (" << e.what() << ") from downstream.";
        ctx.close();
    }

    virtual std::string toString() const {
        return "DiscardServerHandler";
    }
};


#endif //#if !defined(DISCARD_DISCARDSERVERHANDLER_H)
