#if !defined(CETTY_CHANNEL_SOCKET_ASIO_ASIOSERVERSOCKETCHANNEL_H)
#define CETTY_CHANNEL_SOCKET_ASIO_ASIOSERVERSOCKETCHANNEL_H

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

#include <boost/asio.hpp>

#include <cetty/channel/SocketAddress.h>
#include <cetty/channel/socket/ServerSocketChannel.h>
#include <cetty/channel/socket/asio/AsioServicePoolFwd.h>
#include <cetty/channel/socket/asio/DefaultAsioServerSocketChannelConfig.h>


namespace cetty {
namespace logging {
class InternalLogger;
}
}

namespace cetty {
namespace channel {
namespace socket {
namespace asio {

using namespace cetty::logging;
using namespace cetty::channel;
using namespace cetty::channel::socket;

// only response to bind port, open and close.
class AsioServerSocketChannel : public cetty::channel::socket::ServerSocketChannel {
public:
    AsioServerSocketChannel(
        boost::asio::ip::tcp::acceptor& acceptor,
        const AsioServicePtr& ioService,
        const ChannelFactoryPtr& factory,
        const ChannelPipelinePtr& pipeline,
        const ChannelSinkPtr& sink);

    virtual ~AsioServerSocketChannel();

    boost::asio::ip::tcp::acceptor& getAcceptor() {
        return acceptor;
    }

    virtual ChannelConfig& getConfig();
    virtual const ChannelConfig& getConfig() const;

    virtual const SocketAddress& getLocalAddress() const;
    virtual const SocketAddress& getRemoteAddress() const;

    virtual bool isBound() const;

    virtual bool setClosed();

private:
    static InternalLogger* logger;

private:
    AsioServicePtr ioService;
    boost::asio::ip::tcp::acceptor& acceptor;

    DefaultAsioServerSocketChannelConfig config;
    mutable SocketAddress localAddress;
};

}
}
}
}

#endif //#if !defined(CETTY_CHANNEL_SOCKET_ASIO_ASIOSERVERSOCKETCHANNEL_H)
