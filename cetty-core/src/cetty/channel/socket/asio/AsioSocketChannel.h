#if !defined(CETTY_CHANNEL_SOCKET_ASIO_ASIOSOCKETCHANNEL_H)
#define CETTY_CHANNEL_SOCKET_ASIO_ASIOSOCKETCHANNEL_H

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

#include <boost/asio/ip/tcp.hpp>
#include <boost/thread.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/detail/atomic_count.hpp>

#include <cetty/channel/SocketAddress.h>
#include <cetty/channel/ChannelFactory.h>
#include <cetty/channel/ChannelFuture.h>
#include <cetty/channel/ChannelPipeline.h>
#include <cetty/channel/ChannelPipelineFactory.h>
#include <cetty/channel/socket/SocketChannel.h>
#include <cetty/channel/socket/asio/AsioServicePoolPtr.h>
#include <cetty/channel/socket/asio/AsioHandlerAllocator.h>
#include <cetty/channel/socket/asio/AsioSocketChannelConfig.h>
#include <cetty/buffer/ChannelBuffer.h>
#include <cetty/buffer/ChannelBufferFactory.h>

namespace cetty {
namespace logging {
class InternalLogger;
}
}

namespace cetty {
namespace channel  {
namespace socket {
namespace asio {

using namespace cetty::channel;
using namespace cetty::buffer;
using namespace cetty::logging;

class AsioSocketChannelSink;
class AsioWriteOperationQueue;
class AsioServerSocketChannel;

class AsioSocketChannel;
typedef boost::intrusive_ptr<AsioSocketChannel> AsioSocketChannelPtr;

class AsioSocketChannel : public cetty::channel::socket::SocketChannel {
public:
    AsioSocketChannel(const ChannelPtr& parent,
                      const AsioServicePtr& ioService,
                      const ChannelFactoryPtr& factory,
                      const ChannelPipelinePtr& pipeline);

    AsioSocketChannel(const AsioServicePtr& ioService,
                      const ChannelFactoryPtr& factory,
                      const ChannelPipelinePtr& pipeline);

    


    virtual ~AsioSocketChannel();

    virtual ChannelConfig& getConfig();
    virtual const ChannelConfig& getConfig() const;

    virtual ChannelSink& getSink();

    boost::asio::ip::tcp::socket& getSocket() {
        return this->tcpSocket;
    }

    const AsioServicePtr& getService() {
        return ioService;
    }

    void setMainThreadMode(bool mainThreadMode) {
        this->mainThreadMode = mainThreadMode;
    }
    bool isMainThreadMode() const { return mainThreadMode; }

    virtual const SocketAddress& getLocalAddress() const;
    virtual const SocketAddress& getRemoteAddress() const;

    virtual bool isActive() const;

protected:
    virtual bool setClosed();

    virtual void doBind(const SocketAddress& localAddress);
    virtual void doDisconnect();
    virtual void doClose();

    void doConnect(const SocketAddress& remoteAddress,
                   const SocketAddress& localAddress,
                   const ChannelFuturePtr& connectFuture);

    void doFlush(const ChannelBufferPtr& buffer, const ChannelFuturePtr& future);

    void beginRead();

    void handleRead(const boost::system::error_code& error, size_t bytes_transferred);
    void handleWrite(const boost::system::error_code& error, size_t bytes_transferred);

    void handleConnect(const boost::system::error_code& error,
                       boost::asio::ip::tcp::resolver::iterator endpoint_iterator,
                       const ChannelFuturePtr& cf);

    void cleanUpWriteBuffer();

private:
    void init(const ChannelPipelinePtr& pipeline);

private:
    friend class AsioSocketChannelSink;
    friend class AsioWriteOperationQueue;
    friend class AsioServerSocketChannel;

protected:
    static InternalLogger* logger;

protected:
    bool isWriting;
    int  highWaterMarkCounter;

    AsioServicePtr  ioService;
    boost::asio::ip::tcp::socket tcpSocket;

    AsioSocketChannelSink* sink;

    boost::scoped_ptr<AsioWriteOperationQueue> writeQueue;

    AsioSocketChannelConfig config;

    AsioHandlerAllocator<int> readAllocator;
    AsioHandlerAllocator<int> writeAllocator;

    mutable SocketAddress localAddress;
    mutable SocketAddress remoteAddress;

private:
    bool mainThreadMode;
};

}
}
}
}

#endif //#if !defined(CETTY_CHANNEL_SOCKET_ASIO_ASIOSOCKETCHANNEL_H)

// Local Variables:
// mode: c++
// End:
