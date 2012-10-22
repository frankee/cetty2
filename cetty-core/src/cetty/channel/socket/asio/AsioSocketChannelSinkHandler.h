#if !defined(CETTY_CHANNEL_SOCKET_ASIO_ASIOSOCKETCHANNELSINKHANDLER_H)
#define CETTY_CHANNEL_SOCKET_ASIO_ASIOSOCKETCHANNELSINKHANDLER_H

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

#include <boost/intrusive_ptr.hpp>
#include <cetty/channel/ChannelSinkHandler.h>

namespace cetty {
namespace channel {
namespace socket {
namespace asio {

class AsioSocketChannel;

class AsioSocketChannelSinkHandler : public cetty::channel::ChannelSinkHandler {
public:
    typedef boost::intrusive_ptr<AsioSocketChannel> AsioSocketChannelPtr;
    
public:
    AsioSocketChannelSinkHandler(const AsioSocketChannelPtr& channel);
    virtual ~AsioSocketChannelSinkHandler() {}

public:
    virtual void connect(ChannelHandlerContext& ctx,
                         const SocketAddress& remoteAddress,
                         const SocketAddress& localAddress,
                         const ChannelFuturePtr& future);

    virtual void flush(ChannelHandlerContext& ctx,
                       const ChannelFuturePtr& future);

    // TODO: seperated with flush.
    //virtual void setOutboundChannelBuffer(const ChannelBufferPtr& buffer);

private:
    AsioSocketChannelPtr channel;
};

}
}
}
}

#endif //#if !defined(CETTY_CHANNEL_SOCKET_ASIO_ASIOSOCKETCHANNELSINKHANDLER_H)

// Local Variables:
// mode: c++
// End:
