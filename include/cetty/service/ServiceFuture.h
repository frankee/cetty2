#if !defined(CETTY_SERVICE_SERVICEFUTURE_H)
#define CETTY_SERVICE_SERVICEFUTURE_H

/*
 * Copyright (c) 2010-2012 frankee zhou (frankee.zhou at gmail dot com)
 *
 * Distributed under under the Apache License, version 2.0 (the "License").
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 */

#include <vector>
#include <boost/function.hpp>
#include <cetty/util/ReferenceCounter.h>

namespace cetty {
    namespace util {
        class Exception;
    }
}

namespace cetty {
namespace service {

    using cetty::util::Exception;

template <typename T>
class ServiceFuture : public cetty::util::ReferenceCounter<ServiceFuture<T>, int> {
public:
    typedef T ResponseType;
    typedef const T& ResponseConstRefType;
    typedef ServiceFuture<T> SelfType;
    typedef const ServiceFuture<T>& SelfConstRefType;

    typedef typename boost::function2<void, SelfConstRefType, ResponseConstRefType> CompletedCallback;

public:
    ServiceFuture() {}
    ServiceFuture(int timeout) {}
    virtual ~ServiceFuture() {}

    // Resets the RpcController to its initial state so that it may be reused in
    // a new call.  Must not be called while an RPC is in progress.
    //void reset();

    // Advises the RPC system that the caller desires that the RPC call be
    // canceled.  The RPC system may cancel it immediately, may wait awhile and
    // then cancel it, or may not even cancel the call at all.  If the call is
    // canceled, the "done" callback will still be called and the RpcController
    // will indicate that the call failed at that time.
    void startCancel();

    /**
     * Cancels the I/O operation associated with this future
     * and notifies all listeners if canceled successfully.
     *
     * @return <tt>true</tt> if and only if the operation has been canceled.
     *         <tt>false</tt> if the operation can't be canceled or is already
     *         completed.
     */
    //bool cancel();

    /**
     * Returns a channel where the I/O operation associated with this
     * future takes place.
     */
    //virtual const ChannelPtr& getChannel() const = 0;

    /**
     * Returns <tt>true</tt> if and only if this future is
     * complete, regardless of whether the operation was successful, failed,
     * or cancelled.
     */
    bool isDone() const;

    /**
     * Returns <tt>true</tt> if and only if the I/O operation was completed
     * successfully.
     */
    bool isSuccess() const;

    // After a call has finished, returns true if the call failed.  The possible
    // reasons for failure depend on the RPC implementation.  Failed() must not
    // be called before a call has finished.  If Failed() returns true, the
    // contents of the response message are undefined.
    bool failed() const;

    /**
     * Returns the cause of the failed I/O operation if the I/O operation has
     * failed.
     *
     * @return the cause of the failure.
     *         <tt>NULL</tt> if succeeded or this future is not
     *         completed yet.
     */
    const Exception* getCause() const;

    // If true, indicates that the client canceled the RPC, so the server may
    // as well give up on replying to it.  The server should still call the
    // final "done" callback.
    /**
     * Returns <tt>true</tt> if and only if this future was
     * cancelled by a {@link #cancel()} method.
     */
    virtual bool isCancelled() const;

    /**
     * Marks this future as a success and notifies all
     * listeners.
     *
     * @return <tt>true</tt> if and only if successfully marked this future as
     *         a success. Otherwise <tt>false</tt> because this future is
     *         already marked as either a success or a failure.
     */
    virtual bool setSuccess();

    virtual bool setSuccess(const T& response) {
        setResponse(response);
        return setSuccess();
    }

    /**
     * Marks this future as a failure and notifies all
     * listeners.
     *
     * @return <tt>true</tt> if and only if successfully marked this future as
     *         a failure. Otherwise <tt>false</tt> because this future is
     *         already marked as either a success or a failure.
     */
    virtual bool setFailure(const Exception& cause);

    virtual bool setFailure(const T& response, const Exception& cause) {
        setResponse(response);
        return setFailure(cause);
    }


    virtual void setResponse(const T& response) {
        this->response = response;
    }


    virtual const T& getResponse() const {
        return this->response;
    }

    // Asks that the given callback be called when the RPC is canceled.  The
    // callback will always be called exactly once.  If the RPC completes without
    // being canceled, the callback will be called after completion.  If the RPC
    // has already been canceled when NotifyOnCancel() is called, the callback
    // will be called immediately.
    //
    // NotifyOnCancel() must be called no more than once per request.
    //void NotifyOnCancel(Closure* callback);

    /**
     * Adds the specified listener, which is a function object, to this future.
     * The specified listener is notified when this future is
     * {@link #isDone() done}.  If this future is already
     * completed, the specified listener is notified immediately.
     *
     * Use this function, you may not have to take care the listener's
     * life circle, otherwise, the ChannelFutureListener should.
     */
    void addListener(const CompletedCallback& listener) {
        callbacks.push_back(listener);
    }

    /**
     * Waits for this future to be completed.
     *
     * @throws InterruptedException
     *         if the current thread was interrupted
     */
    void await();

    /**
     * Waits for this future to be completed without
     * interruption.  This method catches an {@link InterruptedException} and
     * discards it silently.
     */
    void awaitUninterruptibly();

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
    bool await(int timeoutMillis);

    /**
     * Waits for this future to be completed within the
     * specified time limit without interruption.  This method catches an
     * {@link InterruptedException} and discards it silently.
     *
     * @return <tt>true</tt> if and only if the future was completed within
     *         the specified time limit
     */
    bool awaitUninterruptibly(int timeoutMillis);

protected:
    typedef boost::function0<void> StartCancelCallback;
    void setStartCancelCallback(const StartCancelCallback& callback) {
        startCancelCallback = callback;
    }

private:
    T response;
    StartCancelCallback startCancelCallback;
    std::deque<CompletedCallback> callbacks;
};

}
}
#endif //#if !defined(CETTY_SERVICE_SERVICEFUTURE_H)


