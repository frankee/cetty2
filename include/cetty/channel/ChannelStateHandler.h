#if !defined(CETTY_CHANNEL_CHANNELSTATEHANDLER_H)
#define CETTY_CHANNEL_CHANNELSTATEHANDLER_H

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

/**
 * ChannelHandler which only process the event
 *
 * <code>
 *      class EventHandler {
 *      public:
 *            typedef ChannelStateHandler<EventHandler>::Context Context;
 *      public:
 *            void registTo() {
 *            }
 *
 *            void channelActived(Context& ctx) {
 *            }
 *      }
 * </code>
 */
template<typename T>
class ChannelStateHandler {
public:
    typedef ChannelMessageHandlerContext<T,
            VoidMessage,
            VoidMessage,
            VoidMessage,
            VoidMessage,
            VoidMessageContainer,
            VoidMessageContainer,
            VoidMessageContainer,
            VoidMessageContainer> Context;

    //typedef ChannelHandlerWrapper<T>::Handler Handler;
    //typedef ChannelHandlerWrapper<T>::HandlerPtr HandlerPtr;
};

}
}

#endif //#if !defined(CETTY_CHANNEL_CHANNELSTATEHANDLER_H)

// Local Variables:
// mode: c++
// End:
