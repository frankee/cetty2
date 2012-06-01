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

#include "boost/thread/mutex.hpp"
#include "cetty/buffer/ChannelBuffer.h"
#include "cetty/channel/Channel.h"
#include "cetty/channel/ChannelEvent.h"
#include "cetty/channel/ChannelState.h"
#include "cetty/channel/MessageEvent.h"
#include "cetty/channel/ChannelMessage.h"
#include "cetty/channel/ChannelStateEvent.h"
#include "cetty/channel/ExceptionEvent.h"
#include "cetty/channel/ChannelHandler.h"
#include "cetty/channel/ChannelHandlerContext.h"
#include "cetty/channel/SimpleChannelUpstreamHandler.h"
#include "cetty/logging/InternalLogger.h"

using namespace cetty::channel;
using namespace cetty::buffer;
using namespace cetty::logging;

/**
 * Handles a server-side channel.
 *
 * @author <a href="http://www.jboss.org/netty/">The Netty Project</a>
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 *
 * @version $Rev: 2121 $, $Date: 2010-02-02 09:38:07 +0900 (Tue, 02 Feb 2010) $
 */
class DiscardServerHandler : public SimpleChannelUpstreamHandler {
public:
    DiscardServerHandler() : transferredBytes(0) {}
    virtual ~DiscardServerHandler() {}


    int getTransferredBytes() const {
        //boost::lock_guard<boost::mutex> guard(mutex);
        return transferredBytes;
    }

    virtual ChannelHandlerPtr clone() {
        return shared_from_this();
    }

    virtual void channelStateChanged(ChannelHandlerContext& ctx, const ChannelStateEvent& e) {
        logger->info(e.toString());
        SimpleChannelUpstreamHandler::channelStateChanged(ctx, e);
    }

    virtual void messageReceived(ChannelHandlerContext& ctx, const MessageEvent& e) {
        // Discard received data silently by doing nothing.
        const ChannelBufferPtr& buffer = e.getMessage().value<ChannelBufferPtr>();
        int readableBytes = buffer->readableBytes();

        {
            boost::lock_guard<boost::mutex> guard(mutex);
            transferredBytes += readableBytes;
        }
        
        buffer->clear();
    }

    virtual void exceptionCaught(ChannelHandlerContext& ctx, const ExceptionEvent& e) {
        // Close the connection when an exception is raised.
        logger->warn(
                "Unexpected exception from downstream.",
                e.getCause());
        e.getChannel().close();
    }

    virtual std::string toString() const {
        return "DiscardServerHandler";
    }

private:
    static InternalLogger* logger;
    
    boost::mutex mutex;
    int transferredBytes;
};


#endif //#if !defined(DISCARD_DISCARDSERVERHANDLER_H)
