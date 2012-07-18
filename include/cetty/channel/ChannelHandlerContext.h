#if !defined(CETTY_CHANNEL_CHANNELHANDLERCONTEXT_H)
#define CETTY_CHANNEL_CHANNELHANDLERCONTEXT_H

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

#include <cetty/channel/ChannelFwd.h>
#include <cetty/channel/ChannelHandlerFwd.h>
#include <cetty/channel/ChannelPipelineFwd.h>

namespace cetty {
	namespace channel {

		class ChannelEvent;
		class MessageEvent;
		class ExceptionEvent;
		class ChannelStateEvent;
		class WriteCompletionEvent;
		class ChildChannelStateEvent;

/**
 * Enables a {@link ChannelHandler} to interact with its {@link ChannelPipeline}
 * and other handlers.  A handler can send a {@link ChannelEvent} upstream or
 * downstream, modify the {@link ChannelPipeline} it belongs to dynamically.
 *
 * <h3>Sending an event</h3>
 *
 * You can send or forward a {@link ChannelEvent} to the closest handler in the
 * same {@link ChannelPipeline} by calling {@link #sendUpstream(ChannelEvent)}
 * or {@link #sendDownstream(ChannelEvent)}.  Please refer to
 * {@link ChannelPipeline} to understand how an event flows.
 *
 * <h3>Modifying a pipeline</h3>
 *
 * You can get the {@link ChannelPipeline} your handler belongs to by calling
 * {@link #getPipeline()}.  A non-trivial application could insert, remove, or
 * replace handlers in the pipeline dynamically in runtime.
 *
 * <h3>Retrieving for later use</h3>
 *
 * You can keep the {@link ChannelHandlerContext} for later use, such as
 * triggering an event outside the handler methods, even from a different thread.
 * <pre>
 * public class MyHandler extends {@link SimpleChannelHandler}
 *                        implements {@link LifeCycleAwareChannelHandler} {
 *
 *     <b>private {@link ChannelHandlerContext} ctx;</b>
 *
 *     public void beforeAdd({@link ChannelHandlerContext} ctx) {
 *         <b>this.ctx = ctx;</b>
 *     }
 *
 *     public void login(std::string username, password) {
 *         {@link Channels}.write(
 *                 <b>this.ctx</b>,
 *                 {@link Channels}.succeededFuture(<b>this.ctx</t>.getChannel()),
 *                 new LoginMessage(username, password));
 *     }
 *     ...
 * }
 * </pre>
 *
 * <h3>Storing stateful information</h3>
 *
 * {@link #setAttachment(Object)} and {@link #getAttachment()} allow you to
 * store and access stateful information that is related with a handler and its
 * context.  Please refer to {@link ChannelHandler} to learn various recommended
 * ways to manage stateful information.
 *
 * <h3>A handler can have more than one context</h3>
 *
 * Please note that a {@link ChannelHandler} instance can be added to more than
 * one {@link ChannelPipeline}.  It means a single {@link ChannelHandler}
 * instance can have more than one {@link ChannelHandlerContext} and therefore
 * the single instance can be invoked with different
 * {@link ChannelHandlerContext}s if it is added to one or more
 * {@link ChannelPipeline}s more than once.
 * <p>
 * For example, the following handler will have as many independent attachments
 * as how many times it is added to pipelines, regardless if it is added to the
 * same pipeline multiple times or added to different pipelines multiple times:
 * <pre>
 * public class FactorialHandler extends {@link SimpleChannelHandler} {
 *
 *   // This handler will receive a sequence of increasing integers starting
 *   // from 1.
 *   public void messageReceived({@link ChannelHandlerContext} ctx, {@link MessageEvent} evt) {
 *     Integer a = (Integer) ctx.getAttachment();
 *     Integer b = (Integer) evt.getMessage();
 *
 *     if (a == NULL) {
 *       a = 1;
 *     }
 *
 *     ctx.setAttachment(Integer.valueOf(a * b));
 *   }
 * }
 *
 * // Different context objects are given to "f1", "f2", "f3", and "f4" even if
 * // they refer to the same handler instance.  Because the FactorialHandler
 * // stores its state in a context object (as an attachment), the factorial is
 * // calculated correctly 4 times once the two pipelines (p1 and p2) are active.
 * FactorialHandler fh = new FactorialHandler();
 *
 * {@link ChannelPipeline} p1 = {@link Channels}.pipeline();
 * p1.addLast("f1", fh);
 * p1.addLast("f2", fh);
 *
 * {@link ChannelPipeline} p2 = {@link Channels}.pipeline();
 * p2.addLast("f3", fh);
 * p2.addLast("f4", fh);
 * </pre>
 *
 * <h3>Additional resources worth reading</h3>
 * <p>
 * Please refer to the {@link ChannelHandler}, {@link ChannelEvent}, and
 * {@link ChannelPipeline} to find out what a upstream event and a downstream
 * event are, what fundamental differences they have, how they flow in a
 * pipeline,  and how to handle the event in your application.
 *
 *
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 * @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
 *
 */

class ChannelHandlerContext {
public:
    virtual ~ChannelHandlerContext() {}

    /**
     * Returns the {@link Channel} that the {@link ChannelPipeline} belongs to.
     * This method is a shortcut to <tt>getPipeline().getChannel()</tt>.
     * @return ChannelPtr which will not be NULL.
     */
    virtual const ChannelPtr& getChannel() const = 0;

    /**
     * Returns the {@link ChannelPipeline} that the {@link ChannelHandler}
     * belongs to.
     * @return ChannelPipelinePtr which will not be NULL.
     */
    virtual const ChannelPipelinePtr& getPipeline() const = 0;

    /**
     * Returns the name of the {@link ChannelHandler} in the
     * {@link ChannelPipeline}.
     */
    virtual const std::string& getName() const = 0;

    /**
     * Returns the {@link ChannelHandler} that this context object is
     * serving.
     */
    virtual const ChannelHandlerPtr& getHandler() const = 0;

    /**
     * Returns the {@link ChannelUpstreamHandler} that this context object is
     * actually serving. If the context object serving is not ChannelUpstreamHandler,
     * then return an empty ChannelUpstreamHandlerPtr.
     */
    virtual const ChannelUpstreamHandlerPtr& getUpstreamHandler() const = 0;

    /**
     * Returns the {@link ChannelDownstreamHandler} that this context object is
     * actually serving. If the context object serving is not ChannelDownstreamHandler,
     * then return an empty ChannelDownstreamHandlerPtr.
     */
    virtual const ChannelDownstreamHandlerPtr& getDownstreamHandler() const = 0;

    /**
     * Returns <tt>true</tt> if and only if the {@link ChannelHandler} is an
     * instance of {@link ChannelUpstreamHandler}.
     */
    virtual bool canHandleUpstream() const = 0;

    /**
     * Returns <tt>true</tt> if and only if the {@link ChannelHandler} is an
     * instance of {@link ChannelDownstreamHandler}.
     */
    virtual bool canHandleDownstream() const = 0;

    /**
     * Sends the specified {@link ChannelEvent}, except {@MessageEvent},
     * {@ChannelStateEvent}, {@ChildChannelStateEvent},
     * {@WriteCompletionEvent} and {@ExceptionEvent}, to the
     * {@link ChannelUpstreamHandler} which is placed in the closest upstream
     * from the handler associated with this context.  It is recommended to use
     * the shortcut methods in {@link Channels} rather than calling this method
     * directly.
     */
    virtual void sendUpstream(const ChannelEvent& e) = 0;

    /**
     * Sends the specified {@link MessageEvent} to the
     * {@link ChannelUpstreamHandler} which is placed in the closest upstream
     * from the handler associated with this context.  It is recommended to use
     * the shortcut methods in {@link Channels} rather than calling this method
     * directly.
     */
    virtual void sendUpstream(const MessageEvent& e) = 0;

    /**
     * Sends the specified {@link ChannelStateEvent} to the
     * {@link ChannelUpstreamHandler} which is placed in the closest upstream
     * from the handler associated with this context.  It is recommended to use
     * the shortcut methods in {@link Channels} rather than calling this method
     * directly.
     */
    virtual void sendUpstream(const ChannelStateEvent& e) = 0;

    /**
     * Sends the specified {@link ChildChannelStateEvent} to the
     * {@link ChannelUpstreamHandler} which is placed in the closest upstream
     * from the handler associated with this context.  It is recommended to use
     * the shortcut methods in {@link Channels} rather than calling this method
     * directly.
     */
    virtual void sendUpstream(const ChildChannelStateEvent& e) = 0;

    /**
     * Sends the specified {@link WriteCompletionEvent} to the
     * {@link ChannelUpstreamHandler} which is placed in the closest upstream
     * from the handler associated with this context.  It is recommended to use
     * the shortcut methods in {@link Channels} rather than calling this method
     * directly.
     */
    virtual void sendUpstream(const WriteCompletionEvent& e) = 0;

    /**
     * Sends the specified {@link ExceptionEvent} to the
     * {@link ChannelUpstreamHandler} which is placed in the closest upstream
     * from the handler associated with this context.  It is recommended to use
     * the shortcut methods in {@link Channels} rather than calling this method
     * directly.
     */
    virtual void sendUpstream(const ExceptionEvent& e) = 0;

    /**
     * Sends the specified {@link ChannelEvent}, except {@MessageEvent}
     * and {@ChannelStateEvent} to the
     * {@link ChannelDownstreamHandler} which is placed in the closest
     * downstream from the handler associated with this context.  It is
     * recommended to use the shortcut methods in {@link Channels} rather than
     * calling this method directly.
     */
    virtual void sendDownstream(const ChannelEvent& e) = 0;

    /**
     * Sends the specified {@link MessageEvent} to the
     * {@link ChannelDownstreamHandler} which is placed in the closest
     * downstream from the handler associated with this context.  It is
     * recommended to use the shortcut methods in {@link Channels} rather than
     * calling this method directly.
     */
    virtual void sendDownstream(const MessageEvent& e) = 0;

    /**
     * Sends the specified {@link ChannelStateEvent} to the
     * {@link ChannelDownstreamHandler} which is placed in the closest
     * downstream from the handler associated with this context.  It is
     * recommended to use the shortcut methods in {@link Channels} rather than
     * calling this method directly.
     */
    virtual void sendDownstream(const ChannelStateEvent& e) = 0;

    /**
     * Retrieves an pointer which is {@link #setAttachment(void* attachment) attached} to
     * this context.
     *
     * @return <tt>NULL</tt> if no pointer was attached or
     *                      <tt>NULL</tt> was attached
     */
    virtual void* getAttachment() = 0;

    /**
     * Retrieves an pointer which is {@link #setAttachment(void* attachment) attached} to
     * this context.
     *
     * @return <tt>NULL</tt> if no pointer was attached or
     *                      <tt>NULL</tt> was attached
     */
    virtual const void* getAttachment() const = 0;

    /**
     * Attaches an object to this context to store a stateful information
     * specific to the {@link ChannelHandler} which is associated with this
     * context.
     */
    virtual void setAttachment(void* attachment) = 0;
};

}
}

#endif //#if !defined(CETTY_CHANNEL_CHANNELHANDLERCONTEXT_H)

// Local Variables:
// mode: c++
// End:
