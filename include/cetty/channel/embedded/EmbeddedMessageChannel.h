#if !defined(CETTY_CHANNEL_EMBEDDED_EMBEDDEDMESSAGECHANNEL_H)
#define CETTY_CHANNEL_EMBEDDED_EMBEDDEDMESSAGECHANNEL_H

/*
 * Copyright 2012 The Netty Project
 *
 * The Netty Project licenses this file to you under the Apache License,
 * version 2.0 (the "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at:
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations
 * under the License.
 */
/*
 * Copyright (c) 2010-2012 frankee zhou (frankee.zhou at gmail dot com)
 * Distributed under under the Apache License, version 2.0 (the "License").
 */

#include <cetty/channel/embedded/AbstractEmbeddedChannel.h>
#include <cetty/channel/ChannelOutboundMessageHandler.h>

namespace cetty {
namespace channel {
namespace embedded {

template<typename InboundOutT,
         typename OutboundOutT>
class EmbeddedMessageChannel
        : public AbstractEmbeddedChannel<InboundOutT, OutboundOutT> {
public:
    typedef ChannelOutboundMessageHandler<OutboundOutT> SinkHandler;

public:
    EmbeddedMessageChannel(const ChannelHandlerPtr& handler)
        : AbstractEmbeddedChannel(handler) {
    }

    EmbeddedMessageChannel(const ChannelHandlerPtr& handler1,
                           const ChannelHandlerPtr& handler2)
        : AbstractEmbeddedChannel(handler1, handler2) {
    }

    EmbeddedMessageChannel(const ChannelHandlerPtr& handler1,
                           const ChannelHandlerPtr& handler2,
                           const ChannelHandlerPtr& handler3)
        : AbstractEmbeddedChannel(handler1, handler2, handler3) {
    }

    EmbeddedMessageChannel(const ChannelPipelinePtr& pipeline)
        : AbstractEmbeddedChannel(pipeline) {
    }

    virtual ~EmbeddedMessageChannel() {}

    SinkHandler::MessageQueue& getLastOutboundMessageQueue() {
        boost::intrusive<SinkHandler> handler
            = boost::dynamic_pointer_cast<SinkHandler>(
                  pipeline->getSinkHandler());

        BOOST_ASSERT(handler);
        return handler->getOutboundMessageQueue();
    }

    OutboundOutT readOutbound() {
        SinkHandler::MessageQueue& lastOutboundQueue
            = getLastOutboundMessageQueue();

        if (!lastOutboundQueue.empty()) {
            OutboundOutT out = lastOutboundQueue.front();
            lastOutboundQueue.pop_front();
            return out;
        }

        return OutboundOutT();
    }

    template<T>
    bool writeOutbound(const T& msg) {
        write(msg);
        checkException();
        return !getLastOutboundMessageQueue().empty();
    }

    template<>
    bool writeOutbound(const ChannelBufferPtr& msg) {
        write(msg);
        checkException();
        return !lastInboundMessageQueue.empty();
    }

    bool finish() {
        close();
        checkException();
        return hasInboundOut()
               || !getLastOutboundMessageQueue().empty();
    }
};

}
}
}

#endif //#if !defined(CETTY_CHANNEL_EMBEDDED_EMBEDDEDMESSAGECHANNEL_H)

// Local Variables:
// mode: c++
// End:
