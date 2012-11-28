#if !defined(CETTY_CHANNEL_CHANNELOUTBOUNDMESSAGEHANDLER_H)
#define CETTY_CHANNEL_CHANNELOUTBOUNDMESSAGEHANDLER_H

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

#include <cetty/channel/ChannelMessageHandlerContext.h>

namespace cetty {
namespace channel {

template<typename Handler, typename OutboundIn, typename OutboundOut>
class ChannelOutboundMessageHandler : private boost::noncopyable {
public:
    typedef ChannelMessageHandlerContext<Handler,
            VoidMessage,
            VoidMessage,
            OutboundIn,
            OutboundIn,
            VoidMessageContainer,
            VoidMessageContainer,
            ChannelMessageContainer<OutboundIn>,
            ChannelMessageContainer<OutboundOut> > Context;
};

}
}

#endif //#if !defined(CETTY_CHANNEL_CHANNELOUTBOUNDMESSAGEHANDLER_H)

// Local Variables:
// mode: c++
// End:
