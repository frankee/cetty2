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
#include <cetty/channel/EventLoopPoolPtr.h>
#include <cetty/channel/socket/ServerSocketChannel.h>

#include <cetty/channel/socket/asio/AsioSocketChannel.h>
#include <cetty/channel/socket/asio/AsioServicePoolPtr.h>
#include <cetty/channel/socket/asio/AsioHandlerAllocator.h>
#include <cetty/channel/socket/asio/AsioServerSocketChannelConfig.h>

namespace cetty {
namespace channel {
class AbstractChannelSink;
}
}

namespace cetty {
namespace logging {
class InternalLogger;
}
}

namespace cetty {
namespace channel {
namespace socket {
namespace asio {

class AsioSocketChannel;

using namespace cetty::logging;
using namespace cetty::channel;
using namespace cetty::channel::socket;

// only response to bind port, open and close.
class AsioServerSocketChannel : public cetty::channel::socket::ServerSocketChannel {
public:
    AsioServerSocketChannel(const EventLoopPtr& eventLoop,
                            const ChannelFactoryPtr& factory,
                            const ChannelPipelinePtr& pipeline,
                            const ChannelPipelinePtr& childPipeline,
                            const EventLoopPoolPtr& childEventLoopPool);

    virtual ~AsioServerSocketChannel();

    virtual ChannelConfig& getConfig();
    virtual const ChannelConfig& getConfig() const;

    virtual const SocketAddress& getLocalAddress() const;
    virtual const SocketAddress& getRemoteAddress() const;

    virtual ChannelSink& getSink();

    virtual bool isActive() const;

protected:
    virtual bool setClosed();

    virtual void doBind(const SocketAddress& localAddress);
    virtual void doDisconnect();
    virtual void doClose();

private:
    void accept();
    void handleAccept(const boost::system::error_code& error,
                      AsioSocketChannelPtr channel);

private:
    typedef std::map<int, ChannelPtr> ChildChannels;

    static InternalLogger* logger;

private:
    AsioServicePtr ioService;

    AsioHandlerAllocator<int> acceptAllocator;
    boost::asio::ip::tcp::acceptor acceptor;

    AbstractChannelSink* sink;

    AsioServicePoolPtr childServicePool;
    ChannelPipelinePtr childPipeline;
    ChildChannels childChannels;

    AsioServerSocketChannelConfig config;
    mutable SocketAddress localAddress;
};

}
}
}
}

#endif //#if !defined(CETTY_CHANNEL_SOCKET_ASIO_ASIOSERVERSOCKETCHANNEL_H)
