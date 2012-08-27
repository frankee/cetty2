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

#include <cetty/channel/embedded/AbstractEmbeddedChannel.h>

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
    virtual ~EmbeddedBufferChannel() {}

    const ChannelBufferPtr& getLastOutboundBuffer() {
        return lastOutboundBuffer;
    }

    ChannelBufferPtr readOutbound() {
        if (!lastOutboundBuffer->readable()) {
            return ChannelBufferPtr();
        }

        ChannelBufferPtr buffer
            = lastOutboundBuffer->readBytes(lastOutboundBuffer->readableBytes());

        lastOutboundBuffer->clear();
        return buffer;
    }

    template<T>
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
        return getLastInboundChannelBuffer()->readable()
               || !getLastInboundMessageQueue().empty()
               || lastOutboundBuffer->readable();
    }

private:
    ChannelBufferPtr lastOutboundBuffer;
};

}
}
}

#endif //#if !defined(CETTY_CHANNEL_EMBEDDED_EMBEDDEDBUFFERCHANNEL_H)

// Local Variables:
// mode: c++
// End:
