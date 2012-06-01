#if !defined(CETTY_CHANNEL_SIMPLECHANNELDOWNSTREAMHANDLER_H)
#define CETTY_CHANNEL_SIMPLECHANNELDOWNSTREAMHANDLER_H

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

#include <cetty/channel/ChannelDownstreamHandler.h>

namespace cetty {
namespace channel {

class ChannelEvent;
class MessageEvent;
class ChannelStateEvent;
class ChannelHandlerContext;

/**
 * A {@link ChannelDownstreamHandler} which provides an individual handler
 * method for each event type.  This handler down-casts the received downstream
 * event into more meaningful sub-type event and calls an appropriate handler
 * method with the down-cast event.  The names of the methods starts with the
 * name of the operation and ends with <tt>"Requested"</tt>
 * (e.g. {@link #writeRequested(ChannelHandlerContext, MessageEvent) writeRequested}.)
 * <p>
 * Please use {@link SimpleChannelHandler} if you need to implement both
 * {@link ChannelUpstreamHandler} and {@link ChannelDownstreamHandler}.
 *
 * <h3>Overriding the {@link #handleDownstream(ChannelHandlerContext, ChannelEvent) handleDownstream} method</h3>
 * <p>
 * You can override the {@link #handleDownstream(ChannelHandlerContext, ChannelEvent) handleDownstream}
 * method just like overriding an ordinary Java method.  Please make sure to
 * call <tt>super.handleDownstream()</tt> so that other handler methods are
 * invoked properly:
 * </p>
 * <pre>public class MyChannelHandler extends {@link SimpleChannelDownstreamHandler} {
 *
 *     public void handleDownstream({@link ChannelHandlerContext} ctx, {@link ChannelEvent} e) {
 *
 *         // Log all channel state changes.
 *         if (e instanceof {@link MessageEvent}) {
 *             logger.info("Writing:: " + e);
 *         }
 *
 *         <strong>super.handleDownstream(ctx, e);</strong>
 *     }
 * }</pre>
 *
 *
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 * @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
 */

class SimpleChannelDownstreamHandler : public ChannelDownstreamHandler {
public:
    /**
     * Creates a new instance.
     */
    SimpleChannelDownstreamHandler();
    virtual ~SimpleChannelDownstreamHandler();

    virtual ChannelHandlerPtr clone();

    virtual std::string toString() const;

    /**
     * {@inheritDoc}  Down-casts the received downstream event into more
     * meaningful sub-type event and calls an appropriate handler method with
     * the down-casted event.
     */
    virtual void handleDownstream(ChannelHandlerContext& ctx, const ChannelEvent& e);


    virtual void stateChangeRequested(ChannelHandlerContext& ctx, const ChannelStateEvent& e);

    /**
     * Invoked when {@link Channel#write(Object)} is called.
     */
    virtual void writeRequested(ChannelHandlerContext& ctx, const MessageEvent& e);

    /**
     * Invoked when {@link Channel#bind(SocketAddress)} was called.
     */
    virtual void bindRequested(ChannelHandlerContext& ctx, const ChannelStateEvent& e);

    /**
     * Invoked when {@link Channel#connect(SocketAddress)} was called.
     */
    virtual void connectRequested(ChannelHandlerContext& ctx, const ChannelStateEvent& e);

    /**
     * Invoked when {@link Channel#setInterestOps(int)} was called.
     */
    virtual void setInterestOpsRequested(ChannelHandlerContext& ctx, const ChannelStateEvent& e);

    /**
     * Invoked when {@link Channel#disconnect()} was called.
     */
    virtual void disconnectRequested(ChannelHandlerContext& ctx, const ChannelStateEvent& e);

    /**
     * Invoked when {@link Channel#unbind()} was called.
     */
    virtual void unbindRequested(ChannelHandlerContext& ctx, const ChannelStateEvent& e);

    /**
     * Invoked when {@link Channel#close()} was called.
     */
    virtual void closeRequested(ChannelHandlerContext& ctx, const ChannelStateEvent& e);
};

}
}

#endif //#if !defined(CETTY_CHANNEL_SIMPLECHANNELDOWNSTREAMHANDLER_H)

// Local Variables:
// mode: c++
// End:

