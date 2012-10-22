#if !defined(CETTY_CHANNEL_EMBEDDED_EMBEDDEDBUFFERCHANNEL_H)
#define CETTY_CHANNEL_EMBEDDED_EMBEDDEDBUFFERCHANNEL_H

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

#include <boost/intrusive_ptr.hpp>
#include <cetty/channel/embedded/AbstractEmbeddedChannel.h>
#include <cetty/channel/ChannelOutboundBufferHandler.h>

namespace cetty {
namespace channel {
namespace embedded {

/**
 * EmbeddedBufferChannel which OutboundOutT is ChannelBufferPtr type,
 * and its sink handler is ChannelOutboundBufferHandler using the default one in Channel Pipeline.
 */
template<typename InboundOutT>
class EmbeddedBufferChannel
        : public AbstractEmbeddedChannel<InboundOutT, ChannelBufferPtr> {
public:
    EmbeddedBufferChannel(const ChannelHandlerPtr& handler)
        : AbstractEmbeddedChannel(handler) {
    }

    EmbeddedBufferChannel(const ChannelHandlerPtr& handler1,
                          const ChannelHandlerPtr& handler2)
        : AbstractEmbeddedChannel(handler1, handler2) {
    }

    EmbeddedBufferChannel(const ChannelHandlerPtr& handler1,
                          const ChannelHandlerPtr& handler2,
                          const ChannelHandlerPtr& handler3)
        : AbstractEmbeddedChannel(handler1, handler2, handler3) {
    }

    EmbeddedBufferChannel(const ChannelPipelinePtr& pipeline)
        : AbstractEmbeddedChannel(pipeline) {
    }

    virtual ~EmbeddedBufferChannel() {}

    ChannelBufferPtr getLastOutboundBuffer() {
        boost::intrusive_ptr<ChannelOutboundBufferHandler> handler
            = boost::dynamic_pointer_cast<ChannelOutboundBufferHandler>(
            pipeline->getSinkHandler());
        
        BOOST_ASSERT(handler);
        return handler->getOutboundChannelBuffer();
    }

    ChannelBufferPtr readOutbound() {
        ChannelBufferPtr lastOutboundBuffer = getLastOutboundBuffer();
        if (!lastOutboundBuffer || !lastOutboundBuffer->readable()) {
            return ChannelBufferPtr();
        }

        ChannelBufferPtr buffer = lastOutboundBuffer->readBytes();
        lastOutboundBuffer->clear();
        return buffer;
    }

    template<typename T>
    bool writeOutbound(const T& msg) {
        write(msg);
        checkException();
        return !getLastOutboundBuffer()->readable();
    }

    template<>
    bool writeOutbound(const ChannelBufferPtr& msg) {
        write(msg);
        checkException();
        return getLastOutboundBuffer()->readable();
    }

    bool finish() {
        close();
        checkException();
        return hasInboundOut()
               || (getLastOutboundBuffer()->readable());
    }
};

}
}
}

#endif //#if !defined(CETTY_CHANNEL_EMBEDDED_EMBEDDEDBUFFERCHANNEL_H)

// Local Variables:
// mode: c++
// End:
