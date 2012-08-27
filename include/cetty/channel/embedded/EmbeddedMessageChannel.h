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

namespace cetty {
namespace channel {
namespace embedded {

template<typename InboundOutT,
         typename OutboundOutT>
class EmbeddedMessageChannel
    : public AbstractEmbeddedChannel<InboundOutT, OutboundOutT> {
public:
    virtual ~EmbeddedMessageChannel() {}

    std::deque<OutboundOutT>& getLastOutboundBuffer() {
        return lastOutboundQueue;
    }

    OutboundOutT readOutbound() {
        OutboundOutT out = lastOutboundQueue.front();
        lastOutboundQueue.pop_front();
        return out;
    }

    template<T>
    bool writeOutbound(const T& msg) {
        write(msg);
        checkException();
        return !lastOutboundBuffer.empty();
    }

    template<>
    bool writeOutbound(const ChannelBufferPtr& msg) {
        write(msg);
        checkException();
        return lastOutboundBuffer.empty();
    }

    bool finish() {
        close();
        checkException();
        return getLastInboundChannelBuffer()->readable()
            || !getLastInboundMessageQueue().empty()
            || !lastOutboundQueue.empty();
    }

private:
    std::deque<OutboundOutT> lastOutboundQueue;

};

}
}
}

#endif //#if !defined(CETTY_CHANNEL_EMBEDDED_EMBEDDEDMESSAGECHANNEL_H)

// Local Variables:
// mode: c++
// End:
