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

#include <boost/bind.hpp>

#include <cetty/channel/EventLoop.h>
#include <cetty/channel/ChannelPtr.h>
#include <cetty/channel/ChannelFuture.h>
#include <cetty/channel/ChannelInboundHandler.h>
#include <cetty/channel/ChannelOutboundHandler.h>
#include <cetty/channel/ChannelHandlerPtr.h>
#include <cetty/channel/ChannelInboundInvoker.h>
#include <cetty/channel/ChannelOutboundInvoker.h>
#include <cetty/channel/ChannelException.h>
#include <cetty/buffer/ChannelBuffer.h>

namespace cetty {
namespace channel {

using namespace cetty::buffer;

class ChannelPipeline;
class ChannelInboundBufferHandlerContext;
class ChannelOutboundBufferHandlerContext;

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
private:
    friend class ChannelPipeline;

public:
    ChannelHandlerContext(const std::string& name,
                          ChannelPipeline& pipeline,
                          const ChannelHandlerPtr& handler,
                          ChannelHandlerContext* prev,
                          ChannelHandlerContext* next);

    ChannelHandlerContext(const std::string& name,
                          const EventLoopPtr& eventLoop,
                          ChannelPipeline& pipeline,
                          const ChannelHandlerPtr& handler,
                          ChannelHandlerContext* prev,
                          ChannelHandlerContext* next);

    virtual ~ChannelHandlerContext() {}

    /**
     * Returns the {@link Channel} that the {@link ChannelPipeline} belongs to.
     * This method is a shortcut to <tt>getPipeline().getChannel()</tt>.
     * @return ChannelPtr which will not be NULL.
     */
    const ChannelPtr& getChannel() const;

    /**
     * Returns the {@link ChannelPipeline} that the {@link ChannelHandler}
     * belongs to.
     * @return ChannelPipelinePtr which will not be NULL.
     */
    const ChannelPipeline& getPipeline() const {
        return pipeline;
    }

    ChannelPipeline& getPipeline() {
        return pipeline;
    }

    const EventLoopPtr& getEventLoop() const {
        return eventloop;
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
    const ChannelHandlerPtr& getHandler() const;

    /**
     * Returns the {@link ChannelUpstreamHandler} that this context object is
     * actually serving. If the context object serving is not ChannelUpstreamHandler,
     * then return an empty ChannelUpstreamHandlerPtr.
     */
    const ChannelInboundHandlerPtr& getInboundHandler() const;

    /**
     * Returns the {@link ChannelDownstreamHandler} that this context object is
     * actually serving. If the context object serving is not ChannelDownstreamHandler,
     * then return an empty ChannelDownstreamHandlerPtr.
     */
    const ChannelOutboundHandlerPtr& getOutboundHandler() const;

    ChannelInboundBufferHandlerContext* inboundBufferHandlerContext();
    ChannelOutboundBufferHandlerContext* outboundBufferHandlerContext();

    template<typename T>
    T* inboundMessageHandlerContext() {
        return dynamic_cast<T*>(this);
    }

    template<typename T>
    T* outboundMessageHandlerContext() {
        return dynamic_cast<T*>(this);
    }

    ChannelInboundBufferHandlerContext* nextInboundBufferHandlerContext() {
        return nextInboundBufferHandlerContext(nextInboundContext);
    }
    ChannelOutboundBufferHandlerContext* nextOutboundBufferHandlerContext() {
        return nextOutboundBufferHandlerContext(nextOutboundContext);
    }

    ChannelInboundBufferHandlerContext* nextInboundBufferHandlerContext(ChannelHandlerContext* ctx);
    ChannelOutboundBufferHandlerContext* nextOutboundBufferHandlerContext(ChannelHandlerContext* ctx);

    template<typename T>
    T* nextInboundMessageHandlerContext() {
        return nextInboundMessageHandlerContext<T>(nextInboundContext);
    }

    template<typename T>
    T* nextInboundMessageHandlerContext(ChannelHandlerContext* ctx) {
        ChannelHandlerContext* next = ctx;

        if (!next) {
            return (T*)NULL;
        }

        if (next->isInboundMessageHandler()) {
            T* context = dynamic_cast<T*>(next);

            if (context) { return context; }
        }

        next = next->nextInboundContext;

        while (true) {
            if (!next) {
                return (T*)NULL;
            }

            if (next->isInboundMessageHandler()) {
                T* context = dynamic_cast<T*>(next);

                if (context) { return context; }
            }

            next = next->nextInboundContext;
        }
    }

    template<typename T>
    T* nextOutboundMessageHandlerContext() {
        return nextOutboundMessageHandlerContext<T>(nextOutboundContext);
    }

    template<typename T>
    T* nextOutboundMessageHandlerContext(ChannelHandlerContext* ctx) {
        ChannelHandlerContext* next = nextOutboundContext;

        if (!next) {
            return (T*)NULL;
        }

        if (next->isOutboundMessageHandler()) {
            return dynamic_cast<T*>(next);
        }

        next = next->nextInboundContext;

        while (true) {
            if (!next) {
                return (T*)NULL;
            }

            if (next->isOutboundMessageHandler()) {
                return dynamic_cast<T*>(next);
            }

            next = next->nextInboundContext;
        }
    }

    /**
     * Returns <tt>true</tt> if and only if the {@link ChannelHandler} is an
     * instance of {@link ChannelUpstreamHandler}.
     */
    bool canHandleInboundMessage() const { return canHandleInbound; }

    /**
     * Returns <tt>true</tt> if and only if the {@link ChannelHandler} is an
     * instance of {@link ChannelDownstreamHandler}.
     */
    bool canHandleOutboundMessage() const { return canHandleOutbound; }

    virtual void fireChannelCreated();
    virtual void fireChannelActive();
    virtual void fireChannelInactive();
    virtual void fireExceptionCaught(const ChannelException& cause);
    virtual void fireUserEventTriggered(const UserEvent& event);
    virtual void fireMessageUpdated();
    virtual void fireWriteCompleted();

    virtual ChannelFuturePtr bind(const SocketAddress& localAddress);

    virtual ChannelFuturePtr connect(const SocketAddress& remoteAddress);

    virtual ChannelFuturePtr connect(const SocketAddress& remoteAddress,
                                     const SocketAddress& localAddress);

    virtual ChannelFuturePtr disconnect();

    virtual ChannelFuturePtr close();

    virtual ChannelFuturePtr flush();

    virtual const ChannelFuturePtr& bind(const SocketAddress& localAddress,
                                         const ChannelFuturePtr& future);

    virtual const ChannelFuturePtr& connect(const SocketAddress& remoteAddress,
                                            const ChannelFuturePtr& future);

    virtual const ChannelFuturePtr& connect(const SocketAddress& remoteAddress,
                                            const SocketAddress& localAddress,
                                            const ChannelFuturePtr& future);

    virtual const ChannelFuturePtr& disconnect(const ChannelFuturePtr& future);

    virtual const ChannelFuturePtr& close(const ChannelFuturePtr& future);

    virtual const ChannelFuturePtr& flush(const ChannelFuturePtr& future);

    void fireChannelCreated(ChannelHandlerContext& ctx);
    void fireChannelActive(ChannelHandlerContext& ctx);
    void fireChannelInactive(ChannelHandlerContext& ctx);
    void fireMessageUpdated(ChannelHandlerContext& ctx);
    void fireWriteCompleted(ChannelHandlerContext& ctx);
    void fireExceptionCaught(ChannelHandlerContext& ctx,
                             const ChannelException& cause);
    void fireUserEventTriggered(ChannelHandlerContext& ctx,
                                const UserEvent& event);


    const ChannelFuturePtr& bind(ChannelHandlerContext& ctx,
                                 const SocketAddress& localAddress,
                                 const ChannelFuturePtr& future);

    const ChannelFuturePtr& connect(ChannelHandlerContext& ctx,
                                    const SocketAddress& remoteAddress,
                                    const SocketAddress& localAddress,
                                    const ChannelFuturePtr& future);

    const ChannelFuturePtr& disconnect(ChannelHandlerContext& ctx,
                                       const ChannelFuturePtr& future);

    const ChannelFuturePtr& close(ChannelHandlerContext& ctx,
                                  const ChannelFuturePtr& future);

    const ChannelFuturePtr& flush(ChannelHandlerContext& ctx,
                                  const ChannelFuturePtr& future);

protected:
    virtual bool isInboundBufferHandler() const { return false; }
    virtual bool isInboundMessageHandler() const { return false; }
    virtual bool isOutboundBufferHandler() const { return false; }
    virtual bool isOutboundMessageHandler() const { return false; }

private:
    void init(const ChannelHandlerPtr& handler);

    void attach();
    void detach();

    ChannelFuturePtr newFuture();
    ChannelFuturePtr newSucceededFuture();
    ChannelFuturePtr newFailedFuture(const ChannelException& cause);

    void notifyHandlerException();
    void notifyHandlerException(const std::exception& e);
    void notifyHandlerException(const Exception& e);

protected:
    bool canHandleInbound;
    bool canHandleOutbound;

    ChannelHandlerPtr         handler;
    ChannelInboundHandlerPtr  inboundHandler;
    ChannelOutboundHandlerPtr outboundHandler;

    ChannelHandlerContext* next;
    ChannelHandlerContext* prev;

    ChannelHandlerContext* nextInboundContext;
    ChannelHandlerContext* nextOutboundContext;

    std::string name;

    ChannelPipeline& pipeline;
    EventLoopPtr eventloop;
};

}
}

#endif //#if !defined(CETTY_CHANNEL_CHANNELHANDLERCONTEXT_H)

// Local Variables:
// mode: c++
// End:
