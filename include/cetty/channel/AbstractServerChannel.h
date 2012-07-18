#if !defined(CETTY_CHANNEL_ABSTRACTSERVERCHANNEL_H)
#define CETTY_CHANNEL_ABSTRACTSERVERCHANNEL_H

/*
 * Copyright 2009 Red Hat, Inc.
 *
 * Red Hat licenses this file to you under the Apache License, version 2.0
 * (the "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at:
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 */

/*
 * Copyright (c) 2010-2011 frankee zhou (frankee.zhou at gmail dot com)
 *
 * Distributed under under the Apache License, version 2.0 (the "License").
 *
 */

#include <cetty/channel/ServerChannel.h>
#include <cetty/channel/AbstractChannel.h>
#include <cetty/channel/Channels.h>

namespace cetty {
namespace channel {

class ChannelSink;

/**
 * A skeletal server-side {@link Channel} implementation.  A server-side
 * {@link Channel} does not allow the following operations:
 * <ul>
 * <li>{@link #connect(SocketAddress)}</li>
 * <li>{@link #disconnect()}</li>
 * <li>{@link #getInterestOps()}</li>
 * <li>{@link #setInterestOps(int)}</li>
 * <li>{@link #write(Object)}</li>
 * <li>{@link #write(Object, SocketAddress)}</li>
 * <li>and the shortcut methods which calls the methods mentioned above
 * </ul>
 *
 *
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 *
 * @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
 *
 */

class AbstractServerChannel : public AbstractChannel, public ServerChannel {
public:
    virtual ChannelFuturePtr connect(const SocketAddress& remoteAddress);

    virtual ChannelFuturePtr disconnect();

    virtual int getInterestOps();

    virtual ChannelFuturePtr setInterestOps(int interestOps);

    virtual ChannelFuturePtr write(const ChannelMessage& message);

    virtual ChannelFuturePtr write(const ChannelMessage& message,
                                   const SocketAddress& remoteAddress);

    virtual bool isConnected() const;

protected:
    /**
     * Creates a new instance.
     *
     * @param factory
     *        the factory which created this channel
     * @param pipeline
     *        the pipeline which is going to be attached to this channel
     * @param sink
     *        the sink which will receive downstream events from the pipeline
     *        and send upstream events to the pipeline
     */
    AbstractServerChannel(
        const ChannelFactoryPtr& factory,
        const ChannelPipelinePtr& pipeline,
        const ChannelSinkPtr& sink);

    virtual ~AbstractServerChannel();

    virtual void setInterestOpsNow(int interestOps);

};

}
}

#endif //#if !defined(CETTY_CHANNEL_ABSTRACTSERVERCHANNEL_H)

// Local Variables:
// mode: c++
// End:
