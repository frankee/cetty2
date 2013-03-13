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

using namespace cetty::channel;
using namespace cetty::buffer;
using namespace cetty::util;

using namespace boost::asio::ip;

AsioSocketChannel::AsioSocketChannel(const ChannelPtr& parent,
                                     const EventLoopPtr& eventLoop)
    : Channel(parent, eventLoop),
      isReading_(false),
      isWriting_(false),
      initialized_(false),
      highWaterMarkCounter_(0),
      ioService_(boost::dynamic_pointer_cast<AsioService>(eventLoop)),
      tcpSocket_(boost::dynamic_pointer_cast<AsioService>(eventLoop)->service()),
      readBuffer_(Unpooled::buffer(1024 * 16)),
      bufferContainer_(),
      socketConfig_(tcpSocket_) {
    writeQueue_.reset(new AsioWriteOperationQueue(*this));
}

AsioSocketChannel::AsioSocketChannel(const EventLoopPtr& eventLoop)
    : Channel(ChannelPtr(), eventLoop),
      isReading_(false),
      isWriting_(false),
      initialized_(false),
      highWaterMarkCounter_(0),
      ioService_(boost::dynamic_pointer_cast<AsioService>(eventLoop)),
      tcpSocket_(boost::dynamic_pointer_cast<AsioService>(eventLoop)->service()),
      readBuffer_(Unpooled::buffer(1024 * 16)),
      bufferContainer_(),
      socketConfig_(tcpSocket_) {
    writeQueue_.reset(new AsioWriteOperationQueue(*this));
}

AsioSocketChannel::AsioSocketChannel(int id, const EventLoopPtr& eventLoop)
    : Channel(id, ChannelPtr(), eventLoop),
      isReading_(false),
      isWriting_(false),
      initialized_(false),
      highWaterMarkCounter_(0),
      ioService_(boost::dynamic_pointer_cast<AsioService>(eventLoop)),
      tcpSocket_(boost::dynamic_pointer_cast<AsioService>(eventLoop)->service()),
      readBuffer_(Unpooled::buffer(1024 * 16)),
      bufferContainer_(),
      socketConfig_(tcpSocket_) {
    writeQueue_.reset(new AsioWriteOperationQueue(*this));
}

AsioSocketChannel::AsioSocketChannel(int id,
                                     const ChannelPtr& parent,
                                     const EventLoopPtr& eventLoop)
    : Channel(id, parent, eventLoop),
      isReading_(false),
      isWriting_(false),
      initialized_(false),
      highWaterMarkCounter_(0),
      ioService_(boost::dynamic_pointer_cast<AsioService>(eventLoop)),
      tcpSocket_(boost::dynamic_pointer_cast<AsioService>(eventLoop)->service()),
      readBuffer_(Unpooled::buffer(1024 * 16)),
      bufferContainer_(),
      socketConfig_(tcpSocket_) {
    writeQueue_.reset(new AsioWriteOperationQueue(*this));
}

AsioSocketChannel::~AsioSocketChannel() {
    LOG_DEBUG << "AsioSocketChannel dctor";
}

#if 0
const InetAddress& AsioSocketChannel::localAddress() const {
    if (localAddress.validated()) {
        return localAddress;
    }

    boost::system::error_code ec;
    tcp::endpoint endpoint = tcpSocket_.local_endpoint(ec);

    if (!ec) {
        localAddress = InetAddress(InetAddressImplPtr(
                                       new AsioTcpInetAddressImpl(
                                           ioService_->service(),
                                           endpoint)));
    }
    else {
        LOG_ERROR << "failed to get the local address of the channel from asio.";
    }

    return localAddress;
}

const InetAddress& AsioSocketChannel::remoteAddress() const {
    if (remoteAddress.validated()) {
        return remoteAddress;
    }

    boost::system::error_code ec;
    tcp::endpoint endpoint = tcpSocket_.remote_endpoint(ec);

    if (!ec) {
        remoteAddress = InetAddress(InetAddressImplPtr(
                                        new AsioTcpInetAddressImpl(
                                            ioService_->service(),
                                            endpoint)));
    }
    else {
        LOG_ERROR << "failed to get the remote address of the channel from asio.";
    }

    return remoteAddress;
}
#endif

void AsioSocketChannel::handleRead(const boost::system::error_code& error,
                                   size_t bytes_transferred) {
    if (!error) {
        readBuffer_->offsetWriterIndex(bytes_transferred);

        pipeline().addInboundChannelBuffer(readBuffer_);
        pipeline().fireMessageUpdated();

        beginRead();
    }
    else {
        isReading_ = false;

        if (isOpen()) {
            close(newVoidFuture());
        }

        closeFuture()->setSuccess();
    }
}

void AsioSocketChannel::handleWrite(const boost::system::error_code& error,
                                    size_t bytes_transferred) {
    if (!error) {
        writeQueue_->peek().setSuccess();
        writeQueue_->popup();

        if (writeQueue_->empty()) {
            isWriting_ = false;
        }
    }
    else {
        if (!writeQueue_->empty()) {
            std::string msg;
            StringUtil::printf(&msg, "write buffer failed, message:%s code:%d",
                               error.message().c_str(),
                               error.value());

            writeQueue_->peek().setFailure(RuntimeException(msg));
            writeQueue_->popup();

            LOG_ERROR << msg;
        }

        close(newVoidFuture());
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
    }
    else {
        LOG_ERROR << "boost asio can NOT resolve "
                  << remoteAddress().toString()
                  << " , code=" << error.value() << ", msg=" << error.message();

        ChannelException exception(error.message(), error.value());
        connectFailed(cf, exception);
        return;
    }
}

void AsioSocketChannel::handleConnect(const boost::system::error_code& error,
                                      boost::asio::ip::tcp::resolver::iterator endpointIterator,
                                      const ChannelFuturePtr& cf) {
    if (!error) {
        LOG_INFO << "channel connected to the remote server %s, firing connected event."
                 << remoteAddress().toString();
        
        setActived();
        pipeline().fireChannelActive();
        cf->setSuccess();

        beginRead();
    }
    else if (endpointIterator != boost::asio::ip::tcp::resolver::iterator()) {
        LOG_INFO << "resolved more than one address, try to connect the next address.";
        boost::system::error_code error;
        tcpSocket_.close(error);

        if (error) {
            LOG_ERROR << "failed to close tcp socket before connect, "
                      << error.value()
                      << ":"
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
        LOG_ERROR << "failed to connect to remote server "
                  << remoteAddress().toString()
                  << " , code=" << error.value() << ", msg=" << error.message();

        ChannelException exception(error.message(), error.value());
        connectFailed(cf, exception);
    }
}

void AsioSocketChannel::cleanUpWriteBuffer() {
    ChannelException cause;
    bool fireExceptionCaught = false;

    // Clean up the stale messages in the write buffer.
    if (!writeQueue_->empty()) {
        if (isOpen()) {
            LOG_WARN << "cleanUpWriteBuffer: The channel has not close yet.";
            cause = ChannelException("Channel has not close yet.");
        }
        else {
            cause = ChannelException("Channel has closed.");
        }

        // last one in the writeBuffer should not been cleaned.
        // it is already sent asynchronously, will take care of itself.
        while (writeQueue_->size() > 1) {
            LOG_WARN << "cleanUpWriteBuffer: poll and clean the write queue.";
            writeQueue_->peek().setFailure(cause);
            writeQueue_->popup();

            fireExceptionCaught = true;
        }
    }

    if (fireExceptionCaught) {
        LOG_WARN << "cleanUpWriteBuffer: firing the the exception event.";
        pipeline().fireExceptionCaught(cause);
    }
}

void AsioSocketChannel::doConnect(const InetAddress& remoteAddress,
                                  const InetAddress& localAddress,
                                  const ChannelFuturePtr& connectFuture) {
    const std::string& hostname = remoteAddress.host();
    std::string port = StringUtil::numtostr(remoteAddress.port());

    boost::asio::ip::tcp::resolver resolver(ioService()->service());
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

        close(closeFuture());
        return;
    }

    boost::asio::ip::tcp::endpoint endpoint = *iterator;
    tcpSocket().async_connect(
        endpoint,
        boost::bind(&AsioSocketChannel::handleConnect,
                    this,
                    boost::asio::placeholders::error,
                    ++iterator,
                    connectFuture));

    //     resolver.async_resolve(query, boost::bind(
    //                                &AsioSocketChannel::handleResolve,
    //                                this,
    //                                _1,
    //                                _2,
    //                                connectFuture));

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
}

void AsioSocketChannel::doConnect(ChannelHandlerContext& ctx,
                                  const InetAddress& remoteAddress,
                                  const InetAddress& localAddress,
                                  const ChannelFuturePtr& future) {
    if (!isOpen()) {
        return;
    }

    try {
        //if (connectFuture != null) {
        //    throw new IllegalStateException("connection attempt already made");
        //}

        //connectFuture = future;

        doConnect(remoteAddress, localAddress, future);

        // Schedule connect timeout.
        //int connectTimeoutMillis = config().connectTimeout();

        //         if (connectTimeoutMillis > 0) {
        //             eventLoop()->runAfter(connectTimeoutMillis, boost::bind(
        //                 &AsioSocketChannel::handleConnectTimeout,
        //                 this));
        //         }

    }
    catch (const std::exception& t) {
        //future.setFailure(t);
        //pipeline().fireExceptionCaught(t);
        closeIfClosed();
    }
}

void AsioSocketChannel::beginRead() {
    int size;
    char* buf = readBuffer_->writableBytes(&size);
    LOG_INFO << "AsioSocketChannel begin to async read, with the the buffer size : "
             << size;

    // auto increment the capacity.
    if (size < 128) {
        if (!readBuffer_->ensureWritableBytes(4096, true)) {
            // error!
        }

        buf = readBuffer_->writableBytes(&size);
    }

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
        LOG_WARN << "do close the socket channel, but the channel already closed.";
        return true;
    }

    cleanUpWriteBuffer();

    boost::system::error_code error;

    if (isActive()) {
        // if shutdown failed, try to close tcp socket directly.
        tcpSocket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, error);

        if (error) {
            LOG_ERROR << "failed to shutdown the tcp socket"
                      << error.value() << ":" << error.message();
        }
    }

    tcpSocket_.close(error);

    if (error) {
        LOG_ERROR << "failed to close the tcp socket."
                  << error.value() << ":" << error.message();
    }

    if (!isReading_) {
        closeFuture()->setSuccess();
    }

    return true;
}

void AsioSocketChannel::doFlush(ChannelHandlerContext& ctx,
                                const ChannelFuturePtr& future) {

    BOOST_ASSERT(bufferContainer_);

    const ChannelBufferPtr& buffer = bufferContainer_->getMessages();

    if (!isActive()) {
        LOG_WARN << "sending the msg, but the socket is disconnected, clean up write buffer.";
        cleanUpWriteBuffer();

        if (future) {
            future->setFailure(ChannelException("Channel has been closed."));
        }

        doClose();

        return;
    }

    isWriting_ = true;
    AsioWriteOperation& operation = writeQueue_->offer(buffer, future);

    if (operation.writeBufferSize() == 0) {
        LOG_WARN << "write an empty message, do not write to the socket,\
                             just post a handleWrite operation.";
        ioService_->service().post(boost::bind(
                                       &AsioSocketChannel::handleWrite,
                                       this,
                                       boost::system::error_code(),
                                       0));
        return;
    }

    if (operation.isMultiBuffers()) {
        boost::asio::async_write(tcpSocket_,
                                 operation.getAsioBufferArray(),
                                 makeCustomAllocHandler(writeAllocator_,
                                         boost::bind(&AsioSocketChannel::handleWrite,
                                                 this,
                                                 boost::asio::placeholders::error,
                                                 boost::asio::placeholders::bytes_transferred)));
        LOG_WARN << "write a gathering buffer to the socket, may slow down the system.";
    }
    else {
        boost::asio::async_write(tcpSocket_,
                                 boost::asio::const_buffers_1(operation.getAsioBuffer()),
                                 makeCustomAllocHandler(writeAllocator_,
                                         boost::bind(&AsioSocketChannel::handleWrite,
                                                 this,
                                                 boost::asio::placeholders::error,
                                                 boost::asio::placeholders::bytes_transferred)));
        LOG_INFO << "write a buffer to the socket";
    }
}

void AsioSocketChannel::connectFailed(const ChannelFuturePtr& connectFuture,
                                      const ChannelException& e) {
    connectFuture->setFailure(e);
    pipeline().fireExceptionCaught(e);
    //closeIfClosed();
    doClose();
}

void AsioSocketChannel::doInitialize() {
    if (!initialized_) {
        Channel::config().setOptionSetCallback(boost::bind(
                &AsioSocketChannelConfig::setOption,
                &socketConfig_,
                _1,
                _2));

        // no need use weak_ptr here
        pipeline().setHead<AsioSocketChannel*>("bridge", this);

        initialized_ = true;
    }
}

void AsioSocketChannel::registerTo(Context& context) {
    Channel::registerTo(context);

    bufferContainer_ = context.outboundContainer();

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

void AsioSocketChannel::handleConnectTimeout() {
    //         if (connectTimeoutException == null) {
    //             connectTimeoutException = new ConnectException("connection timed out");
    //         }
    //
    //         ChannelFuture connectFuture = AbstractAioChannel.this.connectFuture;
    //
    //         if (connectFuture != null &&
    //             connectFuture.setFailure(connectTimeoutException)) {
    //                 pipeline().fireExceptionCaught(connectTimeoutException);
    //                 close(voidFuture());
    //         }
}

void AsioSocketChannel::handleConnectFailed() {
    //         connectFuture.setFailure(t);
    //         pipeline().fireExceptionCaught(t);
    //         closeIfClosed();
}

void AsioSocketChannel::handleConnectSuccess() {
    //         assert eventLoop().inEventLoop();
    //         assert connectFuture != null;
    //
    //         try {
    //             boolean wasActive = isActive();
    //             connectFuture.setSuccess();
    //
    //             if (!wasActive && isActive()) {
    //                 pipeline().fireChannelActive();
    //             }
    //         }
    //         catch (Throwable t) {
    //             connectFuture.setFailure(t);
    //             pipeline().fireExceptionCaught(t);
    //             closeIfClosed();
    //         } finally {
    //
    //             connectTimeoutFuture.cancel(false);
    //             connectFuture = null;
    //         }
}

}
}
}
