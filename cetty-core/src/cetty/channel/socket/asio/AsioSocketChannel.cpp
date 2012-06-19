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
#include <boost/assert.hpp>
#include <boost/bind.hpp>

#include <cetty/channel/Channels.h>
#include <cetty/channel/SocketAddress.h>
#include <cetty/channel/ChannelFuture.h>
#include <cetty/channel/ChannelFactory.h>
#include <cetty/channel/ChannelPipeline.h>
#include <cetty/channel/ChannelException.h>
#include <cetty/channel/MessageEvent.h>
#include <cetty/channel/ChannelMessage.h>
#include <cetty/channel/UpstreamMessageEvent.h>
#include <cetty/channel/DownstreamMessageEvent.h>
#include <cetty/channel/DownstreamChannelStateEvent.h>
#include <cetty/channel/CopyableDownstreamMessageEvent.h>
#include <cetty/channel/CopyableDownstreamChannelStateEvent.h>
#include <cetty/channel/DefaultWriteCompletionEvent.h>
#include <cetty/channel/ReceiveBufferSizePredictor.h>
#include <cetty/channel/ReceiveBufferSizePredictorFactory.h>

#include <cetty/channel/socket/asio/AsioServicePool.h>
#include <cetty/channel/socket/asio/AsioSocketAddressImpl.h>
#include <cetty/channel/socket/asio/AsioWriteOperationQueue.h>

#include <cetty/buffer/ChannelBuffer.h>
#include <cetty/buffer/ChannelBufferFactory.h>
#include <cetty/buffer/CompositeChannelBuffer.h>

#include <cetty/logging/LoggerHelper.h>
#include <cetty/logging/InternalLoggerFactory.h>
#include <cetty/util/Integer.h>

namespace cetty {
namespace channel {
namespace socket {
namespace asio {

using namespace cetty::channel;
using namespace cetty::buffer;
using namespace cetty::util;

InternalLogger* AsioSocketChannel::logger;

AsioSocketChannel::AsioSocketChannel(const ChannelPtr& parent,
                                     const ChannelFactoryPtr& factory,
                                     const ChannelPipelinePtr& pipeline,
                                     const ChannelSinkPtr& sink,
                                     const AsioServicePtr& ioService,
                                     const boost::thread::id& id)
    : SocketChannel(parent, factory, pipeline, sink),

      threadId(id),
      ioService(ioService),
      tcpSocket(ioService->service()),
      isWriting(false),
      highWaterMarkCounter(0),
      config(tcpSocket),
      state(CHANNEL_ST_OPEN) {
    const ChannelBufferFactoryPtr& bufferFactory = config.getBufferFactory();
    const ReceiveBufferSizePredictorPtr& predictor = config.getReceiveBufferSizePredictor();
    readBuffer = bufferFactory->getBuffer(
                     bufferFactory->getDefaultOrder(),
                     predictor->nextReceiveBufferSize());

    if (NULL == logger) {
        logger = InternalLoggerFactory::getInstance("AsioSocketChannel");
    }

    writeQueue.reset(new AsioWriteOperationQueue(*this, logger));
}

AsioSocketChannel::~AsioSocketChannel() {
}

const SocketAddress& AsioSocketChannel::getLocalAddress() const {
    if (localAddress.validated()) {
        return localAddress;
    }

    boost::system::error_code ec;
    boost::asio::ip::tcp::endpoint endpoint = tcpSocket.local_endpoint(ec);

    if (!ec) {
        localAddress = SocketAddress(
                           SocketAddress::SmartPointer(new AsioTcpSocketAddressImpl(ioService->service(), endpoint)));
    }
    else {
        LOG_ERROR(logger, "failed to get the local address of the channel from asio.");
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
        remoteAddress = SocketAddress(
                            SocketAddress::SmartPointer(new AsioTcpSocketAddressImpl(ioService->service(), endpoint)));
    }
    else {
        LOG_ERROR(logger, "failed to get the remote address of the channel from asio.");
    }

    return remoteAddress;
}

int AsioSocketChannel::getInterestOps() const {
    if (!isOpen()) {
        return Channel::OP_WRITE;
    }

    int interestOps = getRawInterestOps();

    int writeBufferSize = writeQueue->getWriteBufferSize();

    if (writeBufferSize != 0) {
        if (highWaterMarkCounter > 0) {
            int lowWaterMark = config.getWriteBufferLowWaterMark();

            if (writeBufferSize >= lowWaterMark) {
                interestOps |= Channel::OP_WRITE;
            }
            else {
                interestOps &= ~Channel::OP_WRITE;
            }
        }
        else {
            int highWaterMark = config.getWriteBufferHighWaterMark();

            if (writeBufferSize >= highWaterMark) {
                interestOps |= Channel::OP_WRITE;
            }
            else {
                interestOps &= ~Channel::OP_WRITE;
            }
        }
    }
    else {
        interestOps &= ~Channel::OP_WRITE;
    }

    return interestOps;
}

ChannelFuturePtr AsioSocketChannel::write(const ChannelMessage& message,
        const SocketAddress& remoteAddress) {
    if (!remoteAddress.validated() || remoteAddress == this->remoteAddress) {
        return write(message);
    }
    else {
        LOG_ERROR(logger, "TCP socket NOT ALLOWED writing message to different remote address.");
        return getUnsupportedOperationFuture();
    }
}

ChannelFuturePtr AsioSocketChannel::write(const ChannelMessage& message) {
    ChannelFuturePtr future = Channels::future(this);

    if (boost::this_thread::get_id() == threadId) {
        LOG_INFO(logger, "write message, sending the msg to the pipeline.");
        pipeline->sendDownstream(
            DownstreamMessageEvent(this, future, message, this->remoteAddress));
    }
    else {
        LOG_INFO(logger, "write message in different thread, post the msg to pipeline.");
        ioService->service().post(
            makeCustomAllocHandler(ipcWriteAllocator,
                                   boost::bind<void, ChannelPipeline, const MessageEvent&>(
                                       &ChannelPipeline::sendDownstream,
                                       pipeline,
                                       CopyableDownstreamMessageEvent(
                                           this, future, message, this->remoteAddress))));
    }

    return future;
}

void AsioSocketChannel::innerWrite(const MessageEvent& evt) {
    const ChannelFuturePtr& f = evt.getFuture();

    if (!isConnected()) {
        LOG_WARN(logger, "sending the msg, but the socket is disconnected, clean up write buffer.");
        cleanUpWriteBuffer();

        if (f) {
            f->setFailure(ChannelException("Channel has been closed."));
        }

        return;
    }

    isWriting = true;
    AsioWriteOperation& operation = writeQueue->offer(evt, f);

    if (operation.writeBufferSize() == 0) {
        LOG_WARN(logger, "write an empty message, do not write to the socket,\
                         just post a handleWrite operation.");
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
        LOG_WARN(logger, "write a gathering buffer to the socket, may slow down the system.");
    }
    else {
        boost::asio::async_write(tcpSocket,
                                 boost::asio::const_buffers_1(operation.getAsioBuffer()),
                                 makeCustomAllocHandler(writeAllocator,
                                         boost::bind(&AsioSocketChannel::handleWrite,
                                                 this,
                                                 boost::asio::placeholders::error,
                                                 boost::asio::placeholders::bytes_transferred)));
        LOG_INFO(logger, "write a buffer to the socket");
    }
}

cetty::channel::ChannelFuturePtr AsioSocketChannel::unbind() {
    ChannelFuturePtr future = Channels::future(this);

    if (boost::this_thread::get_id() == threadId) {
        pipeline->sendDownstream(DownstreamChannelStateEvent(
                                     this, future, ChannelState::BOUND));
    }
    else {
        ioService->service().post(
            makeCustomAllocHandler(ipcStateChangeAllocator,
                                   boost::bind<void, ChannelPipeline, const ChannelStateEvent&>(
                                       &ChannelPipeline::sendDownstream,
                                       pipeline,
                                       CopyableDownstreamChannelStateEvent(
                                           this, future, ChannelState::BOUND))));
    }

    return future;
}

cetty::channel::ChannelFuturePtr AsioSocketChannel::close() {
    if (closeFuture->isDone()) {
        return closeFuture;
    }

    if (boost::this_thread::get_id() == threadId) {
        pipeline->sendDownstream(DownstreamChannelStateEvent(
                                     this, closeFuture, ChannelState::OPEN));
    }
    else {
        ioService->service().post(
            makeCustomAllocHandler(ipcStateChangeAllocator,
                                   boost::bind<void, ChannelPipeline, const ChannelStateEvent&>(
                                       &ChannelPipeline::sendDownstream,
                                       pipeline,
                                       CopyableDownstreamChannelStateEvent(
                                           this, closeFuture, ChannelState::OPEN))));
    }

    return closeFuture;
}

void AsioSocketChannel::innerClose(const ChannelFuturePtr& future) {
    if (!isOpen() || !tcpSocket.is_open() || getCloseFuture()->isDone()) {
        LOG_INFO(logger, "close the socket channel, but the channel already closed.");
        return;
    }

    boost::system::error_code error;

    if (isConnected()) {
        // if shutdown failed, try to close tcp socket directly.
        tcpSocket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, error);

        if (error) {
            LOG_ERROR(logger, "failed to shutdown the tcp socket, %d:%s.",
                      error.value(), error.message().c_str());
        }
    }

    tcpSocket.close(error);

    if (error) {
        LOG_ERROR(logger, "failed to close the tcp socket. %d:%s.",
                  error.value(), error.message().c_str());

        IOException e("failed to close the tcp socket.", error.value());
        future->setFailure(e);

        setClosed();
        Channels::fireExceptionCaught(this, e);
    }
    else {
        future->setSuccess();
        setClosed();
    }
}

bool AsioSocketChannel::setClosed() {
    if (isConnected()) {
        LOG_INFO(logger, "closed the socket channel, firing channel disconnected event.");
        Channels::fireChannelDisconnected(this);
    }

    if (isBound()) {
        LOG_INFO(logger, "closed the socket channel, firing channel unbound event.");
        Channels::fireChannelUnbound(this);
    }

    setClosedState();
    cleanUpWriteBuffer();

    LOG_INFO(logger, "closed the socket channel, finally firing channel closed event.");
    Channels::fireChannelClosed(this);

    LOG_INFO(logger, "closed the socket channel, finally calling FutureListener"
             " which to the ChannelCloseFuture.");

    return AbstractChannel::setClosed();
}

cetty::channel::ChannelFuturePtr AsioSocketChannel::disconnect() {
    ChannelFuturePtr future = Channels::future(this);

    if (boost::this_thread::get_id() == threadId) {
        pipeline->sendDownstream(DownstreamChannelStateEvent(
                                     this, future, ChannelState::CONNECTED));
    }
    else {
        ioService->service().post(
            makeCustomAllocHandler(ipcStateChangeAllocator,
                                   boost::bind<void, ChannelPipeline, const ChannelStateEvent&>(
                                       &ChannelPipeline::sendDownstream,
                                       pipeline,
                                       CopyableDownstreamChannelStateEvent(
                                           this, future, ChannelState::CONNECTED))));
    }

    return future;
}

cetty::channel::ChannelFuturePtr AsioSocketChannel::setInterestOps(int interestOps) {
    interestOps = Channels::validateAndFilterDownstreamInteresOps(interestOps);
    ChannelFuturePtr future = Channels::future(this);

    if (boost::this_thread::get_id() == threadId) {
        pipeline->sendDownstream(DownstreamChannelStateEvent(
                                     this, future, ChannelState::INTEREST_OPS, boost::any(interestOps)));
    }
    else {
        ioService->service().post(
            makeCustomAllocHandler(ipcStateChangeAllocator,
                                   boost::bind<void, ChannelPipeline, const ChannelStateEvent&>(
                                       &ChannelPipeline::sendDownstream,
                                       pipeline,
                                       CopyableDownstreamChannelStateEvent(
                                           this, future, ChannelState::INTEREST_OPS, boost::any(interestOps)))));
    }

    return future;
}

void AsioSocketChannel::innerSetInterestOps(const ChannelFuturePtr& future, int interestOps) {
    bool isOrgReadable = isReadable();

    // Override OP_WRITE flag - a user cannot change this flag.
    interestOps &= ~Channel::OP_WRITE;
    interestOps |= getRawInterestOps() & Channel::OP_WRITE;

    setRawInterestOpsNow(interestOps);

    bool isNowReadable = isReadable();
    bool changed = isOrgReadable != isNowReadable;

    if (changed && isNowReadable) {
        Array readerArray;
        readBuffer->writableBytes(&readerArray);
        tcpSocket.async_read_some(
            boost::asio::buffer(readerArray.data(), readerArray.length()),
            boost::bind(&AsioSocketChannel::handleRead,
                        this,
                        boost::asio::placeholders::error,
                        boost::asio::placeholders::bytes_transferred));
    }

    future->setSuccess();

    if (changed) {
        Channels::fireChannelInterestChanged(this, interestOps);
    }
}

void AsioSocketChannel::handleRead(const boost::system::error_code& error,
                                   size_t bytes_transferred) {
    if (!error) {
        readBuffer->offsetWriterIndex(bytes_transferred);

        // Fire the event.
        pipeline->sendUpstream(
            UpstreamMessageEvent(this, ChannelMessage(readBuffer), remoteAddress));
        //Channels::fireMessageReceived(*this, ChannelMessage(readBuffer));

        if (interestOps & OP_READ) { //readable
            Array arry;
            readBuffer->writableBytes(&arry);
            tcpSocket.async_read_some(
                boost::asio::buffer(arry.data(), arry.length()),
                makeCustomAllocHandler(readAllocator,
                                       boost::bind(&AsioSocketChannel::handleRead,
                                                   this,
                                                   boost::asio::placeholders::error,
                                                   boost::asio::placeholders::bytes_transferred)));
        }
    }
    else {
        close();
    }
}

void AsioSocketChannel::handleWrite(const boost::system::error_code& error,
                                    size_t bytes_transferred) {
    if (!error) {
        writeQueue->poll().setSuccess();

        pipeline->sendUpstream(
            DefaultWriteCompletionEvent(this, bytes_transferred));
        //Channels::fireWriteComplete(*this, bytes_transferred);

        if (writeQueue->empty()) {
            isWriting = false;
        }
    }
    else {
        writeQueue->poll().setFailure(
            RuntimeException(std::string("write buffer failed, code=") +
                             Integer::toString(error.value())));
        close();
    }
}

void AsioSocketChannel::handleConnect(const boost::system::error_code& error,
                                      boost::asio::ip::tcp::resolver::iterator endpoint_iterator,
                                      const ChannelFuturePtr& cf) {
    if (!error) {
        setConnectedState();
        LOG_INFO(logger, "channel connected to the remote server %s, firing connected event.", remoteAddress.toString().c_str());
        Channels::fireChannelConnected(this, remoteAddress);
        cf->setSuccess();

        if (isReadable()) {
            Array arry;
            readBuffer->writableBytes(&arry);
            LOG_INFO(logger, "channel begin to receive %d bytes data.", arry.length());

            tcpSocket.async_read_some(
                boost::asio::buffer(arry.data(), arry.length()),
                makeCustomAllocHandler(readAllocator,
                                       boost::bind(&AsioSocketChannel::handleRead,
                                                   this,
                                                   boost::asio::placeholders::error,
                                                   boost::asio::placeholders::bytes_transferred)));
        }
    }
    else if (endpoint_iterator != boost::asio::ip::tcp::resolver::iterator()) {
        LOG_WARN(logger, "resolved more than one address, try to connect the next address.");
        boost::system::error_code error;
        tcpSocket.close(error);

        if (error) {
            LOG_ERROR(logger, "failed to close tcp socket before connect, %d:%s, but skip it.",
                      error.value(), error.message());
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
        innerClose(cf);
    }
}

void AsioSocketChannel::cleanUpWriteBuffer() {
    ChannelException cause;
    bool fireExceptionCaught = false;

    // Clean up the stale messages in the write buffer.
    if (!writeQueue->empty()) {
        if (isOpen()) {
            LOG_WARN(logger, "cleanUpWriteBuffer: The channel has not close yet.");
            cause = ChannelException("Channel has not close yet.");
        }
        else {
            cause = ChannelException("Channel has closed.");
        }

        // last one in the writeBuffer should not been cleaned.
        // it is already sent asynchronously, will take care of itself.
        while (writeQueue->size() > 1) {
            LOG_WARN(logger, "cleanUpWriteBuffer: poll and clean the write queue.");
            writeQueue->poll().setFailure(cause);
            fireExceptionCaught = true;
        }
    }

    if (fireExceptionCaught) {
        LOG_WARN(logger, "cleanUpWriteBuffer: firing the the exception event.");
        Channels::fireExceptionCaught(this, cause);
    }
}

void AsioSocketChannel::handleAtHighWaterMark() {
    ++highWaterMarkCounter;

    if (isConnected()) {
        LOG_WARN(logger, "handleAtHighWaterMark: firing the the Interest Changed event.");
        Channels::fireChannelInterestChanged(this, getInterestOps());
    }
}

void AsioSocketChannel::handleAtLowWaterMark() {
    --highWaterMarkCounter;

    if (isConnected()) {
        LOG_WARN(logger, "handleAtLowWaterMark: firing the the Interest Changed event.");
        Channels::fireChannelInterestChanged(this, getInterestOps());
    }
}

bool AsioSocketChannel::isOpen() const {
    return state >= CHANNEL_ST_OPEN;
}

bool AsioSocketChannel::isBound() const {
    return state >= CHANNEL_ST_BOUND;
}

bool AsioSocketChannel::isConnected() const {
    return state == CHANNEL_ST_CONNECTED;
}

void AsioSocketChannel::setClosedState() {
    state = CHANNEL_ST_CLOSED;
}

void AsioSocketChannel::setBoundState() {
    BOOST_ASSERT(state == CHANNEL_ST_OPEN && "Invalid state.");
    state = CHANNEL_ST_BOUND;
}

void AsioSocketChannel::setConnectedState() {
    if (state != CHANNEL_ST_CLOSED) {
        state = CHANNEL_ST_CONNECTED;
        LOG_INFO(logger, "set the channel state to opened.");
    }
    else {
        LOG_WARN(logger, "the channel is closed, can not open again.");
    }
}

}
}
}
}
