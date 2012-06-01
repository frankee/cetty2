#if !defined(CETTY_CHANNEL_SOCKET_ASIO_ASIOSERVERSOCKETPIPELINESINK_H)
#define CETTY_CHANNEL_SOCKET_ASIO_ASIOSERVERSOCKETPIPELINESINK_H

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
#include <boost/circular_buffer.hpp>

#include <cetty/channel/AbstractChannelSink.h>
#include <cetty/channel/ChannelFuture.h>

#include <cetty/buffer/ChannelBuffer.h>
#include <cetty/util/Integer.h>

#include <cetty/channel/socket/asio/AsioServicePool.h>
#include <cetty/channel/socket/asio/AsioHandlerAllocator.h>

namespace cetty {
namespace channel {
class MessageEvent;
class SocketAddress;
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

using namespace cetty::channel;
using namespace cetty::buffer;
using namespace cetty::logging;

class AsioServicePool;
class AsioSocketChannel;
class AsioAcceptedSocketChannel;
class AsioServerSocketChannel;

class AsioServerSocketPipelineSink : public cetty::channel::AbstractChannelSink {
private:
    typedef std::map<int, ChannelPtr> ChildChannels;

public:
    AsioServerSocketPipelineSink(AsioServicePoolPtr& ioServicePool);
    virtual ~AsioServerSocketPipelineSink();

    virtual void writeRequested(const ChannelPipeline& pipeline,
                                const MessageEvent& e);

    virtual void stateChangeRequested(const ChannelPipeline& pipeline,
                                      const ChannelStateEvent& e);

    boost::asio::ip::tcp::acceptor& getAcceptor() { return acceptor; }
    const AsioServicePtr& getAcceptorService() { return acceptorService; }

private:
    void bind(
        const ChannelPtr& channel,
        const ChannelFuturePtr& future,
        const SocketAddress& localAddress);

    void accept(const ChannelPtr& channel);

    void handleAccept(const boost::system::error_code& error,
        ChannelPtr channel,
        const ChannelPtr& serverChannel);

    void handleServerChannelStateChange(const ChannelPtr& channel,
                                        const ChannelStateEvent& evt);

    void handleAcceptChannelStateChange(const ChannelPtr& channel,
                                        const ChannelStateEvent& evt);

    void closeServerChannel(const ChannelPtr& channel,
                            const ChannelFuturePtr& future);

    void closeAcceptChannel(const ChannelPtr& channel,
                            const ChannelFuturePtr& future);

private:
    static InternalLogger* logger;

private:
    AsioServicePoolPtr ioServicePool;
    AsioServicePtr acceptorService;

    AsioHandlerAllocator<int> acceptAllocator;
    boost::asio::ip::tcp::acceptor acceptor;

    ChildChannels childChannels;
    boost::circular_buffer<ChannelPtr> closedChannels;
};

}
}
}
}


#endif //#if !defined(CETTY_CHANNEL_SOCKET_ASIO_ASIOSERVERSOCKETPIPELINESINK_H)
