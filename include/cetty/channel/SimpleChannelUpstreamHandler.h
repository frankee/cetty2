#if !defined(CETTY_CHANNEL_SIMPLECHANNELUPSTREAMHANDLER_H)
#define CETTY_CHANNEL_SIMPLECHANNELUPSTREAMHANDLER_H

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

#include <cetty/channel/ChannelUpstreamHandler.h>

namespace cetty {
namespace logging {
class InternalLogger;
}
}

namespace cetty {
namespace channel {

class ChannelHandlerContext;
class ChannelEvent;
class MessageEvent;
class ExceptionEvent;
class ChannelStateEvent;
class ChildChannelStateEvent;
class WriteCompletionEvent;

using namespace cetty::logging;

/**
 * A {@link ChannelUpstreamHandler} which provides an individual handler method
 * for each event type.  This handler down-casts the received upstream event
 * into more meaningful sub-type event and calls an appropriate handler method
 * with the down-cast event.  The names of the methods are identical to the
 * upstream event names, as introduced in the {@link ChannelEvent} documentation.
 * <p>
 * Please use {@link SimpleChannelHandler} if you need to implement both
 * {@link ChannelUpstreamHandler} and {@link ChannelDownstreamHandler}.
 *
 * <h3>Overriding the {@link #handleUpstream(ChannelHandlerContext, ChannelEvent) handleUpstream} method</h3>
 * <p>
 * You can override the {@link #handleUpstream(ChannelHandlerContext, ChannelEvent) handleUpstream}
 * method just like overriding an ordinary Java method.  Please make sure to
 * call <tt>super.handleUpstream()</tt> so that other handler methods are invoked
 * properly:
 * </p>
 * <pre>public class MyChannelHandler extends {@link SimpleChannelUpstreamHandler} {
 *
 *     public void handleUpstream({@link ChannelHandlerContext} ctx, {@link ChannelEvent} e) throws Exception {
 *
 *         // Log all channel state changes.
 *         if (e instanceof {@link ChannelStateEvent}) {
 *             logger.info("Channel state changed: " + e);
 *         }
 *
 *         <strong>super.handleUpstream(ctx, e);</strong>
 *     }
 * }</pre>
 *
 *
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 * @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
 */

class SimpleChannelUpstreamHandler : public ChannelUpstreamHandler {
public:
    SimpleChannelUpstreamHandler();
    virtual ~SimpleChannelUpstreamHandler();

    virtual ChannelHandlerPtr clone();

    /**
     * {@inheritDoc}  Down-casts the received upstream event into more
     * meaningful sub-type event and calls an appropriate handler method with
     * the down-casted event.
     */
    virtual void handleUpstream(ChannelHandlerContext& ctx,
                                const ChannelEvent& e);

    /**
     * Invoked when a message object (e.g: {@link ChannelBuffer}) was received
     * from a remote peer.
     */
    virtual void messageReceived(ChannelHandlerContext& ctx,
                                 const MessageEvent& e);

    /**
     * Invoked when an exception was raised by an I/O thread or a
     * {@link ChannelHandler}.
     */
    virtual void exceptionCaught(ChannelHandlerContext& ctx,
                                 const ExceptionEvent& e);

    /**
     * Invoked when something was written into a {@link Channel}.
     */
    virtual void writeCompleted(ChannelHandlerContext& ctx,
                                const WriteCompletionEvent& e);

    /**
     * Invoked when a {@link Channel}'s state has been changed.
     */
    virtual void channelStateChanged(ChannelHandlerContext& ctx,
                                     const ChannelStateEvent& e);

    /**
     * Invoked when a child {@link Channel}'s state  has been changed.
     * (e.g. a server channel accepted a connection, or the accepted connection closed)
     */
    virtual void childChannelStateChanged(ChannelHandlerContext& ctx,
                                          const ChildChannelStateEvent& e);

    /**
     * Invoked when a {@link Channel} is open, but not bound nor connected.
     */
    virtual void channelOpen(ChannelHandlerContext& ctx,
                             const ChannelStateEvent& e);

    /**
     * Invoked when a {@link Channel} is open and bound to a local address,
     * but not connected.
     */
    virtual void channelBound(ChannelHandlerContext& ctx,
                              const ChannelStateEvent& e);

    /**
     * Invoked when a {@link Channel} is open, bound to a local address, and
     * connected to a remote address.
     */
    virtual void channelConnected(ChannelHandlerContext& ctx,
                                  const ChannelStateEvent& e);

    /**
     * Invoked when a {@link Channel}'s {@link Channel#getInterestOps() interestOps}
     * was changed.
     */
    virtual void channelInterestChanged(ChannelHandlerContext& ctx,
                                        const ChannelStateEvent& e);

    /**
     * Invoked when a {@link Channel} was disconnected from its remote peer.
     */
    virtual void channelDisconnected(ChannelHandlerContext& ctx,
                                     const ChannelStateEvent& e);

    /**
     * Invoked when a {@link Channel} was unbound from the current local address.
     */
    virtual void channelUnbound(ChannelHandlerContext& ctx,
                                const ChannelStateEvent& e);

    /**
     * Invoked when a {@link Channel} was closed and all its related resources
     * were released.
     */
    virtual void channelClosed(ChannelHandlerContext& ctx,
                               const ChannelStateEvent& e);

    /**
     * Invoked when a child {@link Channel} was open.
     * (e.g. a server channel accepted a connection)
     */
    virtual void childChannelOpen(ChannelHandlerContext& ctx,
                                  const ChildChannelStateEvent& e);

    /**
     * Invoked when a child {@link Channel} was closed.
     * (e.g. the accepted connection was closed)
     */
    virtual void childChannelClosed(ChannelHandlerContext& ctx,
                                    const ChildChannelStateEvent& e);


    virtual std::string toString() const;

private:
    static InternalLogger* logger;
};

}
}

#endif //#if !defined(CETTY_CHANNEL_SIMPLECHANNELUPSTREAMHANDLER_H)

// Local Variables:
// mode: c++
// End:
