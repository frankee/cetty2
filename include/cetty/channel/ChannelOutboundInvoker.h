#if !defined(CETTY_CHANNEL_CHANNELOUTBOUNDINVOKER_H)
#define CETTY_CHANNEL_CHANNELOUTBOUNDINVOKER_H

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

#include <boost/any.hpp>
#include <cetty/channel/ChannelFuturePtr.h>

namespace cetty {
namespace channel {

class SocketAddress;

class ChannelOutboundInvoker {
public:
    virtual ~ChannelOutboundInvoker() {}

    /**
     * Binds this channel to the specified local address asynchronously.
     *
     * @param localAddress where to bind
     *
     * @return the {@link ChannelFuture ChannelFuturePtr} which will be
     *         notified when the bind request succeeds or fails
     *
     */
    virtual ChannelFuturePtr bind(const SocketAddress& localAddress) = 0;

    /**
     * Connects this channel to the specified remote address asynchronously.
     *
     * @param remoteAddress where to connect
     *
     * @return the {@link ChannelFuture ChannelFuturePtr} which will be
     *         notified when the connection request succeeds or fails
     *
     */
    virtual ChannelFuturePtr connect(const SocketAddress& remoteAddress) = 0;

    /**
     *
     */
    virtual ChannelFuturePtr connect(const SocketAddress& remoteAddress,
                                     const SocketAddress& localAddress) = 0;

    /**
     * Disconnects this channel from the current remote address asynchronously.
     *
     * @return the {@link ChannelFuture ChannelFuturePtr} which will be notified when the
     *         disconnection request succeeds or fails
     */
    virtual ChannelFuturePtr disconnect() = 0;

    /**
     * Closes this channel asynchronously.  If this channel is bound or
     * connected, it will be disconnected and unbound first.  Once a channel
     * is closed, it can not be open again.  Calling this method on a closed
     * channel has no effect.  Please note that this method always returns the
     * same future instance.
     *
     * @return the {@link ChannelFuture  ChannelFuturePtr} which will be notified when the
     *         close request succeeds or fails
     */
    virtual ChannelFuturePtr close() = 0;

    /**
     *
     */
    virtual ChannelFuturePtr flush() = 0;

    /**
     *
     */
    virtual const ChannelFuturePtr& bind(const SocketAddress& localAddress,
                                         const ChannelFuturePtr& future) = 0;

    /**
     *
     */
    virtual const ChannelFuturePtr& connect(const SocketAddress& remoteAddress,
                                            const ChannelFuturePtr& future) = 0;

    /**
     *
     */
    virtual const ChannelFuturePtr& connect(const SocketAddress& remoteAddress,
                                            const SocketAddress& localAddress,
                                            const ChannelFuturePtr& future) = 0;

    /**
     *
     */
    virtual const ChannelFuturePtr& disconnect(const ChannelFuturePtr& future) = 0;

    /**
     *
     */
    virtual const ChannelFuturePtr& close(const ChannelFuturePtr& future) = 0;

    /**
     *
     */
    virtual const ChannelFuturePtr& flush(const ChannelFuturePtr& future) = 0;
};

}
}

#endif //#if !defined(CETTY_CHANNEL_CHANNELOUTBOUNDINVOKER_H)

// Local Variables:
// mode: c++
// End:
