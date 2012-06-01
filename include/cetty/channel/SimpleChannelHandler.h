#if !defined(CETTY_CHANNEL_SIMPLECHANNELHANDLER_H)
#define CETTY_CHANNEL_SIMPLECHANNELHANDLER_H

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

#include <cetty/channel/SimpleChannelUpstreamHandler.h>
#include <cetty/channel/SimpleChannelDownstreamHandler.h>

namespace cetty {
namespace logging {
class InternalLogger;
}
}

namespace cetty {
namespace channel {

using namespace cetty::logging;

class ChannelHandlerContext;
class ChannelEvent;
class MessageEvent;
class ExceptionEvent;
class ChannelStateEvent;
class ChildChannelStateEvent;
class WriteCompletionEvent;

/**
 * A {@link ChannelHandler} which provides an individual handler method
 * for each event type.  This handler down-casts the received upstream or
 * or downstream event into more meaningful sub-type event and calls an
 * appropriate handler method with the down-cast event.  For an upstream
 * event, the names of the methods are identical to the upstream event names,
 * as introduced in the {@link ChannelEvent} documentation.  For a
 * downstream event, the names of the methods starts with the name of the
 * operation and ends with <tt>"Requested"</tt>
 * (e.g. {@link #writeRequested(ChannelHandlerContext, MessageEvent) writeRequested}.)
 * <p>
 * Please use {@link SimpleChannelUpstreamHandler} or
 * {@link SimpleChannelDownstreamHandler} if you want to intercept only
 * upstream or downstream events.
 *
 * <h3>Overriding the {@link #handleUpstream(ChannelHandlerContext, ChannelEvent) handleUpstream}
 *     and {@link #handleDownstream(ChannelHandlerContext, ChannelEvent) handleDownstream} method</h3>
 * <p>
 * You can override the {@link #handleUpstream(ChannelHandlerContext, ChannelEvent) handleUpstream}
 * and {@link #handleDownstream(ChannelHandlerContext, ChannelEvent) handleDownstream}
 * method just like overriding an ordinary Java method.  Please make sure to
 * call <tt>super.handleUpstream()</tt> or <tt>super.handleDownstream()</tt> so
 * that other handler methods are invoked properly:
 * </p>
 * <pre>class MyChannelHandler : public {@link SimpleChannelHandler} {
 * public:
 *     void handleUpstream({@link ChannelHandlerContext} ctx, {@link ChannelEvent} e) {
 *
 *         // Log all channel state changes.
 *         if (e instanceof {@link ChannelStateEvent}) {
 *             logger.info("Channel state changed: " + e);
 *         }
 *
 *         <strong>super.handleUpstream(ctx, e);</strong>
 *     }
 *
 *     void handleDownstream({@link ChannelHandlerContext} ctx, {@link ChannelEvent} e) {
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
 *
 * @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
 */

class SimpleChannelHandler :  public SimpleChannelUpstreamHandler,
    public SimpleChannelDownstreamHandler {
public:
    SimpleChannelHandler();
    virtual ~SimpleChannelHandler();

    virtual ChannelHandlerPtr clone();
    virtual std::string toString() const;
};

}
}

#endif //#if !defined(CETTY_CHANNEL_SIMPLECHANNELHANDLER_H)

// Local Variables:
// mode: c++
// End:

