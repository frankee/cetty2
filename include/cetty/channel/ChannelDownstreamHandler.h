#if !defined(CETTY_CHANNEL_CHANNELDOWNSTREAMHANDLER_H)
#define CETTY_CHANNEL_CHANNELDOWNSTREAMHANDLER_H

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

#include <cetty/channel/ChannelHandler.h>

namespace cetty {
namespace channel {

class ChannelEvent;
class MessageEvent;
class ChannelStateEvent;

class ChannelHandlerContext;

/**
 * Handles or intercepts a downstream {@link ChannelEvent}, and sends a
 * {@link ChannelEvent} to the next handler in a {@link ChannelPipeline}.
 * <p>
 * The most common use case of this interface is to intercept an I/O request
 * such as {@link Channel#write(const ChannelMessage&, bool)} and {@link Channel#close()}.
 *
 * <h3>{@link SimpleChannelDownstreamHandler}</h3>
 * <p>
 * In most cases, you will get to use a {@link SimpleChannelDownstreamHandler}
 * to implement a downstream handler because it provides an individual handler
 * method for each event type.  You might want to implement this interface
 * directly though if you want to handle various types of events in more
 * generic way.
 *
 * <h3>Firing an event to the next handler</h3>
 * <p>
 * You can forward the received event downstream or upstream.  In most cases,
 * {@link ChannelDownstreamHandler} will send the event downstream
 * (i.e. outbound) although it is legal to send the event upstream (i.e. inbound):
 *
 * <pre>
 * // Sending the event downstream (outbound)
 * void handleDownstream({@link ChannelHandlerContext } &ctx, const {@link ChannelEvent} &e) {
 *     ...
 *     ctx.sendDownstream(e);
 *     ...
 * }
 *
 * // Sending the event upstream (inbound)
 * void handleDownstream({@link ChannelHandlerContext} &ctx, const {@link ChannelEvent} &e) {
 *     ...
 *     ctx.sendUpstream(new {@link UpstreamChannelStateEvent}(...));
 *     ...
 * }
 * </pre>
 *
 * <p>
 * Special downstream event, MessageEvent &  ChannelStateEvent will invoke
 * {@link #writeRequested(ChannelHandlerContext&, const MessageEvent&) writeRequested},
 * {@link #stateChangeRequested(ChannelHandlerContext&, const ChannelStateEvent&) stateChangeRequested}.
 * Others downstream event will invoke
 * {@link #handleDownstream(ChannelHandlerContext&, const ChannelEvent&) handleDownstream}.
 *
 * <h4>Using the helper class to send an event</h4>
 * <p>
 * You will also find various helper methods in {@link Channels} to be useful
 * to generate and send an artificial or manipulated event.
 *
 * <h3>State management</h3>
 *
 * Please refer to {@link ChannelHandler}.
 *
 * <h3>Thread safety</h3>
 * <p>
 * {@link #handleDownstream(ChannelHandlerContext&, const ChannelEvent&) handleDownstream},
 * {@link #writeRequested(ChannelHandlerContext&, const MessageEvent&) writeRequested} and
 * {@link #stateChangeRequested(ChannelHandlerContext&, const ChannelStateEvent&) stateChangeRequested}
 * may be invoked by more than one thread simultaneously if the handler is sharedable.
 * If the handler accesses a shared resource or stores stateful information, you might need
 * proper synchronization in the handler implementation.
 *
 *
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 * @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
 *
 */

class ChannelDownstreamHandler : virtual public ChannelHandler {
public:
    ChannelDownstreamHandler(){}
    virtual ~ChannelDownstreamHandler() {}

    /**
     * Handles the specified downstream event except MessageEvent and ChannelStateEvent.
     *
     * @param ctx  the context object for this handler
     * @param e    the downstream event to process or intercept
     */
    virtual void handleDownstream(ChannelHandlerContext& ctx, const ChannelEvent& e) = 0;

    /**
     * Handles the downstream MessageEvent.
     *
     * @param ctx  the context object for this handler
     * @param e    the downstream MessageEvent to process or intercept
     */
    virtual void writeRequested(ChannelHandlerContext& ctx, const MessageEvent& e) = 0;

    /**
     * Handles the downstream ChannelStateEvent.
     *
     * @param ctx  the context object for this handler
     * @param e    the downstream ChannelStateEvent to process or intercept
     */
    virtual void stateChangeRequested(ChannelHandlerContext& ctx, const ChannelStateEvent& e) = 0;

private:
    ChannelDownstreamHandler(const ChannelDownstreamHandler&);
    ChannelDownstreamHandler& operator=(const ChannelDownstreamHandler&);
};

}
}

#endif //#if !defined(CETTY_CHANNEL_CHANNELDOWNSTREAMHANDLER_H)

// Local Variables:
// mode: c++
// End:
