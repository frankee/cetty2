#if !defined(CETTY_CHANNEL_CHANNELINBOUNDMESSAGEHANDLER_H)
#define CETTY_CHANNEL_CHANNELINBOUNDMESSAGEHANDLER_H

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

namespace cetty {
namespace channel {

template<typename H, typename InboundIn, typename InboundOut>
class ChannelInboudMessageHandler {
public:
    typedef ChannelMessageContainer<InboundIn, MESSAGE_BLOCK> InboundContainer;
    typedef ChannelMessageContainer<InboundOut, MESSAGE_BLOCK> NextInboundContainer;

    typedef ChannelMessageTransfer<InboundIn,
            InboundContainer,
            TRANSFER_INBOUND> InboundTransfer;

    typedef ChannelMessageHandlerContext<H,
            InboundIn,
            InboundOut,
            VoidMessage,
            VoidMessage,
            InboundContainer,
            NextInboundContainer,
            VoidMessageContainer,
            VoidMessageContainer> Context;
};

}
}

#endif //#if !defined(CETTY_CHANNEL_CHANNELINBOUNDMESSAGEHANDLER_H)

// Local Variables:
// mode: c++
// End:
