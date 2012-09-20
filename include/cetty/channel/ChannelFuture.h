#if !defined(CETTY_CHANNEL_CHANNELFUTURE_H)
#define CETTY_CHANNEL_CHANNELFUTURE_H

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

#include <cetty/Types.h>
#include <boost/function.hpp>
#include <cetty/util/ReferenceCounter.h>
#include <cetty/channel/ChannelPtr.h>
#include <cetty/channel/ChannelFuturePtr.h>

namespace cetty {
namespace util {
class Exception;
}
}

namespace cetty {
namespace channel {

using namespace cetty::util;

/**
 * The result of an asynchronous {@link Channel} I/O operation.
 * <p>
 * All I/O operations in Cetty are asynchronous.  It means any I/O calls will
 * return immediately with no guarantee that the requested I/O operation has
 * been completed at the end of the call.  Instead, you will be returned with
 * a {@link ChannelFuture} instance which gives you the information about the
 * result or status of the I/O operation.
 * </p>
 * <p>
 * A {@link ChannelFuture} is either <em>uncompleted</em> or <em>completed</em>.
 * When an I/O operation begins, a new future object is created.  The new future
 * is uncompleted initially - it is neither succeeded, failed, nor cancelled
 * because the I/O operation is not finished yet.  If the I/O operation is
 * finished either successfully, with failure, or by cancellation, the future is
 * marked as completed with more specific information, such as the cause of the
 * failure.  Please note that even failure and cancellation belong to the
 * completed state.</p>
 * <pre>
 *                                      +---------------------------+
 *                                      | Completed successfully    |
 *                                      +---------------------------+
 *                                 +---->      isDone() = <b>true</b>      |
 * +--------------------------+    |    |   isSuccess() = <b>true</b>      |
 * |        Uncompleted       |    |    +===========================+
 * +--------------------------+    |    | Completed with failure    |
 * |      isDone() = <b>false</b>    |    |    +---------------------------+
 * |   isSuccess() = <b>false</b>    |----+---->   isDone() = <b>true</b>         |
 * | isCancelled() = <b>false</b>    |    |    | getCause() = <b>non-NULL</b>     |
 * |    getCause() = NULL     |    |    +===========================+
 * +--------------------------+    |    | Completed by cancellation |
 *                                 |    +---------------------------+
 *                                 +---->      isDone() = <b>true</b>      |
 *                                      | isCancelled() = <b>true</b>      |
 *                                      +---------------------------+
 * </pre>
 *
 * Various methods are provided to let you check if the I/O operation has been
 * completed, wait for the completion, and retrieve the result of the I/O
 * operation. It also allows you to add {@link ChannelFutureListener}s so you
 * can get notified when the I/O operation is completed.
 *
 * <h3>Prefer {@link #addListener(ChannelFutureListener*)} to {@link #await()}</h3>
 *
 * It is recommended to prefer {@link #addListener(ChannelFutureListener*)} to
 * {@link #await()} wherever possible to get notified when an I/O operation is
 * done and to do any follow-up tasks.
 * <p>
 * {@link #addListener(ChannelFutureListener*)} is non-blocking.  It simply adds
 * the specified {@link ChannelFutureListener} to the {@link ChannelFuture}, and
 * I/O thread will notify the listeners when the I/O operation associated with
 * the future is done.  {@link ChannelFutureListener} yields the best
 * performance and resource utilization because it does not block at all, but
 * it could be tricky to implement a sequential logic if you are not used to
 * event-driven programming.
 * <p>
 * By contrast, {@link #await()} is a blocking operation.  Once called, the
 * caller thread blocks until the operation is done.  It is easier to implement
 * a sequential logic with {@link #await()}, but the caller thread blocks
 * unnecessarily until the I/O operation is done and there's relatively
 * expensive cost of inter-thread notification.  Moreover, there's a chance of
 * dead lock in a particular circumstance, which is described below.
 *
 * <h3>Do not call {@link #await()} inside {@link ChannelHandler}</h3>
 * <p>
 * The event handler methods in {@link ChannelHandler} is often called by
 * an I/O thread unless an {@link ExecutionHandler} is in the
 * {@link ChannelPipeline}.  If {@link #await()} is called by an event handler
 * method, which is called by the I/O thread, the I/O operation it is waiting
 * for might never be complete because {@link #await()} can block the I/O
 * operation it is waiting for, which is a dead lock.
 * <pre>
 * // BAD - NEVER DO THIS
 * void messageReceived(ChannelHandlerContext& ctx, const MessageEvent& e) {
 *     GoodByeMessage* msg = e.getMessage().pointer<GoodByeMessage>();
 *     if (msg) {
 *         {@link ChannelFuture ChannelFuturePtr} future = e.getChannel().close();
 *         future->awaitUninterruptibly();
 *         // Perform post-closure operation
 *         // ...
 *     }
 * }
 *
 * // GOOD
 * class MyChannelFutureListener : public cetty::channel::ChannelFutureListener {
 * public:
 *     MyChannelFutureListener() {}
 *     virtual ~MyChannelFutureListener() {}
 *
 *     void operationComplete(const ChannelFuturePtr& future) {
 *         // Perform post-closure operation
 *         // ...
 *     }
 * };
 *
 * void messageReceived(ChannelHandlerContext& ctx, const MessageEvent& e) {
 *     GoodByeMessage* msg = e.getMessage().pointer<GoodByeMessage>();
 *     if (msg) {
 *         {@link ChannelFuture ChannelFuturePtr} future = e.getChannel().close();
 *         future->addListener(new MyChannelFutureListener());
 *     }
 * }
 *
 * // BETTER (no need to manage the listener's life cycle)
 * void handleGoodByeMessage(const ChannelFuturePtr& future) {
 *         // Perform post-closure operation
 *         // ...
 * }
 *
 * void messageReceived(ChannelHandlerContext& ctx, const MessageEvent& e) {
 *     GoodByeMessage* msg = e.getMessage().pointer<GoodByeMessage>();
 *     if (msg) {
 *         const {@link ChannelFuture ChannelFuturePtr} &future = e.getChannel().close();
 *         future->setListener(boost::bind(handleGoodByeMessage, future));
 *     }
 * }
 *
 * </pre>
 * <p>
 * In spite of the disadvantages mentioned above, there are certainly the cases
 * where it is more convenient to call {@link #await()}. In such a case, please
 * make sure you do not call {@link #await()} in an I/O thread.  Otherwise,
 * {@link IllegalStateException} will be raised to prevent a dead lock.
 *
 * <h3>Do not confuse I/O timeout and await timeout</h3>
 *
 * The timeout value you specify with {@link #await(int64_t)},
 * {@link #await(int64_t, const TimeUnit&)},
 * {@link #awaitUninterruptibly(int64_t)}, or
 * {@link #awaitUninterruptibly(int64_t, const TimeUnit&)} are not related with I/O
 * timeout at all.  If an I/O operation times out, the future will be marked as
 * 'completed with failure,' as depicted in the diagram above.  For example,
 * connect timeout should be configured via a transport-specific option:
 * <pre>
 * // BAD - NEVER DO THIS
 * {@link ClientBootstrap} b = ...;
 * {@link ChannelFuture ChannelFuturePtr} f = b.connect(...);
 * f->awaitUninterruptibly(10, TimeUnit::SECONDS);
 * if (f->isCancelled()) {
 *     // Connection attempt cancelled by user
 * }
 * else if (!f->isSuccess()) {
 *     // You might get a NullPointerException here because the future
 *     // might not be completed yet.
 *     // f->getCause().printStackTrace();
 * }
 * else {
 *     // Connection established successfully
 * }
 *
 * // GOOD
 * {@link ClientBootstrap} b = ...;
 * // Configure the connect timeout option.
 * <b>b.setOption("connectTimeoutMillis", 10000);</b>
 * {@link ChannelFuture ChannelFuturePtr} f = b.connect(...);
 * f->awaitUninterruptibly();
 *
 * // Now we are sure the future is completed.
 * assert f->isDone();
 *
 * if (f->isCancelled()) {
 *     // Connection attempt cancelled by user
 * }
 * else if (!f->isSuccess()) {
 *     //f->getCause().printStackTrace();
 * }
 * else {
 *     // Connection established successfully
 * }
 * </pre>
 *
 *
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 * @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
 *
 */

class ChannelFuture : public cetty::util::ReferenceCounter<ChannelFuture> {
public:
/**
 * Listens to the result of a {@link ChannelFuture}.  The result of the
 * asynchronous {@link Channel} I/O operation is notified once this listener
 * is added by calling {@link ChannelFuture#addListener(ChannelFutureListener)}.
 *
 * <h3>Return the control to the caller quickly</h3>
 *
 * {@link #operationComplete(ChannelFuture)} is directly called by an I/O
 * thread.  Therefore, performing a time consuming task or a blocking operation
 * in the handler method can cause an unexpected pause during I/O.  If you need
 * to perform a blocking operation on I/O completion, try to execute the
 * operation in a different thread using a thread pool.
     
     * Invoked when the I/O operation associated with the
     * {@link ChannelFuture const ChannelFuturePtr&} has been completed.
     *
     * @param future  the source {@link ChannelFuture} which called this
     *                callback
 */
    typedef boost::function<void (ChannelFuture&)> CompletedCallback;
    typedef boost::function<void (ChannelFuture&, int, int, int)> ProgressedCallback;

public:
    virtual ~ChannelFuture() {}

    /**
     * Returns a channel where the I/O operation associated with this
     * future takes place.
     */
    virtual const ChannelPtr& getChannel() const = 0;

    /**
     * Returns <tt>true</tt> if and only if this future is
     * complete, regardless of whether the operation was successful, failed,
     * or cancelled.
     */
    virtual bool isDone() const = 0;

    /**
     * Returns <tt>true</tt> if and only if this future was
     * cancelled by a {@link #cancel()} method.
     */
    virtual bool isCancelled() const = 0;

    /**
     * Returns <tt>true</tt> if and only if the I/O operation was completed
     * successfully.
     */
    virtual bool isSuccess() const = 0;

    /**
     * Returns the cause of the failed I/O operation if the I/O operation has
     * failed.
     *
     * @return the cause of the failure.
     *         <tt>NULL</tt> if succeeded or this future is not
     *         completed yet.
     */
    virtual const Exception* getCause() const = 0;

    /**
     * Cancels the I/O operation associated with this future
     * and notifies all listeners if canceled successfully.
     *
     * @return <tt>true</tt> if and only if the operation has been canceled.
     *         <tt>false</tt> if the operation can't be canceled or is already
     *         completed.
     */
    virtual bool cancel() = 0;

    /**
     * Marks this future as a success and notifies all
     * listeners.
     *
     * @return <tt>true</tt> if and only if successfully marked this future as
     *         a success. Otherwise <tt>false</tt> because this future is
     *         already marked as either a success or a failure.
     */
    virtual bool setSuccess() = 0;

    /**
     * Marks this future as a failure and notifies all
     * listeners.
     *
     * @return <tt>true</tt> if and only if successfully marked this future as
     *         a failure. Otherwise <tt>false</tt> because this future is
     *         already marked as either a success or a failure.
     */
    virtual bool setFailure(const Exception& cause) = 0;

    /**
     * Notifies the progress of the operation to the listeners that implements
     * {@link ChannelFutureProgressListener}. Please note that this method will
     * not do anything and return <tt>false</tt> if this future is complete
     * already.
     *
     * @return <tt>true</tt> if and only if notification was made.
     */
    virtual bool setProgress(int amount, int current, int total) = 0;

    /**
     * Adds the specified listener, which is a function object, to this future.
     * The specified listener is notified when this future is
     * {@link #isDone() done}.  If this future is already
     * completed, the specified listener is notified immediately.
     *
     * Use this function, you may not have to take care the listener's
     * life circle, otherwise, the ChannelFutureListener should.
     */
    virtual void addListener(const CompletedCallback& listenerr, int priority = 0) = 0;
    virtual void addProgressListener(const ProgressedCallback& listener) = 0;

    /**
     * Adds the specified listener to this future.  The
     * specified listener is notified when this future is
     * {@link #isDone() done}.  If this future is already
     * completed, the specified listener is notified immediately.
     */
    //virtual void addListener(ChannelFutureListener* listener) = 0;

    /**
     * Removes the specified listener from this future.
     * The specified listener is no longer notified when this
     * future is @link #isDone() done@endlink.  If the specified
     * listener is not associated with this future, this method
     * does nothing and returns silently.
     */
    virtual void removeListener(const CompletedCallback& listener) = 0;
    virtual void removeProgressListener(const ProgressedCallback& listener) = 0;

    /**
     * Waits for this future to be completed.
     *
     * @throws InterruptedException
     *         if the current thread was interrupted
     */
    virtual ChannelFuturePtr await() = 0;

    /**
     * Waits for this future to be completed without
     * interruption.  This method catches an {@link InterruptedException} and
     * discards it silently.
     */
    virtual ChannelFuturePtr awaitUninterruptibly() = 0;

    /**
     * Waits for this future to be completed within the
     * specified time limit.
     *
     * @return <tt>true</tt> if and only if the future was completed within
     *         the specified time limit
     *
     * @throws InterruptedException
     *         if the current thread was interrupted
     */
    virtual bool await(int timeoutMillis) = 0;

    /**
     * Waits for this future to be completed within the
     * specified time limit without interruption.  This method catches an
     * {@link InterruptedException} and discards it silently.
     *
     * @return <tt>true</tt> if and only if the future was completed within
     *         the specified time limit
     */
    virtual bool awaitUninterruptibly(int timeoutMillis) = 0;
};

}
}

#endif //#if !defined(CETTY_CHANNEL_CHANNELFUTURE_H)

// Local Variables:
// mode: c++
// End:
