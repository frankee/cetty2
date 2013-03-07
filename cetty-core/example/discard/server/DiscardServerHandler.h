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
#include <cetty/channel/VoidChannelFuture.h>
#include <cetty/channel/ChannelInboundBufferHandler.h>
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
class DiscardServerHandler : private boost::noncopyable {
public:
    typedef ChannelInboudBufferHandler<DiscardServerHandler>::Context Context;
    typedef ChannelInboudBufferHandler<DiscardServerHandler>::InboundContainer InboundContainer;

public:
    DiscardServerHandler() {
    }

    ~DiscardServerHandler() {}

    void registerTo(Context& ctx) {
        container_ = ctx.inboundContainer();

        ctx.setChannelMessageUpdatedCallback(
            boost::bind(&DiscardServerHandler::messageUpdated,
            this,
            _1));
    }

    void messageUpdated(ChannelHandlerContext& ctx) {
        const ChannelBufferPtr& buffer = container_->getMessages();

        if (buffer) {
            buffer->clear();
        }
    }

private:
    InboundContainer* container_;
};

#endif //#if !defined(DISCARD_DISCARDSERVERHANDLER_H)
