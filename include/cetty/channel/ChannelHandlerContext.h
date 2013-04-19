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

#include <string>
#include <boost/any.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include <cetty/channel/EventLoop.h>
#include <cetty/channel/ChannelPtr.h>
#include <cetty/channel/ChannelFuture.h>
#include <cetty/channel/InetAddress.h>
#include <cetty/channel/ChannelException.h>
#include <cetty/channel/ChannelMessageContainer.h>
#include <cetty/buffer/ChannelBuffer.h>

namespace cetty {
namespace channel {

using namespace cetty::buffer;

class ChannelPipeline;
class ChannelPipelineException;

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

class ChannelHandlerContext : private boost::noncopyable {
public:
    /**
     * Gets called before the {@link ChannelHandler} is added to the actual context.
     */
    typedef boost::function<void (ChannelHandlerContext&)> BeforeAddCallback;

    /**
     * Gets called after the {@link ChannelHandler} was added to the actual context.
     */
    typedef boost::function<void (ChannelHandlerContext&)> AfterAddCallback;

    /**
     * Gets called before the {@link ChannelHandler} is removed from the actual context.
     */
    typedef boost::function<void (ChannelHandlerContext&)> BeforeRemoveCallback;

    /**
     * Gets called after the {@link ChannelHandler} was removed from the actual context.
     */
    typedef boost::function<void (ChannelHandlerContext&)> AfterRemoveCallback;

    /**
     * Gets called after the {@link ChannelPipeline} was changed.
     */
    typedef boost::function<void (ChannelHandlerContext&)> PipelineChangedCallback;

    /**
     * Gets called if an user event was triggered.
     */
    typedef boost::function<void (ChannelHandlerContext&,
                                  const boost::any&)> UserEventCallback;

    /**
     * Gets called if a {@link Exception} was thrown.
     */
    typedef boost::function<void (ChannelHandlerContext&,
                                  const ChannelException&)> ChannelExceptionCallback;

    /**
     * The {@link Channel} of the {@link ChannelHandlerContext} was registered with its {@link EventLoop}
     */
    typedef boost::function<void (ChannelHandlerContext&)> ChannelOpenCallback;

    /**
     * The {@link Channel} of the {@link ChannelHandlerContext} is now active
     */
    typedef boost::function<void (ChannelHandlerContext&)> ChannelActiveCallback;

    /**
     * The {@link Channel} of the {@link ChannelHandlerContext} is now inactive and reached its
     * end of lifetime.
     */
    typedef boost::function<void (ChannelHandlerContext&)> ChannelInactiveCallback;

    /**
     * The inbound message of the {@link ChannelHandlerContext} was updated with new data.
     * This means something may be ready to get processed by the actual {@link ChannelHandler}
     * implementation. It's up to the implementation to consume it or keep it in the buffer
     * to wait for more data and consume it later.
     */
    typedef boost::function<void (ChannelHandlerContext&)> ChannelMessageUpdatedCallback;

    /**
     * Invoked when a {@link ChannelHandlerContext#read()} is finished and the inbound buffer of this handler will not
     * be updated until another {@link ChannelHandlerContext#read()} request is issued.
     */
    typedef boost::function<void (ChannelHandlerContext&)> ChannelReadSuspendedCallback;

    /**
     * Called once a bind operation is made.
     *
     * @param ctx           the {@link ChannelHandlerContext} for which the bind operation is made
     * @param localAddress  the {@link SocketAddress} to which it should bound
     * @param promise       the {@link ChannelPromise} to notify once the operation completes
     * @throws Exception    thrown if an error accour
     */
    typedef boost::function<void (ChannelHandlerContext&,
                                  const InetAddress&,
                                  const ChannelFuturePtr&)> BindFunctor;

    /**
     * Called once a connect operation is made.
     *
     * @param ctx               the {@link ChannelHandlerContext} for which the connect operation is made
     * @param remoteAddress     the {@link SocketAddress} to which it should connect
     * @param localAddress      the {@link SocketAddress} which is used as source on connect
     * @param promise           the {@link ChannelPromise} to notify once the operation completes
     * @throws Exception        thrown if an error accour
     */
    typedef boost::function<void (ChannelHandlerContext&,
                                  const InetAddress&,
                                  const InetAddress&,
                                  const ChannelFuturePtr&)> ConnectFunctor;

    /**
     * Called once a disconnect operation is made.
     *
     * @param ctx               the {@link ChannelHandlerContext} for which the disconnect operation is made
     * @param promise           the {@link ChannelPromise} to notify once the operation completes
     * @throws Exception        thrown if an error accour
     */
    typedef boost::function<void (ChannelHandlerContext&,
                                  const ChannelFuturePtr&)> DisconnectFunctor;

    /**
     * Called once a close operation is made.
     *
     * @param ctx               the {@link ChannelHandlerContext} for which the close operation is made
     * @param promise           the {@link ChannelPromise} to notify once the operation completes
     * @throws Exception        thrown if an error accour
     */
    typedef boost::function<void (ChannelHandlerContext&,
                                  const ChannelFuturePtr&)> CloseFunctor;

    /**
     * Called once a flush operation is made and so the outbound data should be written.
     *
     * @param ctx               the {@link ChannelHandlerContext} for which the flush operation is made
     * @param promise           the {@link ChannelPromise} to notify once the operation completes
     * @throws Exception        thrown if an error accour
     */
    typedef boost::function<void (ChannelHandlerContext&,
                                  const ChannelFuturePtr&)> FlushFunctor;

    /**
     * Intercepts {@link ChannelHandlerContext#read()}.
     */
    typedef boost::function<void (ChannelHandlerContext&)> ReadFunctor;

public:
    virtual ~ChannelHandlerContext() {}

    /**
     * use for {@link ChannelPipeline} to initialize this Context when add a new ChannelHandler.
     */
    virtual void initialize(const ChannelPipeline& pipeline);

    /**
     * Returns the {@link Channel} that the {@link ChannelPipeline} belongs to.
     * This method is a shortcut to <tt>getPipeline().getChannel()</tt>.
     * @return ChannelPtr which is NULL, when pipeline has detach the channel.
     */
    ChannelPtr channel() const;

    /**
     * Returns the {@link ChannelPipeline} that the {@link ChannelHandler}
     * belongs to.
     */
    ChannelPipeline& pipeline();

    /**
     * Returns the {@link ChannelPipeline} that the {@link ChannelHandler}
     * belongs to.
     */
    const ChannelPipeline& pipeline() const;

    /**
     * Returns the {@link EventLoop} that the {@link ChannelHandler}
     * belongs to.
     */
    const EventLoopPtr& eventLoop() const;

    /**
     * Returns the name of the {@link ChannelHandler} in the
     * {@link ChannelPipeline}.
     */
    const std::string& name() const;

    ChannelHandlerContext* next() const;
    ChannelHandlerContext* prev() const;

    void setNext(ChannelHandlerContext* ctx);

    void setPrev(ChannelHandlerContext* ctx);

    /**
     * Returns the next outbound context of this context object, excluding this context.
     */
    ChannelHandlerContext* nextOutboundContext() const;

    /**
     * Returns the next inbound context of this context object, excluding this context.
     */
    ChannelHandlerContext* nextInboundContext() const;

    ChannelFuturePtr newFuture();
    ChannelFuturePtr newVoidFuture();
    ChannelFuturePtr newSucceededFuture();
    ChannelFuturePtr newFailedFuture(const ChannelException& cause);

    /**
     * return the string presentation of the context.
     */
    std::string toString() const;

public:
    /**
     *
     */
    template<class T>
    T* inboundMessageContainer() {
        boost::any container = getInboundMessageContainer();

        if (!container.empty()) {
            T** t = boost::any_cast<T*>(&container);

            if (t) {
                return *t;
            }
        }

        return NULL;
    }

    template<class T>
    T* nextInboundMessageContainer() {
        if (next_) {
            return nextInboundMessageContainer<T>(next_);
        }

        return NULL;
    }

    template<class T>
    T* nextInboundMessageContainer(ChannelHandlerContext* ctx) {
        ChannelHandlerContext* context = ctx;

        while (context) {
            T* t = context->inboundMessageContainer<T>();

            if (t) {
                return t;
            }

            context = context->next_;
        }

        return NULL;
    }

    /**
     *
     */
    template<class T>
    T* outboundMessageContainer() {
        boost::any container = getOutboundMessageContainer();

        if (!container.empty()) {
            T** t = boost::any_cast<T*>(&container);

            if (t) {
                return *t;
            }
        }

        return NULL;
    }

    /**
     *
     */
    template<class T>
    T* nextOutboundMessageContainer() {
        if (prev_) {
            return nextOutboundMessageContainer<T>(prev_);
        }

        return NULL;
    }

    /**
     *
     */
    template<class T>
    T* nextOutboundMessageContainer(ChannelHandlerContext* ctx) {
        ChannelHandlerContext* context = ctx;

        while (context) {
            T* t = context->outboundMessageContainer<T>();

            if (t) {
                return t;
            }

            context = context->prev_;
        }

        return NULL;
    }

public:
    const BeforeAddCallback& beforeAddCallback() const;
    const AfterAddCallback& afterAddCallback() const;
    const BeforeRemoveCallback& beforeRemoveCallback() const;
    const AfterRemoveCallback& afterRemoveCallback() const;
    const PipelineChangedCallback& pipelineChangedCallback() const;
    const ChannelExceptionCallback& exceptionCallback() const;
    const UserEventCallback& userEventCallback() const;
    const ChannelOpenCallback& channelOpenCallback() const;
    const ChannelActiveCallback& channelActiveCallback() const;
    const ChannelInactiveCallback& channelInactiveCallback() const;
    const ChannelMessageUpdatedCallback& channelMessageUpdatedCallback() const;
    const ChannelReadSuspendedCallback& channelReadSuspendedCallback() const;
    const BindFunctor& bindFunctor() const;
    const ConnectFunctor& connectFunctor() const;
    const DisconnectFunctor& disconnectFunctor() const;
    const CloseFunctor& closeFunctor() const;
    const FlushFunctor& flushFunctor() const;
    const ReadFunctor& readFunctor() const;

    void setBeforeAddCallback(const BeforeAddCallback& beforeAdd);
    void setAfterAddCallback(const AfterAddCallback& afterAdd);
    void setBeforeRemoveCallback(const BeforeRemoveCallback& beforeRemove);
    void setAfterRemoveCallback(const AfterRemoveCallback& afterRemove);
    void setPipelineChangedCallback(const PipelineChangedCallback& pipelineChanged);

    void setExceptionCallback(const ChannelExceptionCallback& exception);
    void setUserEventCallback(const UserEventCallback& userEvent);

    void setChannelOpenCallback(const ChannelOpenCallback& channelOpen);
    void setChannelActiveCallback(const ChannelActiveCallback& channelActive);
    void setChannelInactiveCallback(const ChannelInactiveCallback& channelInactive);
    void setChannelMessageUpdatedCallback(const ChannelMessageUpdatedCallback& messageUpdated);
    void setChannelReadSuspendedCallback(const ChannelReadSuspendedCallback& readSuspended);
    void setBindFunctor(const BindFunctor& bindFun);
    void setConnectFunctor(const ConnectFunctor& connectFun);
    void setDisconnectFunctor(const DisconnectFunctor& disconnectFun);
    void setCloseFunctor(const CloseFunctor& closeFun);
    void setFlushFunctor(const FlushFunctor& flushFun);
    void setReadFunctor(const ReadFunctor& readFun);

public:
    /**
     * A {@link Channel} was open.
     *
     * This will result in having the {@link ChannelOpenCallback} invoked
     * of the next {@link ChannelHandler} contained in the {@link ChannelPipeline}
     * of the {@link Channel}.
     */
    void fireChannelOpen();

    /**
     * A {@link Channel} is active now, which means it is connected or binded.
     *
     * This will result in having the  {@link ChannelActiveCallback} invoked
     * called of the next {@link ChannelHandler} contained in the {@link ChannelPipeline}
     * of the {@link Channel}.
     */
    void fireChannelActive();

    /**
     * A {@link Channel} is inactive now, which means it is closed.
     *
     * This will result in having the  {@link ChannelStateHandler#channelInactive(ChannelHandlerContext)} method
     * called of the next  {@link ChannelStateHandler} contained in the  {@link ChannelPipeline} of the
     * {@link Channel}.
     */
    void fireChannelInactive();

    /**
     * A {@link Channel} received an {@link Throwable} in one of its inbound operations.
     *
     * This will result in having the  {@link ChannelStateHandler#exceptionCaught(ChannelHandlerContext, Throwable)}
     * method  called of the next  {@link ChannelStateHandler} contained in the  {@link ChannelPipeline} of the
     * {@link Channel}.
     */
    void fireExceptionCaught(const ChannelException& cause);

    /**
     * A {@link Channel} received an user defined event.
     *
     * This will result in having the  {@link ChannelStateHandler#userEventTriggered(ChannelHandlerContext, Object)}
     * method  called of the next  {@link ChannelStateHandler} contained in the  {@link ChannelPipeline} of the
     * {@link Channel}.
     */
    void fireUserEventTriggered(const boost::any& evt);

    /**
     * A {@link Channel} received bytes which are now ready to read from its inbound buffer.
     *
     * This will result in having the  {@link ChannelStateHandler#inboundBufferUpdated(ChannelHandlerContext)}
     * method  called of the next  {@link ChannelStateHandler} contained in the  {@link ChannelPipeline} of the
     * {@link Channel}.
     */
    void fireMessageUpdated();

    /**
     * Triggers an {@link ChannelStateHandler#channelReadSuspended(ChannelHandlerContext) channelReadSuspended}
     * event to the next {@link ChannelHandler} in the {@link ChannelPipeline}.
     *
     * @remark If the channel option is autoread, then this event will not triggered.
     */
    void fireChannelReadSuspended();

    /**
     * Request to bind to the given {@link SocketAddress} and notify the {@link ChannelFuture} once the operation
     * completes, either because the operation was successful or because of an error.
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
                                    const InetAddress& localAddress,
                                    const ChannelFuturePtr& future);

    /**
     * Request to disconnect from the remote peer and notify the {@link ChannelFuture} once the operation completes,
     * either because the operation was successful or because of an error.
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
     * <p>
     * This will result in having the
     * {@link ChannelOperationHandler#close(ChannelHandlerContext, ChannelPromise)}
     * method called of the next {@link ChannelOperationHandler} contained in the  {@link ChannelPipeline} of the
     * {@link Channel}.
     */
    const ChannelFuturePtr& close();
    const ChannelFuturePtr& close(const ChannelFuturePtr& future);

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
    const ChannelFuturePtr& flush();

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

public:
    void fireBeforeAdd();
    void fireAfterAdd();
    void fireBeforeRemove();
    void fireAfterRemove();

    void firePipelineChanged();

    void fireChannelOpen(ChannelHandlerContext& ctx);
    void fireChannelActive(ChannelHandlerContext& ctx);
    void fireChannelInactive(ChannelHandlerContext& ctx);
    void fireMessageUpdated(ChannelHandlerContext& ctx);
    void fireChannelReadSuspended(ChannelHandlerContext& ctx);
    void fireExceptionCaught(ChannelHandlerContext& ctx,
        const ChannelException& cause);

    void fireUserEventTriggered(ChannelHandlerContext& ctx,
        const boost::any& evt);

    const ChannelFuturePtr& bind(ChannelHandlerContext& ctx,
                                 const InetAddress& localAddress,
                                 const ChannelFuturePtr& future);

    const ChannelFuturePtr& connect(ChannelHandlerContext& ctx,
                                    const InetAddress& remoteAddress,
                                    const InetAddress& localAddress,
                                    const ChannelFuturePtr& future);

    const ChannelFuturePtr& disconnect(ChannelHandlerContext& ctx,
                                       const ChannelFuturePtr& future);

    const ChannelFuturePtr& close(ChannelHandlerContext& ctx,
                                  const ChannelFuturePtr& future);

    const ChannelFuturePtr& flush(ChannelHandlerContext& ctx,
                                  const ChannelFuturePtr& future);

    void read(ChannelHandlerContext& ctx);

protected:
    ChannelHandlerContext(const std::string& name);

    ChannelHandlerContext(const std::string& name,
                          const EventLoopPtr& eventLoop);

    /**
     * return the inbound message container of the context.
     */
    virtual boost::any getInboundMessageContainer() = 0;

    /**
     *
     */
    virtual boost::any getOutboundMessageContainer() = 0;

    virtual void onPipelineChanged();

private:
    void resetNeighbourContexts();
    void notifyHandlerException(const ChannelPipelineException& e);

    void clearOutboundChannelBuffer(ChannelHandlerContext& ctx);
    void clearInboundChannelBuffer(ChannelHandlerContext& ctx);

private:
    BeforeAddCallback beforeAddCallback_;
    AfterAddCallback afterAddCallback_;
    BeforeRemoveCallback beforeRemoveCallback_;
    AfterRemoveCallback afterRemoveCallback_;
    PipelineChangedCallback pipelineChangedCallback_;

    ChannelExceptionCallback exceptionCallback_;
    UserEventCallback userEventCallback_;

    ChannelOpenCallback channelOpenCallback_;
    ChannelActiveCallback channelActiveCallback_;
    ChannelInactiveCallback channelInactiveCallback_;
    ChannelMessageUpdatedCallback channelMessageUpdatedCallback_;
    ChannelReadSuspendedCallback channelReadSuspendedCallback_;

    BindFunctor bindFunctor_;
    ConnectFunctor connectFunctor_;
    DisconnectFunctor disconnectFunctor_;
    CloseFunctor closeFunctor_;
    FlushFunctor flushFunctor_;
    ReadFunctor readFunctor_;

private:
    enum EventType {
        EVT_CHANNEL_OPEN = 0,
        EVT_CHANNEL_ACTIVE,
        EVT_CHANNEL_INACTIVE,
        EVT_CHANNEL_EXCEPTION,
        EVT_CHANNEL_READ_SUSPENDED,
        EVT_CHANNEL_USR_EVENT,
        EVT_CHANNEL_MESSAGE_UPDATED,
        EVT_CHANNEL_BIND,
        EVT_CHANNEL_CONNECT,
        EVT_CHANNEL_DISCONNECT,
        EVT_CHANNEL_CLOSE,
        EVT_CHANNEL_FLUSH,
        EVT_CHANNEL_READ,
        EVT_MAX
    };

private:
    std::string name_;

    ChannelHandlerContext* next_;
    ChannelHandlerContext* prev_;

    bool hasNeighbourEventHandler_[EVT_MAX];
    ChannelHandlerContext* neighbourEventHandler_[EVT_MAX];

    EventLoopPtr eventLoop_;
    ChannelWeakPtr channel_;
    const ChannelPipeline* pipeline_;
};

inline
ChannelPtr ChannelHandlerContext::channel() const {
    return channel_.lock();
}

inline
const EventLoopPtr& ChannelHandlerContext::eventLoop() const {
    return eventLoop_;
}

inline
const std::string& ChannelHandlerContext::name() const {
    return name_;
}

inline
ChannelHandlerContext* ChannelHandlerContext::next() const {
    return next_;
}

inline
ChannelHandlerContext* ChannelHandlerContext::prev() const {
    return prev_;
}

inline
void ChannelHandlerContext::setNext(ChannelHandlerContext* ctx) {
    next_ = ctx;
}

inline
void ChannelHandlerContext::setPrev(ChannelHandlerContext* ctx) {
    prev_ = ctx;
}

inline
ChannelHandlerContext* ChannelHandlerContext::nextOutboundContext() const {
    return prev();
}

inline
ChannelHandlerContext* ChannelHandlerContext::nextInboundContext() const {
    return next();
}

inline
std::string ChannelHandlerContext::toString() const {
    return name_;
}

inline
const ChannelHandlerContext::BeforeAddCallback&
ChannelHandlerContext::beforeAddCallback() const {
    return beforeAddCallback_;
}

inline
const ChannelHandlerContext::AfterAddCallback&
ChannelHandlerContext::afterAddCallback() const {
    return afterAddCallback_;
}

inline
const ChannelHandlerContext::BeforeRemoveCallback&
ChannelHandlerContext::beforeRemoveCallback() const {
    return beforeRemoveCallback_;
}

inline
const ChannelHandlerContext::AfterRemoveCallback&
ChannelHandlerContext::afterRemoveCallback() const {
    return afterRemoveCallback_;
}

inline
const ChannelHandlerContext::PipelineChangedCallback&
ChannelHandlerContext::pipelineChangedCallback() const {
    return pipelineChangedCallback_;
}

inline
const ChannelHandlerContext::ChannelExceptionCallback&
ChannelHandlerContext::exceptionCallback() const {
    return exceptionCallback_;
}

inline
const ChannelHandlerContext::UserEventCallback&
ChannelHandlerContext::userEventCallback() const {
    return userEventCallback_;
}

inline
const ChannelHandlerContext::ChannelOpenCallback&
ChannelHandlerContext::channelOpenCallback() const {
    return channelOpenCallback_;
}

inline
const ChannelHandlerContext::ChannelActiveCallback&
ChannelHandlerContext::channelActiveCallback() const {
    return channelActiveCallback_;
}

inline
const ChannelHandlerContext::ChannelInactiveCallback&
ChannelHandlerContext::channelInactiveCallback() const {
    return channelInactiveCallback_;
}

inline
const ChannelHandlerContext::ChannelMessageUpdatedCallback&
ChannelHandlerContext::channelMessageUpdatedCallback() const {
    return channelMessageUpdatedCallback_;
}

inline
const ChannelHandlerContext::ChannelReadSuspendedCallback&
ChannelHandlerContext::channelReadSuspendedCallback() const {
    return channelReadSuspendedCallback_;
}

inline
const ChannelHandlerContext::BindFunctor&
ChannelHandlerContext::bindFunctor() const {
    return bindFunctor_;
}

inline
const ChannelHandlerContext::ConnectFunctor&
ChannelHandlerContext::connectFunctor() const {
    return connectFunctor_;
}

inline
const ChannelHandlerContext::DisconnectFunctor&
ChannelHandlerContext::disconnectFunctor() const {
    return disconnectFunctor_;
}

inline
const ChannelHandlerContext::CloseFunctor&
ChannelHandlerContext::closeFunctor() const {
    return closeFunctor_;
}

inline
const ChannelHandlerContext::FlushFunctor&
ChannelHandlerContext::flushFunctor() const {
    return flushFunctor_;
}

inline
const ChannelHandlerContext::ReadFunctor&
ChannelHandlerContext::readFunctor() const {
    return readFunctor_;
}

inline
void ChannelHandlerContext::setBeforeAddCallback(
    const ChannelHandlerContext::BeforeAddCallback& beforeAdd) {
    beforeAddCallback_ = beforeAdd;
}

inline
void ChannelHandlerContext::setAfterAddCallback(
    const ChannelHandlerContext::AfterAddCallback& afterAdd) {
    afterAddCallback_ = afterAdd;
}

inline
void ChannelHandlerContext::setBeforeRemoveCallback(
    const ChannelHandlerContext::BeforeRemoveCallback& beforeRemove) {
    beforeRemoveCallback_ = beforeRemove;
}

inline
void ChannelHandlerContext::setAfterRemoveCallback(
    const ChannelHandlerContext::AfterRemoveCallback& afterRemove) {
    afterRemoveCallback_ = afterRemove;
}

inline
void ChannelHandlerContext::setPipelineChangedCallback(
    const ChannelHandlerContext::PipelineChangedCallback& pipelineChanged) {
    pipelineChangedCallback_ = pipelineChanged;
}

inline
void ChannelHandlerContext::setExceptionCallback(
    const ChannelHandlerContext::ChannelExceptionCallback& exception) {
    exceptionCallback_ = exception;
}

inline
void ChannelHandlerContext::setUserEventCallback(
    const ChannelHandlerContext::UserEventCallback& userEvent) {
    userEventCallback_ = userEvent;
}

inline
void ChannelHandlerContext::setChannelOpenCallback(
    const ChannelHandlerContext::ChannelOpenCallback& channelOpen) {
    channelOpenCallback_ = channelOpen;
}

inline
void ChannelHandlerContext::setChannelActiveCallback(
    const ChannelHandlerContext::ChannelActiveCallback& channelActive) {
    channelActiveCallback_ = channelActive;
}

inline
void ChannelHandlerContext::setChannelInactiveCallback(
    const ChannelHandlerContext::ChannelInactiveCallback& channelInactive) {
    channelInactiveCallback_ = channelInactive;
}

inline
void ChannelHandlerContext::setChannelMessageUpdatedCallback(
    const ChannelHandlerContext::ChannelMessageUpdatedCallback& messageUpdated) {
    channelMessageUpdatedCallback_ = messageUpdated;
}

inline
void ChannelHandlerContext::setChannelReadSuspendedCallback(
    const ChannelHandlerContext::ChannelReadSuspendedCallback& readSuspended) {
    channelReadSuspendedCallback_ = readSuspended;
}

inline
void ChannelHandlerContext::setBindFunctor(
    const ChannelHandlerContext::BindFunctor& bindFun) {
    bindFunctor_ = bindFun;
}

inline
void ChannelHandlerContext::setConnectFunctor(
    const ChannelHandlerContext::ConnectFunctor& connectFun) {
    connectFunctor_ = connectFun;
}

inline
void ChannelHandlerContext::setDisconnectFunctor(
    const ChannelHandlerContext::DisconnectFunctor& disconnectFun) {
    disconnectFunctor_ = disconnectFun;
}

inline
void ChannelHandlerContext::setCloseFunctor(
    const ChannelHandlerContext::CloseFunctor& closeFun) {
    closeFunctor_ = closeFun;
}

inline
void ChannelHandlerContext::setFlushFunctor(
    const ChannelHandlerContext::FlushFunctor& flushFun) {
    flushFunctor_ = flushFun;
}

inline
void ChannelHandlerContext::setReadFunctor(
    const ChannelHandlerContext::ReadFunctor& readFun) {
    readFunctor_ = readFun;
}

inline
void ChannelHandlerContext::fireBeforeAdd() {
    if (beforeAddCallback_) { beforeAddCallback_(*this); }
}

inline
void ChannelHandlerContext::fireAfterAdd() {
    if (afterAddCallback_) { afterAddCallback_(*this); }
}

inline
void ChannelHandlerContext::fireBeforeRemove() {
    if (beforeRemoveCallback_) { beforeRemoveCallback_(*this); }
}

inline
void ChannelHandlerContext::fireAfterRemove() {
    if (afterRemoveCallback_) {
        afterRemoveCallback_(*this);
    }
}

inline
void ChannelHandlerContext::firePipelineChanged() {
    onPipelineChanged();

    if (pipelineChangedCallback_) {
        pipelineChangedCallback_(*this);
    }
}

inline
void ChannelHandlerContext::resetNeighbourContexts() {
    for (int i = 0; i < EVT_MAX; ++i) {
        neighbourEventHandler_[i] = NULL;
        hasNeighbourEventHandler_[i] = true;
    }
}

}
}

#endif //#if !defined(CETTY_CHANNEL_CHANNELHANDLERCONTEXT_H)

// Local Variables:
// mode: c++
// End:
