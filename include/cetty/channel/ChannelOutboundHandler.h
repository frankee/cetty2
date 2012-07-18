#if !defined(CETTY_CHANNEL_CHANNELOUTBOUNDHANDLER_H)
#define CETTY_CHANNEL_CHANNELOUTBOUNDHANDLER_H

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

#include <cetty/channel/ChannelHandler.h>
#include <cetty/channel/ChannelFutureFwd.h>

namespace cetty {
namespace channel {

class SocketAddress;

class ChannelOutboundHandler : public virtual ChannelHandler {
public:
    virtual void bind(ChannelHandlerContext& ctx,
                      const SocketAddress& localAddress,
                      const ChannelFuturePtr& future) = 0;

    virtual void connect(ChannelHandlerContext& ctx,
                         const SocketAddress& remoteAddress,
                         const SocketAddress& localAddress,
                         const ChannelFuturePtr& future) = 0;

    virtual void disconnect(ChannelHandlerContext& ctx,
                            const ChannelFuturePtr& future) = 0;

    virtual void close(ChannelHandlerContext& ctx,
                       const ChannelFuturePtr& future) = 0;

    virtual void flush(ChannelHandlerContext& ctx,
                       const ChannelFuturePtr& future) = 0;
};

}
}

#endif //#if !defined(CETTY_CHANNEL_CHANNELOUTBOUNDHANDLER_H)

// Local Variables:
// mode: c++
// End:
