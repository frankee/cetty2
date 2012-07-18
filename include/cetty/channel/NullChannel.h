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

    virtual int getId() const;
    virtual int hashCode() const;

    virtual const ChannelPtr& getParent() const;

    virtual const ChannelFactoryPtr&  getFactory() const;
    virtual const ChannelPipelinePtr& getPipeline() const;

    virtual ChannelConfig& getConfig();
    virtual const ChannelConfig& getConfig() const;

    virtual const SocketAddress& getLocalAddress() const;
    virtual const SocketAddress& getRemoteAddress() const;

    virtual ChannelFuturePtr write(const ChannelMessage& message);
    virtual void write(const ChannelMessage& message,
                        ChannelFuturePtr* future);

    virtual ChannelFuturePtr write(const ChannelMessage& message,
                                   const SocketAddress& remoteAddress);

    virtual void write(const ChannelMessage& message,
                        const SocketAddress& remoteAddress,
                        ChannelFuturePtr* future);

    virtual ChannelFuturePtr bind(const SocketAddress& localAddress);
    virtual ChannelFuturePtr connect(const SocketAddress& remoteAddress);
    virtual ChannelFuturePtr disconnect();
    virtual ChannelFuturePtr unbind();
    virtual ChannelFuturePtr close();

    virtual ChannelFuturePtr setInterestOps(int interestOps);
    virtual ChannelFuturePtr setReadable(bool readable);

    virtual ChannelFuturePtr& getCloseFuture();
    virtual ChannelFuturePtr& getSucceededFuture();

    virtual bool isOpen() const;
    virtual bool isBound() const;
    virtual bool isConnected() const;

    virtual int  getInterestOps() const;
    virtual bool isReadable() const;
    virtual bool isWritable() const;

    virtual std::string toString() const;

    virtual int compareTo(const ChannelPtr& c) const;

public:
    static const ChannelPtr& getInstance();

private:
    static ChannelFuturePtr failedFuture;
    static ChannelPtr nullChannel;

private:
    ChannelPtr fatherChannel;
};

}
}

#endif //#if !defined(CETTY_CHANNEL_NULLCHANNEL_H)

// Local Variables:
// mode: c++
// End:
