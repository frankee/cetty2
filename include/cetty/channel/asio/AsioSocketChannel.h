#if !defined(CETTY_CHANNEL_ASIO_ASIOSOCKETCHANNEL_H)
#define CETTY_CHANNEL_ASIO_ASIOSOCKETCHANNEL_H

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

#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/detail/atomic_count.hpp>

#include <cetty/channel/Channel.h>
#include <cetty/channel/SocketAddress.h>
#include <cetty/channel/ChannelFuture.h>
#include <cetty/channel/ChannelPipeline.h>
#include <cetty/channel/ChannelMessageContainer.h>
#include <cetty/channel/ChannelMessageHandlerContext.h>

#include <cetty/channel/asio/AsioServicePoolPtr.h>
#include <cetty/channel/asio/AsioHandlerAllocator.h>
#include <cetty/channel/asio/AsioSocketChannelConfig.h>

#include <cetty/buffer/ChannelBuffer.h>

namespace cetty {
namespace channel {
namespace asio {

using namespace cetty::channel;
using namespace cetty::buffer;

class AsioWriteOperationQueue;
class AsioServerSocketChannel;

class AsioSocketChannel : public cetty::channel::Channel {
public:
    typedef ChannelMessageHandlerContext<
    AsioSocketChannel*,
    VoidMessage,
    VoidMessage,
    ChannelBufferPtr,
    VoidMessage,
    VoidMessageContainer,
    VoidMessageContainer,
    ChannelBufferContainer,
    VoidMessageContainer> Context;

public:
    AsioSocketChannel(const EventLoopPtr& eventLoop);

    AsioSocketChannel(int id,
                      const EventLoopPtr& eventLoop);

    AsioSocketChannel(const ChannelPtr& parent,
                      const EventLoopPtr& eventLoop);

    AsioSocketChannel(int id,
                      const ChannelPtr& parent,
                      const EventLoopPtr& eventLoop);

    virtual ~AsioSocketChannel();

    boost::asio::ip::tcp::socket& tcpSocket();

    const AsioServicePtr& ioService();

    virtual bool isOpen() const;
    virtual bool isActive() const;

    void registerTo(Context& context);

private:
    virtual void doBind(const SocketAddress& localAddress);
    virtual void doDisconnect();
    virtual void doClose();

    virtual void doInitialize();

    void doConnect(ChannelHandlerContext& ctx,
                   const SocketAddress& remoteAddress,
                   const SocketAddress& localAddress,
                   const ChannelFuturePtr& future);

    void handleConnectTimeout();
    void handleConnectFailed();
    void handleConnectSuccess();

    void doConnect(const SocketAddress& remoteAddress,
                   const SocketAddress& localAddress,
                   const ChannelFuturePtr& connectFuture);

    void doFlush(ChannelHandlerContext& ctx, const ChannelFuturePtr& future);

    void beginRead();

    void handleRead(const boost::system::error_code& error,
                    size_t bytes_transferred);

    void handleWrite(const boost::system::error_code& error,
                     size_t bytes_transferred);

    void handleConnect(const boost::system::error_code& error,
                       boost::asio::ip::tcp::resolver::iterator endpointIterator,
                       const ChannelFuturePtr& cf);

    void handleResolve(const boost::system::error_code& error,
                       boost::asio::ip::tcp::resolver::iterator itr,
                       const ChannelFuturePtr& cf);

    void cleanUpWriteBuffer();

    void connectFailed(const ChannelFuturePtr& connectFuture,
                       const ChannelException& e);

private:
    friend class AsioWriteOperationQueue;
    friend class AsioServerSocketChannel;
    friend class AsioSocketChannelSinkHandler;

private:
    bool opened_;
    bool isReading_;
    bool isWriting_;
    bool initialized_;
    int  highWaterMarkCounter_;

    AsioServicePtr  ioService_;
    boost::asio::ip::tcp::socket tcpSocket_;

    ChannelBufferPtr readBuffer_;
    ChannelBufferContainer* bridgeContainer_;
    boost::scoped_ptr<AsioWriteOperationQueue> writeQueue_;

    AsioSocketChannelConfig socketConfig_;

    AsioHandlerAllocator<int> readAllocator_;
    AsioHandlerAllocator<int> writeAllocator_;
};

inline
boost::asio::ip::tcp::socket& AsioSocketChannel::tcpSocket() {
    return tcpSocket_;
}

inline
const AsioServicePtr& AsioSocketChannel::ioService() {
    return ioService_;
}

}
}
}

#endif //#if !defined(CETTY_CHANNEL_ASIO_ASIOSOCKETCHANNEL_H)

// Local Variables:
// mode: c++
// End:
