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
#include <cetty/channel/socket/asio/AsioServicePoolFwd.h>
#include <cetty/channel/socket/asio/AsioHandlerAllocator.h>
#include <cetty/channel/socket/asio/DefaultAsioSocketChannelConfig.h>
#include <cetty/buffer/ChannelBuffer.h>
#include <cetty/buffer/ChannelBufferFactory.h>

namespace cetty {
    namespace logging {
        class InternalLogger;
    }
}

namespace cetty {
    namespace channel {
        class MessageEvent;
    }
}

namespace cetty {
namespace channel  {
namespace socket {
namespace asio {

using namespace cetty::channel;
using namespace cetty::buffer;
using namespace cetty::logging;

class AsioWriteOperationQueue;

class AsioSocketChannel : public cetty::channel::socket::SocketChannel {
public:
    AsioSocketChannel(const ChannelPtr& parent,
                      const ChannelFactoryPtr& factory,
                      const ChannelPipelinePtr& pipeline,
                      const AsioServicePtr& ioService,
                      const boost::thread::id& id);

    virtual ~AsioSocketChannel();

    virtual ChannelConfig& getConfig() { return this->config; }
    virtual const ChannelConfig& getConfig() const { return this->config; }

    boost::asio::ip::tcp::socket& getSocket() {
        return this->tcpSocket;
    }

    const AsioServicePtr& getService() {
        return ioService;
    }

    virtual const SocketAddress& getLocalAddress() const;
    virtual const SocketAddress& getRemoteAddress() const;

    virtual bool isOpen() const;
    virtual bool isBound() const;
    virtual bool isConnected() const;
    
    virtual int getInterestOps() const;
    int getRawInterestOps() const {
        return AbstractChannel::getInterestOps();
    }
    void setRawInterestOpsNow(int interestOps) {
        AbstractChannel::setInterestOpsNow(interestOps);
    }

    void setClosedState();
    void setBoundState();
    void setConnectedState();

    virtual ChannelFuturePtr write(const ChannelMessage& message);
    virtual ChannelFuturePtr write(const ChannelMessage& message,
                                   const SocketAddress& remoteAddress);

    virtual ChannelFuturePtr unbind();
    virtual ChannelFuturePtr close();
    virtual ChannelFuturePtr disconnect();
    virtual ChannelFuturePtr setInterestOps(int interestOps);

    virtual bool setClosed();

    void internalWrite(const MessageEvent& evt);
    void doClose(const ChannelFuturePtr& future);
    void internalSetInterestOps(const ChannelFuturePtr& future, int interestOps);
    void cleanUpWriteBuffer();

    void doConnect(const SocketAddress& remoteAddress,
        const SocketAddress& localAddress,
        const ChannelFuturePtr& connectFuture);

    void doFlush(const ChannelBufferPtr& buffer, const ChannelFuturePtr& future);

    void handleRead(const boost::system::error_code& error, size_t bytes_transferred);
    void handleWrite(const boost::system::error_code& error, size_t bytes_transferred);

    void handleConnect(const boost::system::error_code& error,
                       boost::asio::ip::tcp::resolver::iterator endpoint_iterator,
                       const ChannelFuturePtr& cf);

private:
    void handleAtHighWaterMark();
    void handleAtLowWaterMark();

private:
    friend class AsioWriteOperationQueue;

protected:
    static InternalLogger* logger;

protected:
    bool isWriting;
    int  highWaterMarkCounter;

    boost::thread::id threadId;

    AsioServicePtr  ioService;
    boost::asio::ip::tcp::socket tcpSocket;

    ChannelBufferPtr         readBuffer;
    boost::scoped_ptr<AsioWriteOperationQueue> writeQueue;

    DefaultAsioSocketChannelConfig config;

    AsioHandlerAllocator<int> readAllocator;
    AsioHandlerAllocator<int> writeAllocator;
    AsioHandlerAllocator<boost::detail::atomic_count> ipcWriteAllocator;
    AsioHandlerAllocator<boost::detail::atomic_count> ipcStateChangeAllocator;

    mutable SocketAddress localAddress;
    mutable SocketAddress remoteAddress;

private:
    static const int CHANNEL_ST_OPEN      =  0;
    static const int CHANNEL_ST_BOUND     =  1;
    static const int CHANNEL_ST_CONNECTED =  2;
    static const int CHANNEL_ST_CLOSED    = -1;

    int state;
};

}
}
}
}

#endif //#if !defined(CETTY_CHANNEL_SOCKET_ASIO_ASIOSOCKETCHANNEL_H)

// Local Variables:
// mode: c++
// End:

