#if !defined(CETTY_CHANNEL_CHANNELSINK_H)
#define CETTY_CHANNEL_CHANNELSINK_H

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
 * Distributed under under the Apache License, version 2.0 (the "License").
 */

#include <cetty/channel/ChannelFutureFwd.h>
#include <cetty/buffer/ChannelBufferFwd.h>

namespace cetty {
namespace channel {

class SocketAddress;

using namespace cetty::buffer;

/**
 * Receives and processes the terminal downstream {@link ChannelEvent}s.
 * <p>
 * A {@link ChannelSink} is an internal component which is supposed to be
 * implemented by a transport provider.  Most users will not see this type
 * in their code.
 *
 *
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 *
 * @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
 *
 * @apiviz.uses org.jboss.netty.channel.ChannelPipeline - - sends events upstream
 */

class ChannelSink {
public:
    virtual ~ChannelSink() {}

    virtual void bind(const SocketAddress& localAddress,
                      const ChannelFuturePtr& future) = 0;

    virtual void connect(const SocketAddress& remoteAddress,
                         const SocketAddress& localAddress,
                         const ChannelFuturePtr& future) = 0;

    virtual void disconnect(const ChannelFuturePtr& future) = 0;

    virtual void close(const ChannelFuturePtr& future) = 0;

    virtual void flush(const ChannelBufferPtr& buffer,
        const ChannelFuturePtr& future) = 0;
};

}
}

#endif //#if !defined(CETTY_CHANNEL_CHANNELSINK_H)

// Local Variables:
// mode: c++
// End:

