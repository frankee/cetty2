#if !defined(CETTY_CHANNEL_ABSTRACTCHANNELSINK_H)
#define CETTY_CHANNEL_ABSTRACTCHANNELSINK_H

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

#include <cetty/channel/ChannelSink.h>

namespace cetty {
namespace channel {

class AbstractChannel;

/**
 * A skeletal {@link ChannelSink} implementation.
 *
 *
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 *
 * @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
 */

class AbstractChannelSink : public ChannelSink {
public:
    AbstractChannelSink(AbstractChannel& channel);
    virtual ~AbstractChannelSink();

    virtual void bind(const SocketAddress& localAddress, const ChannelFuturePtr& future);

    virtual void disconnect(const ChannelFuturePtr& future);

    virtual void close(const ChannelFuturePtr& future);

    virtual void connect(const SocketAddress& remoteAddress,
                         const SocketAddress& localAddress,
                         const ChannelFuturePtr& future);

    virtual void flush(const ChannelBufferPtr& buffer,
                       const ChannelFuturePtr& future);

protected:
    bool ensureOpen(const ChannelFuturePtr& future);
    void closeIfClosed();

private:
    AbstractChannel& channel;
};

}
}

#endif //#if !defined(CETTY_CHANNEL_ABSTRACTCHANNELSINK_H)

// Local Variables:
// mode: c++
// End:

