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
#include <cetty/channel/ChannelInboundInvoker.h>
#include <cetty/channel/ChannelOutboundInvoker.h>

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

class ChannelHandlerContext
        : public ChannelOutboundInvoker, public ChannelInboundInvoker {

    friend ChannelPipeline;

public:
    ChannelHandlerContext(
        ChannelPipeline& pipeline,
        const std::string& name,
        const ChannelHandlerPtr& handler,
        DefaultChannelHandlerContext* prev,
        DefaultChannelHandlerContext* next)
        : defaultChannelPipeline(pipeline),
          channelPipeline(&pipeline),
          next(next),
          prev(prev),
          nextInboundContext(NULL),
          nextOutboundContext(NULL),
          canHandleInbound(false),
          canHandleOutbound(false),
          name(name),
          handler(handler),
          sink() {

        inboundHandler = boost::dynamic_pointer_cast<ChannelUpstreamHandler>(handler);

        if (inboundHandler) {
            this->canHandleInbound = true;
        }

        outboundHandler = boost::dynamic_pointer_cast<ChannelDownstreamHandler>(handler);

        if (outboundHandler) {
            this->canHandleOutbound = true;
        }

        if (!canHandleInbound && !canHandleOutbound) {
            throw InvalidArgumentException(
                "handler must be either ChannelUpstreamHandler or ChannelDownstreamHandler.");
        }
    }

    virtual ~ChannelHandlerContext() {}

    /**
     * Returns the {@link Channel} that the {@link ChannelPipeline} belongs to.
     * This method is a shortcut to <tt>getPipeline().getChannel()</tt>.
     * @return ChannelPtr which will not be NULL.
     */
    const ChannelPtr& getChannel() const {
        return pipeline.getChannel();
    }

    /**
     * Returns the {@link ChannelPipeline} that the {@link ChannelHandler}
     * belongs to.
     * @return ChannelPipelinePtr which will not be NULL.
     */
    const ChannelPipeline& getPipeline() const {
        return pipeline;
    }

    /**
     * Returns the name of the {@link ChannelHandler} in the
     * {@link ChannelPipeline}.
     */
    const std::string& getName() const {
        return this->name;
    }

    /**
     * Returns the {@link ChannelHandler} that this context object is
     * serving.
     */
    const ChannelHandlerPtr& getHandler() const {
        return this->handler;
    }

    /**
     * Returns the {@link ChannelUpstreamHandler} that this context object is
     * actually serving. If the context object serving is not ChannelUpstreamHandler,
     * then return an empty ChannelUpstreamHandlerPtr.
     */
    const ChannelInboundHandlerPtr& getInboundHandler() const {
        return this->inboundHandler;
    }

    /**
     * Returns the {@link ChannelDownstreamHandler} that this context object is
     * actually serving. If the context object serving is not ChannelDownstreamHandler,
     * then return an empty ChannelDownstreamHandlerPtr.
     */
    const ChannelOutboundHandlerPtr& getOutboundHandler() const {
        return this->outboundHandler;
    }

    /**
     * Returns <tt>true</tt> if and only if the {@link ChannelHandler} is an
     * instance of {@link ChannelUpstreamHandler}.
     */
    bool canHandleInboundMessage() const {
        return this->canHandleInbound;
    }

    /**
     * Returns <tt>true</tt> if and only if the {@link ChannelHandler} is an
     * instance of {@link ChannelDownstreamHandler}.
     */
    bool canHandleOutboundMessage() const {
        return this->canHandleOutbound;
    }

    virtual void fireChannelCreated() {
        ChannelHandlerContext* next = nextInboundContext;

        if (next) {
            fireChannelCreated(*next);
        }
    }
    
    virtual void fireChannelInactive() {
        ChannelHandlerContext* next = nextInboundContext;

        if (next) {
            fireChannelInactive(*next);
        }
    }
    
    virtual void fireExceptionCaught(const ChannelException& cause) {
        if (cause == null) {
            throw new NullPointerException("cause");
        }

        DefaultChannelHandlerContext next = this.next;

        if (next != null) {
            EventExecutor executor = next.executor();

            if (executor.inEventLoop()) {
                try {
                    next.handler().exceptionCaught(next, cause);
                }
                catch (Throwable t) {
                    if (logger.isWarnEnabled()) {
                        logger.warn(
                            "An exception was thrown by a user handler's " +
                            "exceptionCaught() method while handling the following exception:", cause);
                    }
                }
            }
            else {
                try {
                    executor.execute(new Runnable() {
                        @Override
                        public void run() {
                            fireExceptionCaught(cause);
                        }
                    });
                }
                catch (Throwable t) {
                    if (logger.isWarnEnabled()) {
                        logger.warn("Failed to submit an exceptionCaught() event.", t);
                        logger.warn("The exceptionCaught() event that was failed to submit was:", cause);
                    }
                }
            }
        }
        else {
            logger.warn(
                "An exceptionCaught() event was fired, and it reached at the end of the " +
                "pipeline.  It usually means the last inbound handler in the pipeline did not " +
                "handle the exception.", cause);
        }
    }

    virtual void fireEventTriggered(const ChannelEvent& event) {
        //if (event == null) {
        //    throw new NullPointerException("event");
        //}

        ChannelHandlerContext* next = nextInboundContext;
        if (next) {
            fireEventTriggered(*next, event);
        }
    }

    virtual void fireMessageUpdated() {
        ChannelHandlerContext* next = nextInboundContext;
        if (next) {
            fireMessageUpdated(*next);
        }
    }

    virtual ChannelFuturePtr bind(const SocketAddress& localAddress) {
        return bind(localAddress, newFuture());
    }

    virtual ChannelFuturePtr connect(const SocketAddress& remoteAddress) {
        return connect(remoteAddress, newFuture());
    }

    virtual ChannelFuturePtr connect(const SocketAddress& remoteAddress,
        const SocketAddress& localAddress) {
        return connect(remoteAddress, localAddress, newFuture());
    }

    virtual ChannelFuturePtr disconnect() {
        return disconnect(newFuture());
    }

    virtual ChannelFuturePtr close() {
        return close(newFuture());
    }

    virtual ChannelFuturePtr flush() {
        return flush(newFuture());
    }

    virtual ChannelFuturePtr write(const ChannelMessage& message) {
        return write(message, newFuture());
    }

    virtual const ChannelFuturePtr& bind(const SocketAddress& localAddress,
        const ChannelFuturePtr& future) {
            if (nextOutboundContext) {
                return bind(*nextOutboundContext, localAddress, future);
            }
    }

    virtual const ChannelFuturePtr& connect(const SocketAddress& remoteAddress,
        const ChannelFuturePtr& future) {
        return connect(remoteAddress, SocketAddress::NULL_ADDRESS, future);
    }

    virtual const ChannelFuturePtr& connect(const SocketAddress& remoteAddress,
        const SocketAddress& localAddress,
        const ChannelFuturePtr& future) {
            if (nextOutboundContext) {
                return connect(*nextOutboundContext, remoteAddress, localAddress, future);
            }
    }

    virtual const ChannelFuturePtr& disconnect(const ChannelFuturePtr& future) {
        if (nextOutboundContext) {
            disconnect(*nextOutboundContext, future);
        }
    }

    virtual const ChannelFuturePtr& close(const ChannelFuturePtr& future) {
        if (nextOutboundContext) {
            close(*nextOutboundContext, future);
        }
    }

    virtual const ChannelFuturePtr& flush(const ChannelFuturePtr& future) {
        if (nextOutboundContext) {
            flush(*nextOutboundContext, future);
        }
    }

    virtual const ChannelFuturePtr& write(const ChannelMessage& message,
        const ChannelFuturePtr& future) {
            if (nextOutboundContext) {
                return write(*nextOutboundContext, message, future);
            }
    }

private:
    void fireChannelCreated(ChannelHandlerContext& ctx) {
        if (!ctx.eventloop) {
            try {
                ctx.handler->channelCreated(ctx);
            }
            catch (const Exception& t) {
                pipeline.notifyHandlerException(t);
            }
        }
        else {
            ctx.eventloop->post(
                boost::bind(&ChannelHandlerContext::fireChannelCreated,
                this));
        }
    }

    void fireChannelInactive(ChannelHandlerContext& ctx) {
        if (ctx.eventloop) {
            try {
                ctx.handler->channelInactive(ctx);
            }
            catch (Throwable t) {
                pipeline.notifyHandlerException(t);
            }
        }
        else {
            ctx.eventloop->post(
                boost::bind(&ChannelHandlerContext::fireChannelInactive,
                this));
        }
    }

    void fireEventTriggered(ChannelHandlerContext& ctx, const ChannelEvent& event) {
        if (next != null) {
            EventExecutor executor = next.executor();

            if (executor.inEventLoop()) {
                try {
                    next.handler().userEventTriggered(next, event);
                }
                catch (Throwable t) {
                    pipeline.notifyHandlerException(t);
                }
            }
            else {
                executor.execute(new Runnable() {
                    @Override
                        public void run() {
                            fireUserEventTriggered(event);
                    }
                });
            }
        }
    }

    void fireMessageUpdated(ChannelHandlerContext& ctx) {
        EventExecutor executor = executor();

        if (ctx.eventloop) {
            try {
                ((ChannelStateHandler) ctx.handler).inboundBufferUpdated(ctx);
            }
            catch (Throwable t) {
                pipeline.notifyHandlerException(t);
            }
            finally {
                ByteBuf buf = inByteBuf;
                if (buf != null) {
                    if (!buf.readable()) {
                        buf.discardReadBytes();
                    }
                }
            }
            nextCtxFireInboundBufferUpdatedTask.run();
        }
        else {
            executor.execute(nextCtxFireInboundBufferUpdatedTask);
        }
    }

    const ChannelFuturePtr& bind(ChannelHandlerContext& ctx,
        const SocketAddress& localAddress,
        const ChannelFuturePtr& future) {

    }

    const ChannelFuturePtr& connect(ChannelHandlerContext& ctx,
        const SocketAddress& remoteAddress,
        const SocketAddress& localAddress,
        const ChannelFuturePtr& future) {

    }

    const ChannelFuturePtr& disconnect(ChannelHandlerContext& ctx,
        const ChannelFuturePtr& future) {
    }

    const ChannelFuturePtr& close(ChannelHandlerContext& ctx,
        const ChannelFuturePtr& future) {
    }
    const ChannelFuturePtr& flush(ChannelHandlerContext& ctx,
        const ChannelFuturePtr& future) {
            if (!eventloop) {
                ChannelHandlerContext* ctx = nextOutboundContext;

                try {
                    ctx->handler()->flush(*ctx, future);
                }
                catch (Throwable t) {
                    notifyHandlerException(t);
                }

                finally {
                    if (ctx.outByteBuf != null) {
                        ByteBuf buf = ctx.outByteBuf;

                        if (!buf.readable()) {
                            buf.discardReadBytes();
                        }
                    }
                }

                //pipeline.flush(prev, future);
            }
            else {
                eventloop->post(boost::bind(&ChannelHandlerContext::flush,
                    this,
                    future));
            }

            return future;
    }

    const ChannelFuturePtr& write(ChannelHandlerContext& ctx,
        const ChannelMessage& message,
        const ChannelFuturePtr& future)

    ChannelFuturePtr newFuture() {
        return channel->newFuture();
    }

    ChannelFuturePtr newSucceededFuture() {
        return channel.newSucceededFuture();
    }

    ChannelFuturePtr newFailedFuture(const ChannelException& cause) {
        return channel->newFailedFuture(cause);
    }

private:
    bool canHandleInbound;
    bool canHandleOutbound;

    EventLoopPtr eventloop;

    ChannelHandlerPtr           handler;
    ChannelInboundHandlerPtr   inboundHandler;
    ChannelOutboundHandlerPtr outboundHandler;

    ChannelHandlerContext* next;
    ChannelHandlerContext* prev;

    ChannelHandlerContext* nextInboundContext;
    ChannelHandlerContext* nextOutboundContext;

private:
    std::string name;

    ChannelPipeline& pipeline;
};

}
}

#endif //#if !defined(CETTY_CHANNEL_CHANNELHANDLERCONTEXT_H)

// Local Variables:
// mode: c++
// End:

