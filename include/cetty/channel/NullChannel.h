#if !defined(CETTY_CHANNEL_NULLCHANNEL_H)
#define CETTY_CHANNEL_NULLCHANNEL_H

/*
 * Copyright (c) 2010-2011 frankee zhou (frankee.zhou at gmail dot com)
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

#include <cetty/channel/Channel.h>

namespace cetty {
namespace channel {

class NullChannel : public Channel {
public:
    NullChannel();
    virtual ~NullChannel() {}

    virtual int id() const;

    virtual const ChannelPtr& parent() const;

    virtual const EventLoopPtr& eventLoop() const;

    virtual const ChannelFactoryPtr&  getFactory() const;
    virtual const ChannelPipelinePtr& pipeline() const;

    virtual ChannelConfig& config();
    virtual const ChannelConfig& config() const;

    virtual const SocketAddress& localAddress() const;
    virtual const SocketAddress& remoteAddress() const;

    virtual ChannelFuturePtr bind(const SocketAddress& localAddress);

    virtual ChannelFuturePtr connect(const SocketAddress& remoteAddress);

    virtual ChannelFuturePtr connect(const SocketAddress& remoteAddress,
                                     const SocketAddress& localAddress);

    virtual ChannelFuturePtr disconnect();
    virtual ChannelFuturePtr close();
    virtual ChannelFuturePtr flush();

    virtual const ChannelFuturePtr& bind(const SocketAddress& localAddress,
                                         const ChannelFuturePtr& future);

    virtual const ChannelFuturePtr& connect(const SocketAddress& remoteAddress,
                                            const ChannelFuturePtr& future);

    virtual const ChannelFuturePtr& connect(const SocketAddress& remoteAddress,
                                            const SocketAddress& localAddress,
                                            const ChannelFuturePtr& future);
    virtual const ChannelFuturePtr& disconnect(const ChannelFuturePtr& future);
    virtual const ChannelFuturePtr& close(const ChannelFuturePtr& future);

    virtual const ChannelFuturePtr& flush(const ChannelFuturePtr& future);

    virtual bool isOpen() const;
    virtual bool isActive() const;

    virtual ChannelFuturePtr newFuture();
    virtual ChannelFuturePtr newFailedFuture(const Exception& e);
    virtual ChannelFuturePtr newSucceededFuture();
    virtual const ChannelFuturePtr& closeFuture();

    virtual std::string toString() const;

    virtual int compareTo(const ChannelPtr& c) const;

public:
    static const ChannelPtr& instance();

private:
    static ChannelPtr nullChannel;
    static ChannelFuturePtr failedFuture;

private:
    EventLoopPtr eventLoop;

    ChannelPtr fatherChannel;
};

}
}

#endif //#if !defined(CETTY_CHANNEL_NULLCHANNEL_H)

// Local Variables:
// mode: c++
// End:
