#if !defined(CETTY_CHANNEL_CHANNELS_H)
#define CETTY_CHANNEL_CHANNELS_H

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

#include <vector>
#include <cetty/channel/ChannelFuture.h>
#include <cetty/channel/ChannelHandler.h>
#include <cetty/channel/ChannelPipelineFactory.h>

namespace cetty {
namespace util {
class Exception;
}
}

namespace cetty {
namespace channel {

using namespace cetty::util;

class Channel;
class ChannelPipeline;
class ChannelHandler;
class ChannelHandlerContext;
class ChannelFuture;
class ChannelMessage;

class SocketAddress;

/**
 * A helper class which provides various convenience methods related with
 * {@link Channel}, {@link ChannelHandler}, and {@link ChannelPipeline}.
 *
 * <h3>Factory methods</h3>
 * <p>
 * It is always recommended to use the factory methods provided by
 * {@link Channels} rather than calling the constructor of the implementation
 * types.
 * <ul>
 * <li>{@link #pipeline()}</li>
 * <li>{@link #pipeline(ChannelPipeline)}</li>
 * <li>{@link #pipelineFactory(ChannelPipeline)}</li>
 * <li>{@link #succeededFuture(Channel)}</li>
 * <li>{@link #failedFuture(Channel, Throwable)}</li>
 * </ul>
 *
 * <h3>Upstream and downstream event generation</h3>
 * <p>
 * Various event generation methods are provided to simplify the generation of
 * upstream events and downstream events.  It is always recommended to use the
 * event generation methods provided by {@link Channels} rather than calling
 * {@link ChannelHandlerContext#sendUpstream(ChannelEvent)} or
 * {@link ChannelHandlerContext#sendDownstream(ChannelEvent)} by yourself.
 *
 *
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 *
 * @apiviz.landmark
 *
 * @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
 */

class Channels {
public:
    /**
     * Creates a new {@link ChannelPipeline}.
     */
    static ChannelPipelinePtr pipeline();

    /**
     * Creates a new {@link ChannelPipeline} which contains the specified
     * {@link ChannelHandler}s.  The names of the specified handlers are
     * generated automatically; the first handler's name is <tt>"0"</tt>,
     * the second handler's name is <tt>"1"</tt>, the third handler's name is
     * <tt>"2"</tt>, and so on.
     */
    static ChannelPipelinePtr pipeline(const ChannelHandlerPtr& handler);

    static ChannelPipelinePtr pipeline(const ChannelHandlerPtr& handler0,
                                     const ChannelHandlerPtr& handler1);

    static ChannelPipelinePtr pipeline(const ChannelHandlerPtr& handler0,
                                     const ChannelHandlerPtr& handler1,
                                     const ChannelHandlerPtr& handler2);

    static ChannelPipelinePtr pipeline(const ChannelHandlerPtr& handler0,
                                     const ChannelHandlerPtr& handler1,
                                     const ChannelHandlerPtr& handler2,
                                     const ChannelHandlerPtr& handler3);

    static ChannelPipelinePtr pipeline(const std::vector<ChannelHandlerPtr>& handlers);

    /**
     * Creates a new {@link ChannelPipeline} which contains the same entries
     * with the specified <tt>pipeline</tt>.  Please note that only the names
     * and the references of the {@link ChannelHandler}s will be copied; a new
     * {@link ChannelHandler} instance will never be created.
     */
    static ChannelPipelinePtr pipeline(const ChannelPipelinePtr& pipeline);

    /**
     * Creates a new {@link ChannelPipelineFactory} which creates a new
     * {@link ChannelPipeline} which contains the same entries with the
     * specified <tt>pipeline</tt>.  Please note that only the names and the
     * references of the {@link ChannelHandler}s will be copied; a new
     * {@link ChannelHandler} instance will never be created.
     */
    static ChannelPipelineFactoryPtr pipelineFactory(const ChannelPipelinePtr& pipeline);

    // future factory methods

    /**
     * Creates a new non-cancellable {@link ChannelFuture} for the specified
     * {@link Channel}.
     */
    static ChannelFuturePtr future(const ChannelPtr& channel) {
        return future(channel, false);
    }

    /**
     * Creates a new {@link ChannelFuture} for the specified {@link Channel}.
     *
     * @param cancellable <tt>true</tt> if and only if the returned future
     *                    can be canceled by {@link ChannelFuture#cancel()}
     */
    static ChannelFuturePtr future(const ChannelPtr& channel, bool cancellable);

    /**
     * Creates a new {@link ChannelFuture} which has failed already for the
     * specified {@link Channel}.
     *
     * @param cause the cause of the failure
     */
    static ChannelFuturePtr failedFuture(const ChannelPtr& channel, const Exception& cause);

    // event emission methods

    /**
     * Sends a <tt>"channelOpen"</tt> event to the first
     * {@link ChannelUpstreamHandler} in the {@link ChannelPipeline} of
     * the specified {@link Channel}.  If the specified channel has a parent,
     * a <tt>"childChannelOpen"</tt> event will be sent, too.
     */
    static void fireChannelOpen(const ChannelPtr& channel);

    /**
     * Sends a <tt>"channelOpen"</tt> event to the
     * {@link ChannelUpstreamHandler} which is placed in the closest upstream
     * from the handler associated with the specified
     * {@link ChannelHandlerContext}.
     * <p>
     * Please note that this method does not trigger a
     * <tt>"childChannelOpen"</tt> event unlike {@link #fireChannelOpen(Channel)}
     * method.
     */
    static void fireChannelOpen(ChannelHandlerContext& ctx);

    /**
     * Sends a <tt>"channelBound"</tt> event to the first
     * {@link ChannelUpstreamHandler} in the {@link ChannelPipeline} of
     * the specified {@link Channel}.
     *
     * @param localAddress
     *        the local address where the specified channel is bound
     */
    static void fireChannelBound(const ChannelPtr& channel, const SocketAddress& localAddress);

    /**
     * Sends a <tt>"channelBound"</tt> event to the
     * {@link ChannelUpstreamHandler} which is placed in the closest upstream
     * from the handler associated with the specified
     * {@link ChannelHandlerContext}.
     *
     * @param localAddress
     *        the local address where the specified channel is bound
     */
    static void fireChannelBound(ChannelHandlerContext& ctx, const SocketAddress& localAddress);

    /**
     * Sends a <tt>"channelConnected"</tt> event to the first
     * {@link ChannelUpstreamHandler} in the {@link ChannelPipeline} of
     * the specified {@link Channel}.
     *
     * @param remoteAddress
     *        the remote address where the specified channel is connected
     */
    static void fireChannelConnected(const ChannelPtr& channel, const SocketAddress& remoteAddress);

    /**
     * Sends a <tt>"channelConnected"</tt> event to the
     * {@link ChannelUpstreamHandler} which is placed in the closest upstream
     * from the handler associated with the specified
     * {@link ChannelHandlerContext&}.
     *
     * @param remoteAddress
     *        the remote address where the specified channel is connected
     */
    static void fireChannelConnected(ChannelHandlerContext& ctx, const SocketAddress& remoteAddress);

    /**
     * Sends a <tt>"messageReceived"</tt> event to the first
     * {@link ChannelUpstreamHandler} in the {@link ChannelPipeline} of
     * the specified {@link Channel}.
     *
     * @param message  the received message
     */
    static void fireMessageReceived(const ChannelPtr& channel, const ChannelMessage& message);

    /**
     * Sends a <tt>"messageReceived"</tt> event to the first
     * {@link ChannelUpstreamHandler} in the {@link ChannelPipeline} of
     * the specified {@link Channel} belongs.
     *
     * @param message        the received message
     * @param remoteAddress  the remote address where the received message
     *                       came from
     */
    static void fireMessageReceived(const ChannelPtr& channel, const ChannelMessage& message, const SocketAddress& remoteAddress);

    /**
     * Sends a <tt>"messageReceived"</tt> event to the
     * {@link ChannelUpstreamHandler} which is placed in the closest upstream
     * from the handler associated with the specified
     * {@link ChannelHandlerContext}.
     *
     * @param message  the received message
     */
    static void fireMessageReceived(ChannelHandlerContext& ctx, const ChannelMessage& message);

    /**
     * Sends a <tt>"messageReceived"</tt> event to the
     * {@link ChannelUpstreamHandler} which is placed in the closest upstream
     * from the handler associated with the specified
     * {@link ChannelHandlerContext}.
     *
     * @param message        the received message
     * @param remoteAddress  the remote address where the received message
     *                       came from
     */
    static void fireMessageReceived(ChannelHandlerContext& ctx, const ChannelMessage& message, const SocketAddress& remoteAddress);

    /**
     * Sends a <tt>"writeCompleted"</tt> event to the first
     * {@link ChannelUpstreamHandler} in the {@link ChannelPipeline} of
     * the specified {@link Channel}.
     */
    static void fireWriteCompleted(const ChannelPtr& channel, long amount);

    /**
     * Sends a <tt>"writeCompleted"</tt> event to the
     * {@link ChannelUpstreamHandler} which is placed in the closest upstream
     * from the handler associated with the specified
     * {@link ChannelHandlerContext}.
     */
    static void fireWriteCompleted(ChannelHandlerContext& ctx, long amount);

    /**
     * Sends a <tt>"channelInterestChanged"</tt> event to the first
     * {@link ChannelUpstreamHandler} in the {@link ChannelPipeline} of
     * the specified {@link Channel}.
     */
    static void fireChannelInterestChanged(const ChannelPtr& channel, int interestOps);

    /**
     * Sends a <tt>"channelInterestChanged"</tt> event to the
     * {@link ChannelUpstreamHandler} which is placed in the closest upstream
     * from the handler associated with the specified
     * {@link ChannelHandlerContext}.
     */
    static void fireChannelInterestChanged(ChannelHandlerContext& ctx, int interestOps);

    /**
     * Sends a <tt>"channelDisconnected"</tt> event to the first
     * {@link ChannelUpstreamHandler} in the {@link ChannelPipeline} of
     * the specified {@link Channel}.
     */
    static void fireChannelDisconnected(const ChannelPtr& channel);

    /**
     * Sends a <tt>"channelDisconnected"</tt> event to the
     * {@link ChannelUpstreamHandler} which is placed in the closest upstream
     * from the handler associated with the specified
     * {@link ChannelHandlerContext}.
     */
    static void fireChannelDisconnected(ChannelHandlerContext& ctx);

    /**
     * Sends a <tt>"channelUnbound"</tt> event to the first
     * {@link ChannelUpstreamHandler} in the {@link ChannelPipeline} of
     * the specified {@link Channel}.
     */
    static void fireChannelUnbound(const ChannelPtr& channel);

    /**
     * Sends a <tt>"channelUnbound"</tt> event to the
     * {@link ChannelUpstreamHandler} which is placed in the closest upstream
     * from the handler associated with the specified
     * {@link ChannelHandlerContext}.
     */
    static void fireChannelUnbound(ChannelHandlerContext& ctx);

    /**
     * Sends a <tt>"channelClosed"</tt> event to the first
     * {@link ChannelUpstreamHandler} in the {@link ChannelPipeline} of
     * the specified {@link Channel}.
     */
    static void fireChannelClosed(const ChannelPtr& channel);

    /**
     * Sends a <tt>"channelClosed"</tt> event to the
     * {@link ChannelUpstreamHandler} which is placed in the closest upstream
     * from the handler associated with the specified
     * {@link ChannelHandlerContext}.
     */
    static void fireChannelClosed(ChannelHandlerContext& ctx);

    /**
     * Sends a <tt>"exceptionCaught"</tt> event to the first
     * {@link ChannelUpstreamHandler} in the {@link ChannelPipeline} of
     * the specified {@link Channel}.
     */
    static void fireExceptionCaught(const ChannelPtr& channel, const Exception& cause);

    /**
     * Sends a <tt>"exceptionCaught"</tt> event to the
     * {@link ChannelUpstreamHandler} which is placed in the closest upstream
     * from the handler associated with the specified
     * {@link ChannelHandlerContext}.
     */
    static void fireExceptionCaught(ChannelHandlerContext& ctx, const Exception& cause);

    /**
     * Sends a <tt>"bind"</tt> request to the last
     * {@link ChannelDownstreamHandler} in the {@link ChannelPipeline} of
     * the specified {@link Channel}.
     *
     * @param channel  the channel to bind
     * @param localAddress  the local address to bind to
     *
     * @return the {@link ChannelFuture} which will be notified when the
     *         bind operation is done
     */
    static ChannelFuturePtr bind(const ChannelPtr& channel,
        const SocketAddress& localAddress);


    /**
     * Sends a <tt>"bind"</tt> request to the
     * {@link ChannelDownstreamHandler} which is placed in the closest
     * downstream from the handler associated with the specified
     * {@link ChannelHandlerContext}.
     *
     * @param ctx     the context
     * @param future  the future which will be notified when the bind
     *                operation is done
     * @param localAddress the local address to bind to
     */
    static void bind(ChannelHandlerContext& ctx,
        const ChannelFuturePtr& future,
        const SocketAddress& localAddress);

    /**
     * Sends a <tt>"unbind"</tt> request to the last
     * {@link ChannelDownstreamHandler} in the {@link ChannelPipeline} of
     * the specified {@link Channel}.
     *
     * @param channel  the channel to unbind
     *
     * @return the {@link ChannelFuture} which will be notified when the
     *         unbind operation is done
     */
    static ChannelFuturePtr unbind(const ChannelPtr& channel);

    /**
     * Sends a <tt>"unbind"</tt> request to the
     * {@link ChannelDownstreamHandler} which is placed in the closest
     * downstream from the handler associated with the specified
     * {@link ChannelHandlerContext}.
     *
     * @param ctx     the context
     * @param future  the future which will be notified when the unbind
     *                operation is done
     */
    static void unbind(ChannelHandlerContext& ctx, const ChannelFuturePtr& future);

    /**
     * Sends a <tt>"connect"</tt> request to the last
     * {@link ChannelDownstreamHandler} in the {@link ChannelPipeline} of
     * the specified {@link Channel}.
     *
     * @param channel  the channel to attempt a connection
     * @param remoteAddress  the remote address to connect to
     *
     * @return the {@link ChannelFuture} which will be notified when the
     *         connection attempt is done
     */
    static ChannelFuturePtr connect(const ChannelPtr& channel,
        const SocketAddress& remoteAddress);

    /**
     * Sends a <tt>"connect"</tt> request to the
     * {@link ChannelDownstreamHandler} which is placed in the closest
     * downstream from the handler associated with the specified
     * {@link ChannelHandlerContext}.
     *
     * @param ctx     the context
     * @param future  the future which will be notified when the connection
     *                attempt is done
     * @param remoteAddress the remote address to connect to
     */
    static void connect(ChannelHandlerContext& ctx,
                        const ChannelFuturePtr& future,
                        const SocketAddress& remoteAddress);

    /**
     * Sends a <tt>"write"</tt> request to the last
     * {@link ChannelDownstreamHandler} in the {@link ChannelPipeline} of
     * the specified {@link Channel}.
     *
     * @param channel  the channel to write a message
     * @param message  the message to write to the channel
     *
     * @return the {@link ChannelFuture} which will be notified when the
     *         write operation is done
     */
    static ChannelFuturePtr write(const ChannelPtr& channel,
                                  const ChannelMessage& message);

    /**
     * Sends a <tt>"write"</tt> request to the
     * {@link ChannelDownstreamHandler} which is placed in the closest
     * downstream from the handler associated with the specified
     * {@link ChannelHandlerContext}.
     *
     * @param ctx     the context
     * @param future  the future which will be notified when the write
     *                operation is done
     */
    static void write(ChannelHandlerContext& ctx,
                      const ChannelFuturePtr& future,
                      const ChannelMessage& message);

    /**
     * Sends a <tt>"write"</tt> request to the last
     * {@link ChannelDownstreamHandler} in the {@link ChannelPipeline} of
     * the specified {@link Channel}.
     *
     * @param channel  the channel to write a message
     * @param message  the message to write to the channel
     * @param remoteAddress  the destination of the message.
     *                       <tt>NULL</tt> to use the default remote address
     *
     * @return the {@link ChannelFuture} which will be notified when the
     *         write operation is done
     */
    static ChannelFuturePtr write(const ChannelPtr& channel,
                                  const ChannelMessage& message,
                                  const SocketAddress& remoteAddress);

    /**
     * Sends a <tt>"write"</tt> request to the
     * {@link ChannelDownstreamHandler} which is placed in the closest
     * downstream from the handler associated with the specified
     * {@link ChannelHandlerContext}.
     *
     * @param ctx     the context
     * @param future  the future which will be notified when the write
     *                operation is done
     * @param message the message to write to the channel
     * @param remoteAddress  the destination of the message.
     *                       <tt>NULL</tt> to use the default remote address.
     */
    static void write(ChannelHandlerContext& ctx,
                      const ChannelFuturePtr& future,
                      const ChannelMessage& message,
                      const SocketAddress& remoteAddress);

    /**
     * Sends a <tt>"setInterestOps"</tt> request to the last
     * {@link ChannelDownstreamHandler} in the {@link ChannelPipeline} of
     * the specified {@link Channel}.
     *
     * @param channel     the channel to change its interestOps
     * @param interestOps the new interestOps
     *
     * @return the {@link ChannelFuture} which will be notified when the
     *         interestOps is changed
     */
    static ChannelFuturePtr setInterestOps(const ChannelPtr& channel, int interestOps);

    /**
     * Sends a <tt>"setInterestOps"</tt> request to the
     * {@link ChannelDownstreamHandler} which is placed in the closest
     * downstream from the handler associated with the specified
     * {@link ChannelHandlerContext}.
     *
     * @param ctx     the context
     * @param future  the future which will be notified when the interestOps is
     *                changed.
     */
    static void setInterestOps(ChannelHandlerContext& ctx,
                               const ChannelFuturePtr& future,
                               int interestOps);

    /**
     * Sends a <tt>"disconnect"</tt> request to the last
     * {@link ChannelDownstreamHandler} in the {@link ChannelPipeline} of
     * the specified {@link Channel}.
     *
     * @param channel  the channel to disconnect
     *
     * @return the {@link ChannelFuture} which will be notified on disconnection
     */
    static ChannelFuturePtr disconnect(const ChannelPtr& channel);

    /**
     * Sends a <tt>"disconnect"</tt> request to the
     * {@link ChannelDownstreamHandler} which is placed in the closest
     * downstream from the handler associated with the specified
     * {@link ChannelHandlerContext}.
     *
     * @param ctx     the context
     * @param future  the future which will be notified on disconnection
     */
    static void disconnect(ChannelHandlerContext& ctx, const ChannelFuturePtr& future);

    /**
     * Sends a <tt>"close"</tt> request to the last
     * {@link ChannelDownstreamHandler} in the {@link ChannelPipeline} of
     * the specified {@link Channel}.
     *
     * @param channel  the channel to close
     *
     * @return the {@link ChannelFuture} which will be notified on closure
     */
    static ChannelFuturePtr close(const ChannelPtr& channel);

    /**
     * Sends a <tt>"close"</tt> request to the
     * {@link ChannelDownstreamHandler} which is placed in the closest
     * downstream from the handler associated with the specified
     * {@link ChannelHandlerContext}.
     *
     * @param ctx     the context
     * @param future  the future which will be notified on closure
     */
    static void close(ChannelHandlerContext& ctx, const ChannelFuturePtr& future);

    static int validateAndFilterDownstreamInteresOps(int interestOps);

private:
    static void fireChildChannelStateChanged(const ChannelPtr& channel,
        const ChannelPtr& childChannel);

    static void validateInterestOps(int interestOps);
    static int  filterDownstreamInterestOps(int interestOps);

    Channels() {}
};

}
}

#endif //#if !defined(CETTY_CHANNEL_CHANNELS_H)

// Local Variables:
// mode: c++
// End:
