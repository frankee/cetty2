/*
 * Copyright (c) 2010-2011 frankee zhou (frankee.zhou at gmail dot com)
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

#include <cetty/channel/socket/asio/AsioSocketChannel.h>

#include <boost/any.hpp>
#include <boost/bind.hpp>
#include <boost/assert.hpp>
#include <boost/intrusive_ptr.hpp>

#include <cetty/channel/SocketAddress.h>
#include <cetty/channel/ChannelFuture.h>
#include <cetty/channel/ChannelFactory.h>
#include <cetty/channel/ChannelPipeline.h>
#include <cetty/channel/ChannelException.h>
#include <cetty/channel/AdaptiveReceiveBuffer.h>

#include <cetty/channel/socket/asio/AsioService.h>
#include <cetty/channel/socket/asio/AsioServicePool.h>
#include <cetty/channel/socket/asio/AsioSocketChannelSink.h>
#include <cetty/channel/socket/asio/AsioSocketAddressImpl.h>
#include <cetty/channel/socket/asio/AsioWriteOperationQueue.h>
#include <cetty/channel/socket/asio/AsioClientSocketChannelFactory.h>

#include <cetty/buffer/ChannelBuffer.h>
#include <cetty/buffer/ChannelBufferFactory.h>
#include <cetty/buffer/CompositeChannelBuffer.h>

#include <cetty/logging/LoggerHelper.h>
#include <cetty/util/Integer.h>

namespace cetty {
namespace channel {
namespace socket {
namespace asio {

using namespace cetty::channel;
using namespace cetty::buffer;
using namespace cetty::util;

AsioSocketChannel::AsioSocketChannel(const ChannelPtr& parent,
                                     const EventLoopPtr& eventLoop,
                                     const ChannelFactoryPtr& factory,
                                     const ChannelPipelinePtr& pipeline)
    : SocketChannel(eventLoop, parent, factory, pipeline),
      ioService(boost::dynamic_pointer_cast<AsioService>(eventLoop)),
      tcpSocket(boost::dynamic_pointer_cast<AsioService>(eventLoop)->service()),
      sink(),
      isWriting(false),
      highWaterMarkCounter(0),
      config(tcpSocket) {
      init(pipeline);
}

AsioSocketChannel::AsioSocketChannel(const EventLoopPtr& eventLoop,
                                     const ChannelFactoryPtr& factory,
                                     const ChannelPipelinePtr& pipeline)
    : SocketChannel(eventLoop, ChannelPtr(), factory, pipeline),
      ioService(boost::dynamic_pointer_cast<AsioService>(eventLoop)),
      tcpSocket(boost::dynamic_pointer_cast<AsioService>(eventLoop)->service()),
      sink(),
      isWriting(false),
      highWaterMarkCounter(0),
      config(tcpSocket) {
    init(pipeline);
}

AsioSocketChannel::~AsioSocketChannel() {
    if (sink) {
        delete sink;
    }
}

ChannelConfig& AsioSocketChannel::getConfig() {
    return this->config;
}

const ChannelConfig& AsioSocketChannel::getConfig() const {
    return this->config;
}

ChannelSink& AsioSocketChannel::getSink() {
    return *sink;
}

const SocketAddress& AsioSocketChannel::getLocalAddress() const {
    if (localAddress.validated()) {
        return localAddress;
    }

    boost::system::error_code ec;
    boost::asio::ip::tcp::endpoint endpoint = tcpSocket.local_endpoint(ec);

    if (!ec) {
        localAddress = SocketAddress(SocketAddressImplPtr(
            new AsioTcpSocketAddressImpl(ioService->service(), endpoint)));
    }
    else {
        LOG_ERROR << "failed to get the local address of the channel from asio.";
    }

    return localAddress;
}

const SocketAddress& AsioSocketChannel::getRemoteAddress() const {
    if (remoteAddress.validated()) {
        return remoteAddress;
    }

    boost::system::error_code ec;
    boost::asio::ip::tcp::endpoint endpoint = tcpSocket.remote_endpoint(ec);

    if (!ec) {
        remoteAddress = SocketAddress(SocketAddressImplPtr(
            new AsioTcpSocketAddressImpl(ioService->service(), endpoint)));
    }
    else {
        LOG_ERROR << "failed to get the remote address of the channel from asio.";
    }

    return remoteAddress;
}

bool AsioSocketChannel::isActive() const {
    return tcpSocket.is_open();
}

bool AsioSocketChannel::setClosed() {
    cleanUpWriteBuffer();

    LOG_INFO << "closed the socket channel, finally calling FutureListener"
             " which to the ChannelCloseFuture.";

    return AbstractChannel::setClosed();
}

void AsioSocketChannel::handleRead(const boost::system::error_code& error,
                                   size_t bytes_transferred) {
    if (!error) {
        AdaptiveReceiveBuffer& buffer = pipeline->getReceiveBuffer();
        buffer.setReceiveBufferSize(bytes_transferred);
        pipeline->setInboundChannelBuffer(buffer.channelBuffer());

        pipeline->fireMessageUpdated();

        beginRead();
    }
    else {
        close();
    }
}

void AsioSocketChannel::handleWrite(const boost::system::error_code& error,
                                    size_t bytes_transferred) {
    if (!error) {
        writeQueue->peek().setSuccess();
        writeQueue->popup();

        pipeline->fireWriteCompleted();

        if (writeQueue->empty()) {
            isWriting = false;
        }
    }
    else {
        if (!writeQueue->empty()) {
            writeQueue->peek().setFailure(
                RuntimeException(std::string("write buffer failed, code=") +
                Integer::toString(error.value())));
            writeQueue->popup();
        }
        
        close();
    }
}

void AsioSocketChannel::handleConnect(const boost::system::error_code& error,
                                      boost::asio::ip::tcp::resolver::iterator endpoint_iterator,
                                      const ChannelFuturePtr& cf) {
    if (!error) {
        LOG_INFO << "channel connected to the remote server %s, firing connected event."
            << remoteAddress.toString();
        pipeline->fireChannelActive();
        cf->setSuccess();

        beginRead();
    }
    else if (endpoint_iterator != boost::asio::ip::tcp::resolver::iterator()) {
        LOG_WARN << "resolved more than one address, try to connect the next address.";
        boost::system::error_code error;
        tcpSocket.close(error);

        if (error) {
            LOG_ERROR << "failed to close tcp socket before connect, "
                << error.value()
                << ":"
                << error.message()
                << ", but skip it.";
        }

        boost::asio::ip::tcp::endpoint endpoint = *endpoint_iterator;
        tcpSocket.async_connect(endpoint,
                                boost::bind(&AsioSocketChannel::handleConnect,
                                            this,
                                            boost::asio::placeholders::error,
                                            ++endpoint_iterator,
                                            cf));
    }
    else {
        cf->setFailure(ChannelException("failed to connect to remote server", error.value()));
        doClose();
    }
}

void AsioSocketChannel::cleanUpWriteBuffer() {
    ChannelException cause;
    bool fireExceptionCaught = false;

    // Clean up the stale messages in the write buffer.
    if (!writeQueue->empty()) {
        if (isOpen()) {
            LOG_WARN << "cleanUpWriteBuffer: The channel has not close yet.";
            cause = ChannelException("Channel has not close yet.");
        }
        else {
            cause = ChannelException("Channel has closed.");
        }

        // last one in the writeBuffer should not been cleaned.
        // it is already sent asynchronously, will take care of itself.
        while (writeQueue->size() > 1) {
            LOG_WARN << "cleanUpWriteBuffer: poll and clean the write queue.";
            writeQueue->peek().setFailure(cause);
            writeQueue->popup();

            fireExceptionCaught = true;
        }
    }

    if (fireExceptionCaught) {
        LOG_WARN << "cleanUpWriteBuffer: firing the the exception event.";
        pipeline->fireExceptionCaught(cause);
    }
}

void AsioSocketChannel::doConnect(const SocketAddress& remoteAddress,
                                  const SocketAddress& localAddress,
                                  const ChannelFuturePtr& connectFuture) {
    const std::string& hostname = remoteAddress.hostName();
    std::string port = Integer::toString(remoteAddress.port());

    boost::asio::ip::tcp::resolver resolver(getService()->service());
    boost::asio::ip::tcp::resolver::query query(hostname, port);

    boost::system::error_code error;
    boost::asio::ip::tcp::resolver::iterator iterator = resolver.resolve(query, error);

    if (error) {
        LOG_ERROR << "boost asio can NOT resolve "
            << hostname << ":" << port
            << " , code=" << error.value() << ", msg=" << error.message()
            << ", then firing this exception.";

        ChannelException exception(error.message(), error.value());
        connectFuture->setFailure(exception);
        //Channels::fireExceptionCaught(channel, exception);

        close(getCloseFuture());
        return;
    }

    boost::asio::ip::tcp::endpoint endpoint = *iterator;
    getSocket().async_connect(
        endpoint,
        boost::bind(&AsioSocketChannel::handleConnect,
                    this,
                    boost::asio::placeholders::error,
                    ++iterator,
                    connectFuture));

    const EventLoopPoolPtr& pool = eventLoop->getEventLoopPool();
    if (pool && pool->isMainThread()) {
            LOG_INFO << "the asio service pool starting to run in main thread.";
            if (pool->start()) {
                LOG_INFO << "the asio service pool started to running.";
            }
            else {
                //TODO: may duplicated with the callback of AsioSocketChannel::handleConnect
                LOG_ERROR << "start the boost asio service error,"
                    << " and firing an exception, then terminate self.";
                ChannelException e("the boost asio service can not be started.");
                connectFuture->setFailure(e);
                pipeline->fireExceptionCaught(e);
                doClose();

                //TODO: should terminate the program
                std::terminate();
            }
    }
}

void AsioSocketChannel::beginRead() {
    Array arry;
    pipeline->getReceiveBuffer().writableBytes(&arry);
    LOG_INFO << "AsioSocketChannel begin to async read, with the the buffer size" << arry.length();
    tcpSocket.async_read_some(
        boost::asio::buffer(arry.data(), arry.length()),
        boost::bind(&AsioSocketChannel::handleRead,
                    this,
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
}

void AsioSocketChannel::doBind(const SocketAddress& localAddress) {

}

void AsioSocketChannel::doDisconnect() {
    doClose();
}

void AsioSocketChannel::doClose() {
    if (!isOpen() || !tcpSocket.is_open() || getCloseFuture()->isDone()) {
        LOG_INFO << "close the socket channel, but the channel already closed.";
        return;
    }

    boost::system::error_code error;

    if (isActive()) {
        // if shutdown failed, try to close tcp socket directly.
        tcpSocket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, error);

        if (error) {
            LOG_ERROR << "failed to shutdown the tcp socket"
                << error.value() << ":" << error.message();
        }
    }

    tcpSocket.close(error);

    if (error) {
        LOG_ERROR << "failed to close the tcp socket."
            << error.value() << ":" << error.message();

        IOException e("failed to close the tcp socket.", error.value());
        //future->setFailure(e);

        //setClosed();
        //Channels::fireExceptionCaught(this, e);
    }
    else {
        //future->setSuccess();
        //setClosed();
    }
}

void AsioSocketChannel::doFlush(const ChannelBufferPtr& buffer,
                                const ChannelFuturePtr& future) {

    if (!isActive()) {
        LOG_WARN << "sending the msg, but the socket is disconnected, clean up write buffer.";
        cleanUpWriteBuffer();

        if (future) {
            future->setFailure(ChannelException("Channel has been closed."));
        }

        return;
    }

    isWriting = true;
    AsioWriteOperation& operation = writeQueue->offer(buffer, future);

    if (operation.writeBufferSize() == 0) {
        LOG_WARN << "write an empty message, do not write to the socket,\
                             just post a handleWrite operation.";
        ioService->service().post(boost::bind(
                                      &AsioSocketChannel::handleWrite,
                                      this,
                                      boost::system::error_code(),
                                      0));
        return;
    }

    if (operation.isMultiBuffers()) {
        boost::asio::async_write(tcpSocket,
                                 operation.getAsioBufferArray(),
                                 makeCustomAllocHandler(writeAllocator,
                                         boost::bind(&AsioSocketChannel::handleWrite,
                                                 this,
                                                 boost::asio::placeholders::error,
                                                 boost::asio::placeholders::bytes_transferred)));
        LOG_WARN << "write a gathering buffer to the socket, may slow down the system.";
    }
    else {
        boost::asio::async_write(tcpSocket,
                                 boost::asio::const_buffers_1(operation.getAsioBuffer()),
                                 makeCustomAllocHandler(writeAllocator,
                                         boost::bind(&AsioSocketChannel::handleWrite,
                                                 this,
                                                 boost::asio::placeholders::error,
                                                 boost::asio::placeholders::bytes_transferred)));
        LOG_INFO << "write a buffer to the socket";
    }
}

void AsioSocketChannel::init(const ChannelPipelinePtr& pipeline) {
    writeQueue.reset(new AsioWriteOperationQueue(*this));
    this->sink = new AsioSocketChannelSink(*this);

    AbstractChannel::setPipeline(pipeline);
    LOG_INFO << "AsioSocketChannel firing the Channel Create Event.";
    pipeline->fireChannelCreated();
}

}
}
}
}
