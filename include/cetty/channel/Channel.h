#if !defined(CETTY_CHANNEL_CHANNEL_H)
#define CETTY_CHANNEL_CHANNEL_H

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

#include <string>
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <boost/enable_shared_from_this.hpp>

#include <cetty/channel/EventLoop.h>
#include <cetty/channel/ChannelPtr.h>
#include <cetty/channel/ChannelConfig.h>
#include <cetty/channel/ChannelFuture.h>
#include <cetty/channel/ChannelPipeline.h>
#include <cetty/channel/ChannelHandlerContext.h>
#include <cetty/channel/ChannelHandlerWrapper.h>

#include <cetty/util/Exception.h>
#include <cetty/util/ReferenceCounter.h>

namespace cetty {
namespace channel {

using namespace cetty::util;

class InetAddress;
class ChannelConfig;
class ChannelSink;

/**
 * A nexus to a network socket or a component which is capable of I/O
 * operations such as read, write, connect, and bind.
 * <p>
 * A channel provides a user:
 * <ul>
 * <li>the current state of the channel (e.g. is it open? is it connected?),</li>
 * <li>the {@linkplain ChannelConfig configuration parameters} of the channel (e.g. receive buffer size),</li>
 * <li>the I/O operations that the channel supports (e.g. read, write, connect, and bind), and</li>
 * <li>the {@link ChannelPipeline} which handles all I/O events and requests
 *     associated with the channel.</li>
 * </ul>
 *
 * <h3>All I/O operations are asynchronous.</h3>
 * <p>
 * All I/O operations in Netty are asynchronous.  It means any I/O calls will
 * return immediately with no guarantee that the requested I/O operation has
 * been completed at the end of the call.  Instead, you will be returned with
 * a {@link ChannelFuture} instance which will notify you when the requested I/O
 * operation has succeeded, failed, or canceled.
 *
 * <h3>Channels are hierarchical</h3>
 * <p>
 * A {@link Channel} can have a {@linkplain #parent() parent} depending on
 * how it was created.  For instance, a {@link SocketChannel}, that was accepted
 * by {@link ServerSocketChannel}, will return the {@link ServerSocketChannel}
 * as its parent on {@link #parent()}.
 * <p>
 * The semantics of the hierarchical structure depends on the transport
 * implementation where the {@link Channel} belongs to.  For example, you could
 * write a new {@link Channel} implementation that creates the sub-channels that
 * share one socket connection, as <a href="http://beepcore.org/">BEEP</a> and
 * <a href="http://en.wikipedia.org/wiki/Secure_Shell">SSH</a> do.
 *
 * <h3>Downcast to access transport-specific operations</h3>
 * <p>
 * Some transports exposes additional operations that is specific to the
 * transport.  Down-cast the {@link Channel} to sub-type to invoke such
 * operations.  For example, with the old I/O datagram transport, multicast
 * join / leave operations are provided by {@link DatagramChannel}.
 *
 * @dot
 * strict digraph {
 * node            [fontname=Helvetica]
 * Channel         [shape=box]
 * ChannelConfig   [shape=box URL="\ref ChannelConfig"]
 * ChannelPipeline [shape=box URL="\ref ChannelPipeline"]
 * Channel -> ChannelConfig [arrowtail=diamond]
 * Channel -> ChannelPipeline [arrowtail=diamond]
 * }
 * @enddot
 *
 */

class Channel : public boost::enable_shared_from_this<Channel> {
public:
    typedef ChannelPipeline::Initializer PipelineInitializer;

public:
    virtual ~Channel();

    /**
     * Returns the unique integer ID of this channel.
     */
    int id() const;

    /**
     * Return the {@link EventLoop} this {@link Channel} was registered too.
     */
    const EventLoopPtr& eventLoop() const;

    /**
     * Returns the parent of this channel.
     *
     * @return the parent channel.
     *         empty {@link ChanelPtr} if this channel does not have a parent channel.
     */
    const ChannelPtr& parent() const;

    /**
     * Returns the configuration of this channel.
     */
    ChannelConfig& config();

    /**
     * Returns the const configuration reference of this channel.
     */
    const ChannelConfig& config() const;

    /**
     * Returns the {@link ChannelPipeline} which handles {@link ChannelEvent}s
     * associated with this channel.
     */
    ChannelPipeline& pipeline();

    /**
     * Returns {@code true} if the {@link Channel} is open and may get active later
     */
    bool isOpen() const;

    /**
     * Return {@code true} if the {@link Channel} is active and so connected.
     */
    bool isActive() const;

    /**
     * Returns the local address where this channel is bound to.
     *
     * @return the local address of this channel.
     * @remark Return an {@link InetAddress NULL_ADDRESS}
     * if this channel is not bound.
     */
    const InetAddress& localAddress() const;

    /**
     * Returns the remote address where this channel is connected to.
     *
     * @return the remote address of this channel.
     * @remark An empty {@link InetAddress NULL_ADDRESS}
     *         if this channel is not connected.
     *         If this channel is not connected but it can receive messages
     *         from arbitrary remote addresses (e.g. {@link DatagramChannel},
     *         use {@link MessageEvent#getRemoteAddress()} to determine
     *         the origination of the received message as this method will
     *         return {@link InetAddress NULL_ADDRESS}.
     */
    const InetAddress& remoteAddress() const;

    /**
     * Returns the Initializer.
     */
    const PipelineInitializer& initializer() const;

    /**
     * Set the {@link Initializer} to the channel, take action before the channel open.
     */
    void setInitializer(const PipelineInitializer& initializer);

    /**
     * Return a new {@link ChannelFuture}.
     */
    ChannelFuturePtr newFuture();

    /**
     * Return a {@link VoidChannelFuture}. This method always return the same instance.
     */
    ChannelFuturePtr newVoidFuture();

    /**
     * Create a new {@link ChannelFuture} which is marked as failed already. So {@link Future#isSuccess()}
     * will return {@code false}. All {@link FutureListener} added to it will be notified directly. Also
     * every call of blocking methods will just return without blocking.
     */
    ChannelFuturePtr newFailedFuture(const Exception& e);

    /**
     * Create a new {@link ChannelFuture} which is marked as successes already. So {@link ChannelFuture#isSuccess()}
     * will return {@code true}. All {@link FutureListener} added to it will be notified directly. Also
     * every call of blocking methods will just return without blocking.
     */
    ChannelFuturePtr newSucceededFuture();

    /**
     * Returns the {@link ChannelFuture} which will be notified when this
     * channel is closed.  This method always returns the same future instance.
     */
    const ChannelFuturePtr& closeFuture();

public:
    /**
     * Open the {@link Channel}, do the initialize with the {@link Initializer}
     * and then do call {@link #doPreOpen}, then fire ChannelOpen to the {@link ChannelPipeline}.
     *
     * If the {@link Channel} once close before, will only not reinitialize with the {@link Initializer}.
     */
    void open();

    /**
     * Request to bind to the given {@link SocketAddress} and notify the {@link ChannelFuture} once the operation
     * completes, either because the operation was successful or because of an error.
     * <p>
     * This will result in having the
     * {@link ChannelOperationHandler#bind(ChannelHandlerContext, SocketAddress, ChannelPromise)} method
     * called of the next {@link ChannelOperationHandler} contained in the  {@link ChannelPipeline} of the
     * {@link Channel}.
     */
    ChannelFuturePtr bind(const InetAddress& localAddress);

    /**
     * Request to connect to the given {@link InetAddress} and notify the {@link ChannelFuture} once the operation
     * completes, either because the operation was successful or because of an error.
     * <p>
     * If the connection fails because of a connection timeout, the {@link ChannelFuture} will get failed with
     * a {@link ConnectTimeoutException}. If it fails because of connection refused a {@link ConnectException}
     * will be used.
     * <p>
     * This will result in having the
     * {@link ChannelOperationHandler#connect(ChannelHandlerContext, SocketAddress, SocketAddress, ChannelPromise)}
     * method called of the next {@link ChannelOperationHandler} contained in the  {@link ChannelPipeline} of the
     * {@link Channel}.
     */
    ChannelFuturePtr connect(const InetAddress& remoteAddress);

    /**
     * Request to connect to the given {@link SocketAddress} while bind to the localAddress and notify the
     * {@link ChannelFuture} once the operation completes, either because the operation was successful or because of
     * an error.
     * <p>
     * This will result in having the
     * {@link ChannelOperationHandler#connect(ChannelHandlerContext, SocketAddress, SocketAddress, ChannelPromise)}
     * method called of the next {@link ChannelOperationHandler} contained in the  {@link ChannelPipeline} of the
     * {@link Channel}.
     */
    ChannelFuturePtr connect(const InetAddress& remoteAddress,
                             const InetAddress& localAddress);

    /**
     * Request to disconnect from the remote peer and notify the {@link ChannelFuture} once the operation completes,
     * either because the operation was successful or because of an error.
     * <p>
     * This will result in having the
     * {@link ChannelHandlerContext::DisconnectFunctor}
     * callback called of the next {@link ChannelOperationHandler} contained in the  {@link ChannelPipeline} of the
     * {@link Channel}.
     */
    ChannelFuturePtr disconnect();

    /**
     * Request to close this ChannelOutboundInvoker and notify the {@link ChannelFuture} once the operation completes,
     * either because the operation was successful or because of
     * an error.
     *
     * After it is closed it is not possible to reuse it again.
     * <p>
     * This will result in having the
     * {@link ChannelOperationHandler#close(ChannelHandlerContext, ChannelPromise)}
     * method called of the next {@link ChannelOperationHandler} contained in the  {@link ChannelPipeline} of the
     * {@link Channel}.
     */
    ChannelFuturePtr close();

    /**
     * Request to flush all pending data which belongs to this ChannelOutboundInvoker and notify the
     * {@link ChannelFuture} once the operation completes, either because the operation was successful or because of
     * an error.
     * <p>
     * Be aware that the flush could be only partially successful. In such cases the {@link ChannelFuture} will be
     * failed with an {@link IncompleteFlushException}. So if you are interested to know if it was partial successful
     * you need to check if the returned {@link ChannelFuture#cause()} returns an instance of
     * {@link IncompleteFlushException}. In such cases you may want to call {@link #flush(ChannelPromise)} or
     * {@link #flush()} to flush the rest of the data or just close the connection via {@link #close(ChannelPromise)} or
     * {@link #close()}  if it is not possible to recover.
     * <p>
     * This will result in having the
     * {@link ChannelOperationHandler#flush(ChannelHandlerContext, ChannelPromise)}
     * method called of the next {@link ChannelOperationHandler} contained in the  {@link ChannelPipeline} of the
     * {@link Channel}.
     */
    ChannelFuturePtr flush();

    /**
     * Request to bind to the given {@link SocketAddress} and notify the {@link ChannelFuture} once the operation
     * completes, either because the operation was successful or because of an error.
     *
     * The given {@link ChannelPromise} will be notified.
     * <p>
     * This will result in having the
     * {@link ChannelOperationHandler#bind(ChannelHandlerContext, SocketAddress, ChannelPromise)} method
     * called of the next {@link ChannelOperationHandler} contained in the  {@link ChannelPipeline} of the
     * {@link Channel}.
     */
    const ChannelFuturePtr& bind(const InetAddress& localAddress,
                                 const ChannelFuturePtr& future);

    /**
     * Request to connect to the given {@link SocketAddress} and notify the {@link ChannelFuture} once the operation
     * completes, either because the operation was successful or because of an error.
     *
     * The given {@link ChannelFuture} will be notified.
     *
     * <p>
     * If the connection fails because of a connection timeout, the {@link ChannelFuture} will get failed with
     * a {@link ConnectTimeoutException}. If it fails because of connection refused a {@link ConnectException}
     * will be used.
     * <p>
     * This will result in having the
     * {@link ChannelOperationHandler#connect(ChannelHandlerContext, SocketAddress, SocketAddress, ChannelPromise)}
     * method called of the next {@link ChannelOperationHandler} contained in the  {@link ChannelPipeline} of the
     * {@link Channel}.
     */
    const ChannelFuturePtr& connect(const InetAddress& remoteAddress,
                                    const ChannelFuturePtr& future);

    /**
     * Request to connect to the given {@link SocketAddress} while bind to the localAddress and notify the
     * {@link ChannelFuture} once the operation completes, either because the operation was successful or because of
     * an error.
     *
     * The given {@link ChannelPromise} will be notified and also returned.
     * <p>
     * This will result in having the
     * {@link ChannelOperationHandler#connect(ChannelHandlerContext, SocketAddress, SocketAddress, ChannelPromise)}
     * method called of the next {@link ChannelOperationHandler} contained in the  {@link ChannelPipeline} of the
     * {@link Channel}.
     */
    const ChannelFuturePtr& connect(const InetAddress& remoteAddress,
                                    const InetAddress& localAddress,
                                    const ChannelFuturePtr& future);

    /**
     * Request to discconect from the remote peer and notify the {@link ChannelFuture} once the operation completes,
     * either because the operation was successful or because of an error.
     *
     * The given {@link ChannelPromise} will be notified.
     * <p>
     * This will result in having the
     * {@link ChannelOperationHandler#disconnect(ChannelHandlerContext, ChannelPromise)}
     * method called of the next {@link ChannelOperationHandler} contained in the  {@link ChannelPipeline} of the
     * {@link Channel}.
     */
    const ChannelFuturePtr& disconnect(const ChannelFuturePtr& future);

    /**
     * Request to close this ChannelOutboundInvoker and notify the {@link ChannelFuture} once the operation completes,
     * either because the operation was successful or because of
     * an error.
     *
     * After it is closed it is not possible to reuse it again.
     * The given {@link ChannelPromise} will be notified.
     * <p>
     * This will result in having the
     * {@link ChannelOperationHandler#close(ChannelHandlerContext, ChannelPromise)}
     * method called of the next {@link ChannelOperationHandler} contained in the  {@link ChannelPipeline} of the
     * {@link Channel}.
     */
    const ChannelFuturePtr& close(const ChannelFuturePtr& future);

    /**
     * Request to flush all pending data which belongs to this ChannelOutboundInvoker and notify the
     * {@link ChannelFuture} once the operation completes, either because the operation was successful or because of
     * an error.
     * <p>
     * Be aware that the flush could be only partially successful. In such cases the {@link ChannelFuture} will be
     * failed with an {@link IncompleteFlushException}. So if you are interested to know if it was partial successful
     * you need to check if the returned {@link ChannelFuture#cause()} returns an instance of
     * {@link IncompleteFlushException}. In such cases you may want to call {@link #flush(ChannelPromise)} or
     * {@link #flush()} to flush the rest of the data or just close the connection via {@link #close(ChannelPromise)} or
     * {@link #close()}  if it is not possible to recover.
     *
     * The given {@link ChannelPromise} will be notified.
     * <p>
     * This will result in having the
     * {@link ChannelOperationHandler#flush(ChannelHandlerContext, ChannelPromise)}
     * method called of the next {@link ChannelOperationHandler} contained in the  {@link ChannelPipeline} of the
     * {@link Channel}.
     *
     */
    const ChannelFuturePtr& flush(const ChannelFuturePtr& future);

    /**
     * Request to Read data from the {@link Channel} into the first inbound buffer, triggers an
     * {@link ChannelStateHandler#inboundBufferUpdated(ChannelHandlerContext) inboundBufferUpdated} event if data was
     * read, and triggers an
     * {@link ChannelStateHandler#channelReadSuspended(ChannelHandlerContext) channelReadSuspended} event so the
     * handler can decide to continue reading.  If there's a pending read operation already, this method does nothing.
     * <p>
     * This will result in having the
     * {@link ChannelOperationHandler#read(ChannelHandlerContext)}
     * method called of the next {@link ChannelOperationHandler} contained in the  {@link ChannelPipeline} of the
     * {@link Channel}.
     */
    void read();

    template<typename Container>
    Container* outboundMessageContainer() {
        return pipeline_->outboundMessageContainer<Container>();
    }

    template<typename Message>
    bool addOutboundChannelMessage(const Message& message) {
        return pipeline_->addOutboundChannelMessage<Message>(message);
    }

    bool addOutboundChannelBuffer(const ChannelBufferPtr& buffer) {
        return pipeline_->addOutboundChannelBuffer(buffer);
    }

    /**
     * Sends a message to this channel asynchronously.    If this channel was
     * created by a connectionless transport (e.g. {@link DatagramChannel})
     * and is not connected yet, you have to call
     * {@link #write(const ChannelMessage&, const InetAddress&, bool)}
     * instead.  Otherwise, the write request will fail with
     * {@link NotYetConnectedException} and an <tt>'exceptionCaught'</tt> event
     * will be triggered.
     *
     * @param message the message to write
     * @param withFuture indicated whether to return a future or not
     *
     * @return the {@link ChannelFuture ChannelFuturePtr} which will be notified when the
     *         write request succeeds or fails
     *
     */
    /**
     * Request to write a message via this ChannelOutboundInvoker and notify the {@link ChannelFuture}
     * once the operation completes, either because the operation was successful or because of an error.
     *
     * If you want to write a {@link FileRegion} use {@link #sendFile(FileRegion)}.
     * <p>
     * Be aware that the write could be only partially successful as the message may need to get encoded before write it
     * to the remote peer. In such cases the {@link ChannelFuture} will be failed with a
     * {@link IncompleteFlushException}. In such cases you may want to call {@link #flush(ChannelPromise)} or
     * {@link #flush()} to flush the rest of the data or just close the connection via {@link #close(ChannelPromise)}
     * or {@link #close()} if it is not possible to recover.
     *
     * The given {@link ChannelPromise} will be notified.
     * <p>
     * This will result in having the message added to the outbound buffer of the next {@link ChannelOutboundHandler}
     * and the {@link ChannelOperationHandler#flush(ChannelHandlerContext, ChannelPromise)}
     * method called of the next {@link ChannelOperationHandler} contained in the  {@link ChannelPipeline} of the
     * {@link Channel}.
     */
    template<typename T>
    ChannelFuturePtr writeMessage(const T& message);

    template<typename T>
    const ChannelFuturePtr& writeMessage(const T& message,
                                         const ChannelFuturePtr& future);

    ChannelFuturePtr writeBuffer(const ChannelBufferPtr& buffer);

    const ChannelFuturePtr& writeBuffer(const ChannelBufferPtr& buffer,
                                        const ChannelFuturePtr& future);

public:
    /**
     * Compares the {@link #getId() ID} of the two channels.
     */
    int compareTo(const ChannelPtr& c) const;

    /**
     * Returns the {@link std::string} representation of this channel.  The returned
     * string contains the @link #getId() ID}, {@link #getLocalAddress() local address},
     * and {@link #getRemoteAddress() remote address} of this channel for
     * easier identification.
     */
    virtual std::string toString() const;

protected:
    enum OperationResult {
        OP_FAILED   = 0,
        OP_SUCCESS  = 1,
        OP_SUCCESS_ = 3,
    };

protected:
    /**
     * Creates a new instance.
     *
     * @param parent
     *        the parent of this channel. <tt>ChannelPtr()</tt> if there's no parent.
     * @param eventLoop
     *        the sink which will receive downstream events from the pipeline
     */
    Channel(const ChannelPtr& parent,
            const EventLoopPtr& eventLoop);

    /**
     * (Internal use only) Creates a new temporary instance with the specified
     * ID.
     *
     * @param id
     *        the pipeline which is going to be attached to this channel
     * @param parent
     *        the parent of this channel. <tt>NULL</tt> if there's no parent.
     * @param sink
     *        the sink which will receive downstream events from the pipeline
     *        and send upstream events to the pipeline
     */
    Channel(int id,
            const ChannelPtr& parent,
            const EventLoopPtr& eventLoop);

    // do the real job in the channel implement
    virtual bool doBind(const InetAddress& localAddress) = 0;
    virtual bool doDisconnect() = 0;
    virtual bool doClose() = 0;

    /**
     * hook before channel open
     * NOOP by default
     */
    virtual void doPreOpen() {}

    /**
     * hook before channel fireChannelActive
     * NOOP by default
     */
    virtual void doPreFireActive() {}

    /**
     * hook before channel close
     * NOOP by default
     */
    virtual void doPreClose() {}

    /**
     * all 
     */
    void onClosedComplete();

    void closeIfClosed();

    void fireClosedCleanly();

    /**
     * (Internal use only) Set the Channel to active state.
     */
    void setActived();

    /**
     * (Internal use only) Set the local address of the Channel.
     */
    void setLocalAddress(const InetAddress& local);

    /**
     * (Internal use only) Set the remote address of the Channel.
     */
    void setRemoteAddress(const InetAddress& remote);

    template<typename C>
    void registerTo(C& context) {
        context.setBindFunctor(boost::bind(&Channel::doBind,
                                           this,
                                           _1,
                                           _2,
                                           _3));

        context.setDisconnectFunctor(boost::bind(&Channel::doDisconnect,
                                     this,
                                     _1,
                                     _2));

        context.setCloseFunctor(boost::bind(&Channel::doClose,
                                            this,
                                            _1,
                                            _2));
    }

private:
    void allocateId();
    void processFailure(const std::string& message,
                        const ChannelFuturePtr& future);

private:
    /**
     * handle the bind event at the sink of the pipeline as the
     * {@link BindFunctor} of the {@link ChannelMessageHandlerContext}.
     */
    void doBind(ChannelHandlerContext& ctx,
                const InetAddress& localAddress,
                const ChannelFuturePtr& future);

    /**
     * handle the disconnect event at the sink of the pipeline as the
     * {@link DisconnectFunctor} of the {@link ChannelMessageHandlerContext}.
     */
    void doDisconnect(ChannelHandlerContext& ctx,
                      const ChannelFuturePtr& future);

    /**
     * handle the close event at the sink of the pipeline as the
     * {@link CloseFunctor} of the {@link ChannelMessageHandlerContext}.
     */
    void doClose(ChannelHandlerContext& ctx,
                 const ChannelFuturePtr& future);

private:
    enum ChannelState {
        CHANNEL_INIT,
        CHANNEL_OPENED,
        CHANNEL_ACTIVED,
        CHANNEL_INACTIVED
    };

private:
    int id_;
    int state_;

    ChannelPtr parent_;
    EventLoopPtr eventLoop_;

    ChannelConfig config_;
    ChannelPipeline* pipeline_;
    PipelineInitializer initializer_;

    ChannelFuturePtr succeededFuture_;
    ChannelFuturePtr closeFuture_;

    InetAddress localAddress_;
    InetAddress remoteAddress_;

    /** Cache for the string representation of this channel */
    mutable bool strValActive_;
    mutable std::string strVal_;
};

inline
int Channel::id() const {
    return id_;
}

inline
const EventLoopPtr& Channel::eventLoop() const {
    return eventLoop_;
}

inline
const ChannelPtr& Channel::parent() const {
    return parent_;
}

inline
ChannelPipeline& Channel::pipeline() {
    return *pipeline_;
}

inline
ChannelConfig& Channel::config() {
    return config_;
}

inline
const ChannelConfig& Channel::config() const {
    return config_;
}

inline
const InetAddress& Channel::localAddress() const {
    return localAddress_;
}

inline
const InetAddress& Channel::remoteAddress() const {
    return remoteAddress_;
}

inline
void Channel::setLocalAddress(const InetAddress& local) {
    localAddress_ = local;
}

inline
void Channel::setRemoteAddress(const InetAddress& remote) {
    remoteAddress_ = remote;
}

inline
const Channel::PipelineInitializer& Channel::initializer() const {
    return initializer_;
}

inline
void Channel::setInitializer(const Channel::PipelineInitializer& initializer) {
    initializer_ = initializer;
}

inline
bool Channel::isOpen() const {
    return state_ == CHANNEL_OPENED || state_ == CHANNEL_ACTIVED;
}

inline
bool Channel::isActive() const {
    return state_ == CHANNEL_ACTIVED;
}

inline
ChannelFuturePtr Channel::newSucceededFuture() {
    return succeededFuture_;
}

inline
const ChannelFuturePtr& Channel::closeFuture() {
    return closeFuture_;
}

inline
ChannelFuturePtr Channel::bind(const InetAddress& localAddress) {
    localAddress_ = localAddress;
    return pipeline_->bind(localAddress);
}

inline
const ChannelFuturePtr& Channel::bind(const InetAddress& localAddress,
                                      const ChannelFuturePtr& future) {
    localAddress_ = localAddress;
    return pipeline_->bind(localAddress, future);
}

inline
ChannelFuturePtr Channel::connect(const InetAddress& remoteAddress) {
    remoteAddress_ = remoteAddress;
    return pipeline_->connect(remoteAddress);
}

inline
ChannelFuturePtr Channel::connect(const InetAddress& remoteAddress,
                                  const InetAddress& localAddress) {
    remoteAddress_ = remoteAddress;
    return pipeline_->connect(remoteAddress, localAddress);
}

inline
const ChannelFuturePtr& Channel::connect(const InetAddress& remoteAddress,
        const ChannelFuturePtr& future) {
    remoteAddress_ = remoteAddress;
    return pipeline_->connect(remoteAddress, future);
}

inline
const ChannelFuturePtr& Channel::connect(const InetAddress& remoteAddress,
        const InetAddress& localAddress,
        const ChannelFuturePtr& future) {
    remoteAddress_ = remoteAddress;
    return pipeline_->connect(remoteAddress, localAddress, future);
}

inline
ChannelFuturePtr Channel::disconnect() {
    return pipeline_->disconnect();
}

inline
const ChannelFuturePtr& Channel::disconnect(const ChannelFuturePtr& future) {
    return pipeline_->disconnect(future);
}

inline
ChannelFuturePtr Channel::flush() {
    return pipeline_->flush();
}

inline
const ChannelFuturePtr& Channel::flush(const ChannelFuturePtr& future) {
    return pipeline_->flush(future);
}

inline
void Channel::read() {
    pipeline_->read();
}

template<typename T> inline
ChannelFuturePtr Channel::writeMessage(const T& message) {
    if (isActive()) {
        return pipeline_->writeMessage(message);
    }
    else {
        return newFailedFuture(ChannelException("write message before channel is Active."));
    }
}


template<typename T> inline
const ChannelFuturePtr& Channel::writeMessage(const T& message,
        const ChannelFuturePtr& future) {
    if (isActive()) {
        return pipeline_->writeMessage(message, future);
    }
    else {
        ChannelException e("channel is not active, do nothing with the write.");
        future->setFailure(e);
    }

    return future;
}

inline
ChannelFuturePtr Channel::writeBuffer(const ChannelBufferPtr& buffer) {
    if (isActive()) {
        return pipeline_->writeBuffer(buffer);
    }
    else {
        return newFailedFuture(ChannelException("write message before channel is Active."));
    }
}

inline
const ChannelFuturePtr& Channel::writeBuffer(const ChannelBufferPtr& buffer,
        const ChannelFuturePtr& future) {
    if (isActive()) {
        return pipeline_->writeBuffer(buffer, future);
    }
    else {
        ChannelException e("channel is not active, do nothing with the write.");
        future->setFailure(e);
    }

    return future;
}

}
}

#endif //#if !defined(CETTY_CHANNEL_CHANNEL_H)

// Local Variables:
// mode: c++
// End:
