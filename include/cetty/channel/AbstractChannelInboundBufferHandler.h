#if !defined(CETTY_CHANNEL_ABSTRACTCHANNELINBOUNDBUFFERHANDLER_H)
#define CETTY_CHANNEL_ABSTRACTCHANNELINBOUNDBUFFERHANDLER_H

/*
 * Copyright (c) 2010-2012 frankee zhou (frankee.zhou at gmail dot com)
 *
 * Distributed under under the Apache License, version 2.0 (the "License").
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 */

#include <cetty/channel/ChannelInboundBufferHandler.h>

#include <cetty/channel/VoidChannelMessage.h>
#include <cetty/channel/ChannelPipelineMessageTransfer.h>

namespace cetty {
namespace channel {

using namespace cetty::buffer;

template<typename InboundOutT = ChannelBufferPtr,
         typename InboundContext = ChannelInboundBufferHandlerContext,
         typename OutboundOutT = ChannelBufferPtr,
         typename OutboundContext = ChannelOutboundBufferHandlerContext>
class AbstractChannelInboundBufferHandler : public ChannelInboundBufferHandler {
public:
    typedef InboundContext NextInboundContext;
    typedef OutboundContext NextOutboundContext;

public:
    virtual ~AbstractChannelInboundBufferHandler() {}

protected:
    ChannelPipelineMessageTransfer<InboundOutT, NextInboundContext> inboundTransfer;
    ChannelPipelineMessageTransfer<OutboundOutT, NextOutboundContext> outboundTransfer;
};

//template<>
//class AbstractChannelInboundBufferHandler<ChannelBufferPtr, ChannelInboundBufferHandlerContext> {
//public:
//};

}
}

#endif //#if !defined(CETTY_CHANNEL_ABSTRACTCHANNELINBOUNDBUFFERHANDLER_H)

// Local Variables:
// mode: c++
// End:
