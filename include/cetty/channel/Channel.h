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
#include <cetty/channel/ChannelFwd.h>
#include <cetty/channel/ChannelFutureFwd.h>
#include <cetty/channel/ChannelFactoryFwd.h>
#include <cetty/channel/ChannelPipelineFwd.h>

namespace cetty {
namespace channel {

class SocketAddress;
class ChannelConfig;
class ChannelMessage;

/**
 * A nexus to a network socket or a component which is capable of I/O
 * operations such as read, write, connect, and bind.
 * <p>
 * A channel provides a user:
 * <ul>
 * <li>the current state of the channel (e.g. is it open? is it connected?),</li>
 * <li>the {@link ChannelConfig configuration parameters} of the channel (e.g. receive buffer size),</li>
 * <li>the I/O operations that the channel supports (e.g. read, write, connect, and bind), and</li>
 * <li>the {@link ChannelPipeline} which handles all {@link ChannelEvent I/O events and requests} associated with the channel.</li>
 * </ul>
 *
 * <h3>All I/O operations are asynchronous.</h3>
 * <p>
 * All I/O operations in Cetty are asynchronous.  It means any I/O calls will
 * return immediately with no guarantee that the requested I/O operation has
 * been completed at the end of the call.  Instead, you will be returned with
 * a {@link ChannelFuture} instance which will notify you when the requested I/O
 * operation has succeeded, failed, or canceled.
 *
 * <h3>Channels are hierarchical</h3>
 * <p>
 * A {@link Channel} can have a @link #getParent() parent@endlink depending on
 * how it was created.  For instance, a {@link SocketChannel}, that was accepted
 * by {@link ServerSocketChannel}, will return the {@link ServerSocketChannel}
 * as its parent on {@link #getParent()}.
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
 * operations.  For example, with the I/O datagram transport, multicast
 * join / leave operations are provided by {@link DatagramChannel}.
 *
 * <h3>InterestOps</h3>
 * <p>
 * A {@link Channel} has a property called {@link #getInterestOps() interestOps}.
 * It is represented as a <a href="http://en.wikipedia.org/wiki/Bit_field">bit
 * field</a> which is composed of the two flags.
 * <ul>
 * <li>{@link #OP_READ} - If set, a message sent by a remote peer will be read
 *     immediately.  If unset, the message from the remote peer will not be read
 *     until the {@link #OP_READ} flag is set again (i.e. read suspension).</li>
 * <li>{@link #OP_WRITE} - If set, a write request will not be sent to a remote
 *     peer until the {@link #OP_WRITE} flag is cleared and the write request
 *     will be pending in a queue.  If unset, the write request will be flushed
 *     out as soon as possible from the queue.</li>
 * <li>{@link #OP_READ_WRITE} - This is a combination of {@link #OP_READ} and
 *     {@link #OP_WRITE}, which means only write requests are suspended.</li>
 * <li>{@link #OP_NONE} - This is a combination of (NOT {@link #OP_READ}) and
 *     (NOT {@link #OP_WRITE}), which means only read operation is suspended.</li>
 * </ul>
 * </p><p>
 * You can set or clear the {@link #OP_READ} flag to suspend and resume read
 * operation via {@link #setReadable(bool)}.
 * </p><p>
 * Please note that you cannot suspend or resume write operation just like you
 * can set or clear {@link #OP_READ}. The {@link #OP_WRITE} flag is read only
 * and provided simply as a mean to tell you if the size of pending write
 * requests exceeded a certain threshold or not so that you don't issue too many
 * pending writes. For example, the asio implimented socket transport uses the
 * <tt>writeBufferLowWaterMark</tt> and <tt>writeBufferHighWaterMark</tt>
 * properties in {@link AsioSocketChannelConfig} to determine when to set or
 * clear the {@link #OP_WRITE} flag.
 * </p>
 *
 *
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 * @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
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
class Channel {
public:
    virtual ~Channel() {}

    /**
     * The {@link #getInterestOps() interestOps} value which tells that only
     * read operation has been suspended.
     */
    static const int OP_NONE = 0;

    /**
     * The {@link #getInterestOps() interestOps} value which tells that neither
     * read nor write operation has been suspended.
     */
    static const int OP_READ = 1;

    /**
     * The {@link #getInterestOps() interestOps} value which tells that both
     * read and write operation has been suspended.
     */
    static const int OP_WRITE = 4;

    /**
     * The {@link #getInterestOps() interestOps} value which tells that only
     * write operation has been suspended.
     */
    static const int OP_READ_WRITE = OP_READ | OP_WRITE;

    static const char* getInterestOpsString(int ops) {
        switch (ops) {
        case OP_READ: return "OP_READ";
        case OP_WRITE: return "OP_WRITE";
        case OP_READ_WRITE: return "OP_READ_WRITE";
        case OP_NONE: return "OP_NONE";
        default: return "OP_UNKNOWN";
        }
    }

    /**
     * Returns the unique integer ID of this channel.
     */
    virtual int getId() const = 0;

    /**
     * Returns the identity hash code of this channel.
     */
    virtual int hashCode() const = 0;

    /**
     * Returns the parent of this channel.
     *
     * @return the parent channel.
     *         <tt>NULL</tt> if this channel does not have a parent channel.
     */
    virtual const ChannelPtr& getParent() const = 0;

    /**
     * Returns the {@link ChannelFactory} which created this channel.
     */
    virtual const ChannelFactoryPtr& getFactory() const = 0;

    /**
     * Returns the configuration of this channel.
     */
    virtual ChannelConfig& getConfig() = 0;

    /**
     * Returns the const configuration reference of this channel.
     */
    virtual const ChannelConfig& getConfig() const = 0;

    /**
     * Returns the {@link ChannelPipeline} which handles {@link ChannelEvent}s
     * associated with this channel.
     */
    virtual const ChannelPipelinePtr& getPipeline() const  = 0;

    /**
     * Returns <tt>true</tt> if and only if this channel is open.
     */
    virtual bool isOpen() const = 0;

    /**
     * Returns <tt>true</tt> if and only if this channel is bound to a
     * {@link #getLocalAddress() local address}.
     */
    virtual bool isBound() const = 0;

    /**
     * Returns <tt>true</tt> if and only if this channel is connected to a
     * {@link #getRemoteAddress() remote address}.
     */
    virtual bool isConnected() const = 0;

    /**
     * Returns the local address where this channel is bound to.
     *
     * @return the local address of this channel.
     * @remark Return an {@link SocketAddress NULL_ADDRESS}
     * if this channel is not bound.
     */
    virtual const SocketAddress& getLocalAddress() const = 0;

    /**
     * Returns the remote address where this channel is connected to.
     *
     * @return the remote address of this channel.
     * @remark An empty {@link SocketAddress NULL_ADDRESS}
     *         if this channel is not connected.
     *         If this channel is not connected but it can receive messages
     *         from arbitrary remote addresses (e.g. {@link DatagramChannel},
     *         use {@link MessageEvent#getRemoteAddress()} to determine
     *         the origination of the received message as this method will
     *         return {@link SocketAddress NULL_ADDRESS}.
     */
    virtual const SocketAddress& getRemoteAddress() const = 0;

    /**
     * Sends a message to this channel asynchronously.    If this channel was
     * created by a connectionless transport (e.g. {@link DatagramChannel})
     * and is not connected yet, you have to call
     * {@link #write(const ChannelMessage&, const SocketAddress&, bool)}
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
    virtual ChannelFuturePtr write(const ChannelMessage& message) = 0;

    //virtual void write(const ChannelMessage& message,
    //                    ChannelFuturePtr* future) = 0;

    /**
     * Sends a message to this channel asynchronously.  It has an additional
     * parameter that allows a user to specify where to send the specified
     * message instead of this channel's current remote address.  If this
     * channel was created by a connectionless transport (e.g. {@link DatagramChannel})
     * and is not connected yet, you must specify NOT NULL_ADDRESS.  Otherwise,
     * the write request will fail with {@link NotYetConnectedException} and
     * an <tt>'exceptionCaught'</tt> event will be triggered.
     *
     * @param message       the message to write
     * @param remoteAddress where to send the specified message.
     *                      This method is identical to
     *                      {@link #write(const ChannelMessage&, bool)}
     *                      if <tt>NULL_ADDRESS</tt> is specified here.
     * @param withFurture indicated wether to return a future or not.
     *
     * @return the {@link ChannelFuture ChannelFuturePtr} which will be notified when the
     *         write request succeeds or fails
     *
     */
    virtual ChannelFuturePtr write(const ChannelMessage& message,
                                   const SocketAddress& remoteAddress) = 0;

    //virtual void write(const ChannelMessage& message,
    //                    const SocketAddress& remoteAddress,
    //                    ChannelFuturePtr* future) = 0;

    /**
     * Binds this channel to the specified local address asynchronously.
     *
     * @param localAddress where to bind
     *
     * @return the {@link ChannelFuture ChannelFuturePtr} which will be
     *         notified when the bind request succeeds or fails
     *
     */
    virtual ChannelFuturePtr bind(const SocketAddress& localAddress) = 0;

    /**
     * Connects this channel to the specified remote address asynchronously.
     *
     * @param remoteAddress where to connect
     *
     * @return the {@link ChannelFuture ChannelFuturePtr} which will be
     *         notified when the connection request succeeds or fails
     *
     */
    virtual ChannelFuturePtr connect(const SocketAddress& remoteAddress) = 0;

    /**
     * Disconnects this channel from the current remote address asynchronously.
     *
     * @return the {@link ChannelFuture ChannelFuturePtr} which will be notified when the
     *         disconnection request succeeds or fails
     */
    virtual ChannelFuturePtr disconnect() = 0;

    /**
     * Unbinds this channel from the current local address asynchronously.
     *
     * @return the {@link ChannelFuture ChannelFuturePtr} which will be notified when the
     *         unbind request succeeds or fails
     */
    virtual ChannelFuturePtr unbind() = 0;

    /**
     * Closes this channel asynchronously.  If this channel is bound or
     * connected, it will be disconnected and unbound first.  Once a channel
     * is closed, it can not be open again.  Calling this method on a closed
     * channel has no effect.  Please note that this method always returns the
     * same future instance.
     *
     * @return the {@link ChannelFuture  ChannelFuturePtr} which will be notified when the
     *         close request succeeds or fails
     */
    virtual ChannelFuturePtr close() = 0;

    /**
     * Returns the {@link ChannelFuture  ChannelFuturePtr} which will be notified when this
     * channel is closed.  This method always returns the same future instance.
     */
    virtual const ChannelFuturePtr&  getCloseFuture() = 0;

    /**
     * Returns the {@link ChannelFuture  ChannelFuturePtr} which is already succeeded.
     * This method always returns the same future instance, which is cached
     * for easy use.
     */
    virtual const ChannelFuturePtr& getSucceededFuture() = 0;

    /**
     * Returns the current <tt>interestOps</tt> of this channel.
     *
     * @return {@link #OP_NONE}, {@link #OP_READ}, {@link #OP_WRITE}, or
     *         {@link #OP_READ_WRITE}
     */
    virtual int getInterestOps() const = 0;

    /**
     * Returns <tt>true</tt> if and only if the I/O thread will read a message
     * from this channel.  This method is a shortcut to the following code:
     * <pre>
     * return (getInterestOps() & OP_READ) != 0;
     * </pre>
     */
    virtual bool isReadable() const = 0;

    /**
     * Returns <tt>true</tt> if and only if the I/O thread will perform the
     * requested write operation immediately.  Any write requests made when
     * this method returns <tt>false</tt> are queued until the I/O thread is
     * ready to process the queued write requests.  This method is a shortcut
     * to the following code:
     * <pre>
     * return (getInterestOps() & OP_WRITE) == 0;
     * </pre>
     */
    virtual bool isWritable() const = 0;

    /**
     * Changes the <tt>interestOps</tt> of this channel asynchronously.
     *
     * @param interestOps the new <tt>interestOps</tt>
     *
     * @return the {@link ChannelFuture ChannelFuturePtr} which will be notified when the
     *         <tt>interestOps</tt> change request succeeds or fails
     */
    virtual ChannelFuturePtr setInterestOps(int interestOps) = 0;

    /**
     * Suspends or resumes the read operation of the I/O thread asynchronously.
     * This method is a shortcut to the following code:
     * <pre>
     * int interestOps = getInterestOps();
     * if (readable) {
     *     setInterestOps(interestOps | OP_READ);
     * }
     * else {
     *     setInterestOps(interestOps & ~OP_READ);
     * }
     * </pre>
     *
     * @param readable <tt>true</tt> to resume the read operation and
     *                 <tt>false</tt> to suspend the read operation
     *
     * @return the {@link ChannelFuture ChannelFuturePtr} which will be notified when the
     *         <tt>interestOps</tt> change request succeeds or fails
     */
    virtual ChannelFuturePtr setReadable(bool readable) = 0;

    /**
     * Compares the @link #getId() ID@endlink of the two channels.
     */
    virtual int compareTo(const ChannelPtr& c) const = 0;

    /**
     * Returns a string representing this Channel
     *
     * @return  a string representation of this Channel.
     */
    virtual std::string toString() const = 0;
};

}
}

#endif //#if !defined(CETTY_CHANNEL_CHANNEL_H)

// Local Variables:
// mode: c++
// End:
