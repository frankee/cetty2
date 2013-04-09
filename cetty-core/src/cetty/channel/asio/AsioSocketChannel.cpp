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

#include <cetty/channel/asio/AsioSocketChannel.h>

#include <boost/any.hpp>
#include <boost/bind.hpp>
#include <boost/assert.hpp>
#include <boost/intrusive_ptr.hpp>

#include <cetty/channel/InetAddress.h>
#include <cetty/channel/ChannelFuture.h>
#include <cetty/channel/ChannelPipeline.h>
#include <cetty/channel/ChannelException.h>

#include <cetty/channel/asio/AsioService.h>
#include <cetty/channel/asio/AsioServicePool.h>
#include <cetty/channel/asio/AsioWriteOperationQueue.h>

#include <cetty/buffer/ChannelBuffer.h>
#include <cetty/buffer/CompositeChannelBuffer.h>

#include <cetty/util/StringUtil.h>
#include <cetty/logging/LoggerHelper.h>

namespace cetty {
namespace channel {
namespace asio {

using namespace boost::asio;
using namespace boost::asio::ip;

using namespace cetty::util;
using namespace cetty::buffer;
using namespace cetty::channel;

static const int DEFAULT_READER_BUFFER_SIZE = 1024 * 16;
static const int MIN_READER_BUFFER_SIZE = 128;
static const int READER_BUFFER_SIZE_STEP = 4096;

AsioSocketChannel::AsioSocketChannel(const ChannelPtr& parent,
                                     const EventLoopPtr& eventLoop)
    : Channel(parent, eventLoop),
      isReading_(false),
      isWriting_(false),
      isConnecting_(false),
      initialized_(false),
      highWaterMarkCounter_(0),
      ioService_(boost::dynamic_pointer_cast<AsioService>(eventLoop)),
      tcpSocket_(boost::dynamic_pointer_cast<AsioService>(eventLoop)->service()),
      resolver_(boost::dynamic_pointer_cast<AsioService>(eventLoop)->service()),
      readBuffer_(Unpooled::buffer(DEFAULT_READER_BUFFER_SIZE)),
      writeBufferContainer_(),
      socketConfig_(tcpSocket_) {
    writeQueue_.reset(new AsioWriteOperationQueue(*this));
}

AsioSocketChannel::AsioSocketChannel(const EventLoopPtr& eventLoop)
    : Channel(ChannelPtr(), eventLoop),
      isReading_(false),
      isWriting_(false),
      isConnecting_(false),
      initialized_(false),
      highWaterMarkCounter_(0),
      ioService_(boost::dynamic_pointer_cast<AsioService>(eventLoop)),
      tcpSocket_(boost::dynamic_pointer_cast<AsioService>(eventLoop)->service()),
      resolver_(boost::dynamic_pointer_cast<AsioService>(eventLoop)->service()),
      readBuffer_(Unpooled::buffer(DEFAULT_READER_BUFFER_SIZE)),
      writeBufferContainer_(),
      socketConfig_(tcpSocket_) {
    writeQueue_.reset(new AsioWriteOperationQueue(*this));
}

AsioSocketChannel::AsioSocketChannel(int id, const EventLoopPtr& eventLoop)
    : Channel(id, ChannelPtr(), eventLoop),
      isReading_(false),
      isWriting_(false),
      isConnecting_(false),
      initialized_(false),
      highWaterMarkCounter_(0),
      ioService_(boost::dynamic_pointer_cast<AsioService>(eventLoop)),
      tcpSocket_(boost::dynamic_pointer_cast<AsioService>(eventLoop)->service()),
      resolver_(boost::dynamic_pointer_cast<AsioService>(eventLoop)->service()),
      readBuffer_(Unpooled::buffer(DEFAULT_READER_BUFFER_SIZE)),
      writeBufferContainer_(),
      socketConfig_(tcpSocket_) {
    writeQueue_.reset(new AsioWriteOperationQueue(*this));
}

AsioSocketChannel::AsioSocketChannel(int id,
                                     const ChannelPtr& parent,
                                     const EventLoopPtr& eventLoop)
    : Channel(id, parent, eventLoop),
      isReading_(false),
      isWriting_(false),
      isConnecting_(false),
      initialized_(false),
      highWaterMarkCounter_(0),
      ioService_(boost::dynamic_pointer_cast<AsioService>(eventLoop)),
      tcpSocket_(boost::dynamic_pointer_cast<AsioService>(eventLoop)->service()),
      resolver_(boost::dynamic_pointer_cast<AsioService>(eventLoop)->service()),
      readBuffer_(Unpooled::buffer(DEFAULT_READER_BUFFER_SIZE)),
      writeBufferContainer_(),
      socketConfig_(tcpSocket_) {
    writeQueue_.reset(new AsioWriteOperationQueue(*this));
}

AsioSocketChannel::~AsioSocketChannel() {
    LOG_DEBUG << "AsioSocketChannel dtor";
}

void AsioSocketChannel::handleRead(const boost::system::error_code& error,
                                   size_t bytes_transferred) {
    if (!error) {
        LOG_INFO << "channel" << toString()
                 << "has read " << bytes_transferred << " bytes";

        readBuffer_->offsetWriterIndex(bytes_transferred);

        pipeline().addInboundChannelBuffer(readBuffer_);
        pipeline().fireMessageUpdated();

        beginRead();
    }
    else {
        LOG_ERROR << "channel " << toString()
                  << " read error, code:" << error.value()
                  << " message: " << error.message();

        isReading_ = false;

        if (isOpen()) {
            close(newVoidFuture());
        }
    }
}

void AsioSocketChannel::handleWrite(const boost::system::error_code& error,
                                    size_t bytes_transferred) {
    if (!error) {
        LOG_DEBUG << "channel " << toString()
                  << " written buffer with " << bytes_transferred << " bytes.";

        writeQueue_->front().setSuccess();
        writeQueue_->popFront();

        if (writeQueue_->empty()) {
            isWriting_ = false;
        }
    }
    else {
        ChannelException e(error.message(), error.value());

        while (!writeQueue_->empty()) {
            writeQueue_->front().setFailure(e);
            writeQueue_->popFront();
        }

        LOG_ERROR << "channel " << toString()
                  << " failed to write buffer, code: " << error.value()
                  << " message: " << error.message();

        if (isOpen()) {
            close(newVoidFuture());
        }
    }
}

void AsioSocketChannel::handleResolve(const boost::system::error_code& error,
                                      boost::asio::ip::tcp::resolver::iterator itr,
                                      const ChannelFuturePtr& cf) {
    if (!error) {
        tcp::endpoint endpoint = *itr;
        tcpSocket_.async_connect(
            endpoint,
            boost::bind(&AsioSocketChannel::handleConnect,
                        this,
                        boost::asio::placeholders::error,
                        ++itr,
                        cf));

        LOG_INFO << "channel " << toString()
                 << "resolved the remote address to "
                 << endpoint
                 << ", and begin to connect asynchronously";
    }
    else {
        LOG_ERROR << "channel " << toString()
                  << " can NOT resolve the remote address"
                  << ", code:" << error.value()
                  << " message:" << error.message();

        ChannelException exception(error.message(), error.value());
        connectFailed(cf, exception);
        return;
    }
}

void AsioSocketChannel::handleConnect(const boost::system::error_code& error,
                                      boost::asio::ip::tcp::resolver::iterator endpointIterator,
                                      const ChannelFuturePtr& cf) {
    if (!error) {
        if (connectTimeout_) {
            connectTimeout_->cancel();
            connectTimeout_.reset();
        }

        setActived();
        cf->setSuccess();

        LOG_INFO << "channel " << toString()
                 << "connected, firing connection event.";

        beginRead();
    }
    else if (endpointIterator != boost::asio::ip::tcp::resolver::iterator()) {
        LOG_INFO << "resolved more than one address, try to connect the next address.";
        boost::system::error_code error;
        tcpSocket_.close(error);

        if (error) {
            LOG_ERROR << "channel " << toString()
                      << " failed to close tcp socket before connect, code: "
                      << error.value()
                      << " message: "
                      << error.message()
                      << ", but skip it, error will be processed by the next handleConnect.";
        }

        tcp::endpoint endpoint = *endpointIterator;
        tcpSocket_.async_connect(endpoint,
                                 boost::bind(&AsioSocketChannel::handleConnect,
                                             this,
                                             boost::asio::placeholders::error,
                                             ++endpointIterator,
                                             cf));
    }
    else {
        LOG_ERROR << "channel " << toString()
                  << " failed to connect to remote server, code: "
                  << error.value()
                  << " message: " << error.message();

        ChannelException exception(error.message(), error.value());
        connectFailed(cf, exception);
    }
}

bool AsioSocketChannel::doConnect(const InetAddress& remoteAddress,
                                  const InetAddress& localAddress,
                                  const ChannelFuturePtr& connectFuture) {
    const std::string& hostname = remoteAddress.host();
    std::string port = StringUtil::numtostr(remoteAddress.port());

    boost::asio::ip::tcp::resolver::query query(hostname, port);

#if 0 // blocking DNS resolve
    boost::system::error_code error;
    boost::asio::ip::tcp::resolver::iterator iterator =
        resolver_.resolve(query, error);

    if (error) {
        LOG_ERROR << "boost asio can NOT resolve "
                  << hostname << ":" << port
                  << " , code:" << error.value()
                  << " message:" << error.message();

        ChannelException exception(error.message(), error.value());
        connectFailed(connectFuture, exception);
        return false;
    }

    boost::asio::ip::tcp::endpoint endpoint = *iterator;
    tcpSocket().async_connect(
        endpoint,
        boost::bind(&AsioSocketChannel::handleConnect,
                    this,
                    boost::asio::placeholders::error,
                    ++iterator,
                    connectFuture));
#else
    resolver_.async_resolve(query, boost::bind(
                                &AsioSocketChannel::handleResolve,
                                this,
                                _1,
                                _2,
                                connectFuture));
#endif

    const EventLoopPoolPtr& pool = eventLoop()->eventLoopPool();

    if (pool && pool->isSingleThread()) {
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
            pipeline().fireExceptionCaught(e);
            doClose();

            //TODO: should terminate the program
            std::terminate();
        }
    }

    return true;
}

void AsioSocketChannel::doConnect(ChannelHandlerContext& ctx,
                                  const InetAddress& remoteAddress,
                                  const InetAddress& localAddress,
                                  const ChannelFuturePtr& future) {
    if (!isOpen()) {
        LOG_WARN << "channel " << toString()
                 << " try to connect to remote before channel open.";
        return;
    }

    if (isConnecting_) {
        LOG_WARN << "channel " << toString()
                 << " connection attempt already made";
        return;
    }

    isConnecting_ = true;

    if (doConnect(remoteAddress, localAddress, future)) {
        // Schedule connect timeout.
        int connectTimeoutMillis = config().connectTimeout();

        if (connectTimeoutMillis > 0) {
            connectTimeout_ =
                eventLoop()->runAfter(connectTimeoutMillis,
                                      boost::bind(
                                          &AsioSocketChannel::handleConnectTimeout,
                                          this,
                                          boost::ref(future)));
        }
    }
}

void AsioSocketChannel::beginRead() {
    int size;
    char* buf = readBuffer_->writableBytes(&size);

    // auto increment the capacity.
    if (size < MIN_READER_BUFFER_SIZE) {
        if (!readBuffer_->ensureWritableBytes(4096, true)) {
            LOG_ERROR << "channel " << toString()
                      << "failed to get more writable bytes for read buffer";
        }

        buf = readBuffer_->writableBytes(&size);
    }

    LOG_INFO << "channel" << toString()
             << " begin to async read, with the the buffer size "
             << size << " Bytes";

    tcpSocket_.async_read_some(
        boost::asio::buffer(buf, size),
        boost::bind(&AsioSocketChannel::handleRead,
                    this,
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));

    isReading_ = true;
}

bool AsioSocketChannel::doBind(const InetAddress& localAddress) {
    return true;
}

bool AsioSocketChannel::doDisconnect() {
    return doClose();
}

bool AsioSocketChannel::doClose() {
    if (!isOpen()) {
        LOG_WARN << "channel " << toString()
                 << " do close, but its already closed.";
        return true;
    }

    cleanUpWriteBuffer();

    boost::system::error_code error;

    if (isActive()) {
        // if shutdown failed, try to close tcp socket directly.
        tcpSocket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, error);

        if (error) {
            LOG_ERROR << "channel " << toString()
                      << "failed to shutdown the tcp socket, code: "
                      << error.value() << " message: " << error.message();
        }
    }

    tcpSocket_.close(error);

    if (error) {
        LOG_ERROR << "channel " << toString()
                  << "failed to close the tcp socket, code: "
                  << error.value() << " message: " << error.message();
    }

    // FIXME: add failed result to close future?
    closeFuture()->setSuccess();

    return true;
}

void AsioSocketChannel::doFlush(ChannelHandlerContext& ctx,
                                const ChannelFuturePtr& future) {

    BOOST_ASSERT(writeBufferContainer_);

    const ChannelBufferPtr& buffer = writeBufferContainer_->getMessages();

    if (!isActive()) {
        LOG_ERROR << "channel " << toString()
                  << "failed to send the msg, because the socket is \
                     disconnected, and then close this channel";

        if (future) {
            future->setFailure(ChannelException("Channel is not active."));
        }

        doClose();
        return;
    }

    isWriting_ = true;
    AsioWriteOperation& operation = writeQueue_->offer(buffer, future);

    if (operation.writeBufferSize() == 0) {
        LOG_WARN << "channel " << toString()
                 << "write an empty message, do not write to the socket,\
                             just post a handleWrite operation.";
        ioService_->service().post(boost::bind(
                                       &AsioSocketChannel::handleWrite,
                                       this,
                                       boost::system::error_code(),
                                       0));
        return;
    }

    if (operation.isSingleBuffer()) {
        tcpSocket_.async_send(const_buffers_1(operation.asioBuffer()),
                              makeCustomAllocHandler(writeAllocator_,
                                      boost::bind(&AsioSocketChannel::handleWrite,
                                              this,
                                              boost::asio::placeholders::error,
                                              boost::asio::placeholders::bytes_transferred)));
        LOG_INFO << "channel " << toString()
                 << "write a buffer to the socket asynchronously";
    }
    else {
        tcpSocket_.async_send(operation.asioBufferArray(),
                              makeCustomAllocHandler(writeAllocator_,
                                      boost::bind(&AsioSocketChannel::handleWrite,
                                              this,
                                              boost::asio::placeholders::error,
                                              boost::asio::placeholders::bytes_transferred)));
        LOG_WARN << "channel " << toString()
                 << "write a gathering buffer to the socket asynchronously,\
                    may slow down the system.";
    }
}

void AsioSocketChannel::connectFailed(const ChannelFuturePtr& connectFuture,
                                      const ChannelException& e) {
    isConnecting_ = false;

    if (connectTimeout_) {
        connectTimeout_->cancel();
        connectTimeout_.reset();
    }

    connectFuture->setFailure(e);
    pipeline().fireExceptionCaught(e);
    //closeIfClosed();
    doClose();
}

void AsioSocketChannel::doPreOpen() {
    if (!initialized_) {
        Channel::config().setOptionSetCallback(boost::bind(
                &AsioSocketChannelConfig::setOption,
                &socketConfig_,
                _1,
                _2));

        // no need use weak_ptr here
        pipeline().setHead<AsioSocketChannel*>("head", this);

        initialized_ = true;
    }
}

void AsioSocketChannel::doPreActive() {
    boost::system::error_code ec;
    tcp::endpoint endpoint = tcpSocket_.remote_endpoint(ec);

    if (!ec) {
        uint16_t port = endpoint.port();
        std::string host = endpoint.address().to_string(ec);

        if (!ec) {
            setRemoteAddress(InetAddress(host, port));

            endpoint = tcpSocket_.local_endpoint(ec);

            if (!ec) {
                port = endpoint.port();
                host = endpoint.address().to_string(ec);

                if (!ec) {
                    setLocalAddress(InetAddress(host, port));
                }
                else {
                    LOG_WARN << "channel " << toString()
                             << "failed to get the host part of the local address";
                }
            }
            else {
                LOG_WARN << "channel " << toString()
                         << "failed to get the local address";
            }
        }
        else {
            LOG_WARN << "channel " << toString()
                     << "failed to get the host part of the remote address";
        }
    }
    else {
        LOG_WARN << "channel " << toString()
                 << "failed to get the remote address";
    }
}

void AsioSocketChannel::registerTo(Context& context) {
    Channel::registerTo(context);

    writeBufferContainer_ = context.outboundContainer();

    context.setConnectFunctor(boost::bind(
                                  &AsioSocketChannel::doConnect,
                                  this,
                                  _1,
                                  _2,
                                  _3,
                                  _4));

    context.setFlushFunctor(boost::bind(
                                &AsioSocketChannel::doFlush,
                                this,
                                _1,
                                _2));
}

void AsioSocketChannel::handleConnectTimeout(const ChannelFuturePtr& future) {
    ChannelException e("connection timed out");
    connectFailed(future, e);
}

void AsioSocketChannel::cleanUpWriteBuffer() {
    // last one in the writeBuffer should not been cleaned.
    // the handleWrite callback may be already queued into the AsioService,
    // it should take care of itself.
    if (!writeQueue_->empty()) {
        ChannelException e("channel closed");
        AsioWriteOperation firstOp = writeQueue_->front();
        writeQueue_->popFront();

        while (!writeQueue_->empty()) {
            writeQueue_->front().setFailure(e);
            writeQueue_->popFront();
        }

        writeQueue_->offer(firstOp);
    }
}

}
}
}
