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

#include <boost/any.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include <cetty/channel/EventLoop.h>
#include <cetty/channel/ChannelPtr.h>
#include <cetty/channel/ChannelFuture.h>
#include <cetty/channel/SocketAddress.h>
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
    typedef boost::function<void (ChannelHandlerContext&)> BeforeAddCallback;

    typedef boost::function<void (ChannelHandlerContext&)> AfterAddCallback;

    typedef boost::function<void (ChannelHandlerContext&)> BeforeRemoveCallback;

    typedef boost::function<void (ChannelHandlerContext&)> AfterRemoveCallback;

    typedef boost::function<void (ChannelHandlerContext&,
                                  const ChannelException&)> ExceptionCallback;

    typedef boost::function<void (ChannelHandlerContext&,
                                  const boost::any&)> UserEventCallback;

    typedef boost::function<void (ChannelHandlerContext&)> ChannelOpenCallback;

    typedef boost::function<void (ChannelHandlerContext&)> ChannelActiveCallback;

    typedef boost::function<void (ChannelHandlerContext&)> ChannelInactiveCallback;

    typedef boost::function<void (ChannelHandlerContext&)> ChannelMessageUpdatedCallback;

    typedef boost::function<void (ChannelHandlerContext&,
                                  const SocketAddress&,
                                  const ChannelFuturePtr&)> BindFunctor;

    typedef boost::function<void (ChannelHandlerContext&,
                                  const SocketAddress&,
                                  const SocketAddress&,
                                  const ChannelFuturePtr&)> ConnectFunctor;

    typedef boost::function<void (ChannelHandlerContext&,
                                  const ChannelFuturePtr&)> DisconnectFunctor;

    typedef boost::function<void (ChannelHandlerContext&,
                                  const ChannelFuturePtr&)> CloseFunctor;

    typedef boost::function<void (ChannelHandlerContext&,
                                  const ChannelFuturePtr&)> FlushFunctor;

public:
    virtual ~ChannelHandlerContext() {}

    /**
     * Returns the {@link Channel} that the {@link ChannelPipeline} belongs to.
     * This method is a shortcut to <tt>getPipeline().getChannel()</tt>.
     * @return ChannelPtr which is NULL, when pipeline has detach the channel.
     */
    const ChannelPtr& channel() const;

    /**
     * Returns the {@link ChannelPipeline} that the {@link ChannelHandler}
     * belongs to.
     */
    const ChannelPipeline& pipeline() const;

    /**
     * Returns the {@link ChannelPipeline} that the {@link ChannelHandler}
     * belongs to.
     */
    ChannelPipeline& pipeline();

    void setPipeline(ChannelPipeline* pipeline);

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
    ChannelHandlerContext* before() const;

    virtual void setNext(ChannelHandlerContext* ctx);
    virtual void setBefore(ChannelHandlerContext* ctx);

    /**
     * Returns the next outbound context of this context object, excluding this context.
     */
    ChannelHandlerContext* nextOutboundContext() const {
        return before();
    }

    /**
     * Returns the next inbound context of this context object, excluding this context.
     */
    ChannelHandlerContext* nextInboundContext() const {
        return next();
    }

    virtual void attach();
    void detach();

    std::string toString() const { return name_; }

public:
    virtual boost::any getInboundMessageContainer() = 0;
    virtual boost::any getOutboundMessageContainer() = 0;

    template<class T>
    T* inboundMessageContainer() {
        boost::any& container = getInboundMessageContainer();
        return boost::any_cast<T>(&container);
    }

    template<class T>
    T* nextInboundMessageContainer() {

    }

    template<class T>
    T* nextInboundMessageContainer(ChannelHandlerContext* ctx) {

    }

    template<class T>
    T* outboundMessageContainer() {
        boost::any& container = getOutboundMessageContainer();
        return boost::any_cast<T>(&container);
    }

    template<class T>
    T* nextOutboundMessageContainer() {

    }

    template<class T>
    T* nextOutboundMessageContainer(ChannelHandlerContext* ctx) {

    }

public:
    const BeforeAddCallback& beforeAddCallback() const { return beforeAddCallback_; }
    const AfterAddCallback& afterAddCallback() const { return afterAddCallback_; }
    const BeforeRemoveCallback& beforeRemoveCallback() const { return beforeRemoveCallback_; }
    const AfterRemoveCallback& afterRemoveCallback() const { return afterRemoveCallback_; }
    const ExceptionCallback& exceptionCallback() const { return exceptionCallback_; }
    const UserEventCallback& userEventCallback() const { return userEventCallback_; }
    const ChannelOpenCallback& channelOpenCallback() const { return channelOpenCallback_; }
    const ChannelActiveCallback& channelActiveCallback() const { return channelActiveCallback_; }
    const ChannelInactiveCallback& channelInactiveCallback() const { return channelInactiveCallback_; }
    const ChannelMessageUpdatedCallback& channelMessageUpdatedCallback() const { return channelMessageUpdatedCallback_; }
    const BindFunctor& bindFunctor() const { return bindFunctor_; }
    const ConnectFunctor& connectFunctor() const { return connectFunctor_; }
    const DisconnectFunctor& disconnectFunctor() const { return disconnectFunctor_; }
    const CloseFunctor& closeFunctor() const { return closeFunctor_; }
    const FlushFunctor& flushFunctor() const { return flushFunctor_; }

    void setBeforeAddCallback(const BeforeAddCallback& beforeAdd) { beforeAddCallback_ = beforeAdd; }
    void setAfterAddCallback(const AfterAddCallback& afterAdd) { afterAddCallback_ = afterAdd; }
    void setBeforeRemoveCallback(const BeforeRemoveCallback& beforeRemove) { beforeRemoveCallback_ = beforeRemove; }
    void setAfterRemoveCallback(const AfterRemoveCallback& afterRemove) { afterRemoveCallback_ = afterRemove; }

    void setExceptionCallback(const ExceptionCallback& exception) { exceptionCallback_ = exception; }
    void setUserEventCallback(const UserEventCallback& userEvent) { userEventCallback_ = userEvent; }

    void setChannelOpenCallback(const ChannelOpenCallback& channelOpen) { channelOpenCallback_ = channelOpen; }
    void setChannelActiveCallback(const ChannelActiveCallback& channelActive) { channelActiveCallback_ = channelActive; }
    void setChannelInactiveCallback(const ChannelInactiveCallback& channelInactive) { channelInactiveCallback_ = channelInactive; }
    void setChannelMessageUpdatedCallback(const ChannelMessageUpdatedCallback& messageUpdated) { channelMessageUpdatedCallback_ = messageUpdated; }

    void setBindFunctor(const BindFunctor& bindFun) { bindFunctor_ = bindFun; }
    void setConnectFunctor(const ConnectFunctor& connectFun) { connectFunctor_ = connectFun; }
    void setDisconnectFunctor(const DisconnectFunctor& disconnectFun) { disconnectFunctor_ = disconnectFun; }
    void setCloseFunctor(const CloseFunctor& closeFun) { closeFunctor_ = closeFun; }
    void setFlushFunctor(const FlushFunctor& flushFun) { flushFunctor_ = flushFun; }

public:
    void fireChannelOpen();
    void fireChannelActive();
    void fireChannelInactive();
    void fireExceptionCaught(const ChannelException& cause);
    void fireUserEventTriggered(const boost::any& evt);
    void fireMessageUpdated();

    const ChannelFuturePtr& bind(const SocketAddress& localAddress,
                                 const ChannelFuturePtr& future);

    const ChannelFuturePtr& connect(const SocketAddress& remoteAddress,
                                    const SocketAddress& localAddress,
                                    const ChannelFuturePtr& future);

    const ChannelFuturePtr& disconnect(const ChannelFuturePtr& future);

    const ChannelFuturePtr& close();
    const ChannelFuturePtr& close(const ChannelFuturePtr& future);

    const ChannelFuturePtr& flush();
    const ChannelFuturePtr& flush(const ChannelFuturePtr& future);

public:
    void fireBeforeAdd() { if (beforeAddCallback_) { beforeAddCallback_(*this); } }
    void fireAfterAdd() { if (afterAddCallback_) { afterAddCallback_(*this); } }
    void fireBeforeRemove() { if (beforeRemoveCallback_) { beforeRemoveCallback_(*this); } }
    void fireAfterRemove() { if (afterRemoveCallback_) { afterRemoveCallback_(*this); } }

    void fireChannelOpen(ChannelHandlerContext& ctx);
    void fireChannelActive(ChannelHandlerContext& ctx);
    void fireChannelInactive(ChannelHandlerContext& ctx);
    void fireMessageUpdated(ChannelHandlerContext& ctx);
    void fireExceptionCaught(ChannelHandlerContext& ctx, const ChannelException& cause);
    void fireUserEventTriggered(ChannelHandlerContext& ctx, const boost::any& evt);

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
    ChannelHandlerContext(const std::string& name);

    ChannelHandlerContext(const std::string& name,
                          const EventLoopPtr& eventLoop);

    

private:
    void init();

    ChannelFuturePtr newFuture();
    ChannelFuturePtr newSucceededFuture();
    ChannelFuturePtr newFailedFuture(const ChannelException& cause);

    void notifyHandlerException(const ChannelPipelineException& e);

    void clearOutboundChannelBuffer(ChannelHandlerContext& ctx);
    void clearInboundChannelBuffer(ChannelHandlerContext& ctx);

private:
    BeforeAddCallback beforeAddCallback_;
    AfterAddCallback afterAddCallback_;
    BeforeRemoveCallback beforeRemoveCallback_;
    AfterRemoveCallback afterRemoveCallback_;

    ExceptionCallback exceptionCallback_;
    UserEventCallback userEventCallback_;

    ChannelOpenCallback channelOpenCallback_;
    ChannelActiveCallback channelActiveCallback_;
    ChannelInactiveCallback channelInactiveCallback_;
    ChannelMessageUpdatedCallback channelMessageUpdatedCallback_;

    BindFunctor bindFunctor_;
    ConnectFunctor connectFunctor_;
    DisconnectFunctor disconnectFunctor_;
    CloseFunctor closeFunctor_;
    FlushFunctor flushFunctor_;

private:
    std::string name_;

    ChannelHandlerContext* next_;
    ChannelHandlerContext* before_;

    ChannelPipeline* pipeline_;
    EventLoopPtr eventLoop_;
};

}
}

#endif //#if !defined(CETTY_CHANNEL_CHANNELHANDLERCONTEXT_H)

// Local Variables:
// mode: c++
// End:
