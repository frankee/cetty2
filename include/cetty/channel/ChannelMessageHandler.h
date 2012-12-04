#if !defined(CETTY_CHANNEL_CHANNELMESSAGEHANDLER_H)
#define CETTY_CHANNEL_CHANNELMESSAGEHANDLER_H

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

#include <cetty/channel/ChannelHandlerWrapper.h>
#include <cetty/channel/ChannelMessageHandlerContext.h>

namespace cetty {
namespace channel {

template<typename H,
    typename InboundIn,
    typename InboundOut,
    typename OutboundIn,
    typename OutboundOut>
class ChannelMessageHandler {
public:
    typedef typename ChannelHandlerWrapper<H>::Handler Handler;
    typedef typename ChannelHandlerWrapper<H>::HandlerPtr HandlerPtr;

    typedef ChannelMessageContainer<InboundIn, MESSAGE_BLOCK> InboundContainer;
    typedef ChannelMessageContainer<InboundOut, MESSAGE_BLOCK> NextInboundContainer;
    typedef ChannelMessageContainer<OutboundIn, MESSAGE_BLOCK> OutboundContainer;
    typedef ChannelMessageContainer<OutboundOut, MESSAGE_BLOCK> NextOutboundContainer;

    typedef typename InboundContainer::MessageQueue InboundQueue;
    typedef typename OutboundContainer::MessageQueue OutboudnQueue;

    typedef ChannelMessageTransfer<InboundOut, NextInboundContainer, TRANSFER_INBOUND> InboundTransfer;
    typedef ChannelMessageTransfer<OutboundOut, NextOutboundContainer, TRANSFER_OUTBOUND> OutboundTransfer;

    typedef ChannelMessageHandlerContext<H,
        InboundIn,
        InboundOut,
        OutboundIn,
        OutboundOut,
        InboundContainer,
        NextInboundContainer,
        OutboundContainer,
        NextOutboundContainer> Context;
        
};

}
}

#endif //#if !defined(CETTY_CHANNEL_CHANNELMESSAGEHANDLER_H)

// Local Variables:
// mode: c++
// End:
