#if !defined(CETTY_CHANNEL_DEFAULTCHANNELFUTURE_H)
#define CETTY_CHANNEL_DEFAULTCHANNELFUTURE_H

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

#include <deque>
#include <queue>
#include <cetty/channel/ChannelFuture.h>

namespace boost {
    class mutex;
    class condition_variable;
}

namespace cetty {
namespace channel {

using namespace ::cetty::util;

/**
 * The default {@link ChannelFuture} implementation.  It is recommended to
 * use {@link Channels#future(Channel)} and {@link Channels#future(Channel, bool)}
 * to create a new {@link ChannelFuture} rather than calling the constructor
 * explicitly.
 *
 *
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 * @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
 */

class DefaultChannelFuture : public ChannelFuture {
public:
    /**
     * Returns <tt>true</tt> if and only if the dead lock checker is enabled.
     */
    static bool isUseDeadLockChecker() {
        return useDeadLockChecker;
    }

    /**
     * Enables or disables the dead lock checker.  It is not recommended to
     * disable the dead lock checker.  Disable it at your own risk!
     */
    static void setUseDeadLockChecker(bool useDeadLockChecker);

public:
    /**
     * Creates a new instance.
     *
     * @param channel
     *        the {@link Channel} associated with this future
     * @param cancellable
     *        <tt>true</tt> if and only if this future can be canceled
     * @param threadUnsaft
     *        <tt>true</tt> if the future only used in the one thread loop
     *        is mainly internal used.
     */
    DefaultChannelFuture(const ChannelPtr& channel,
        bool cancellable,
        bool threadUnsafe = false);

    virtual ~DefaultChannelFuture();

    virtual const ChannelPtr& getChannel() const;

    virtual bool isDone() const;
    virtual bool isSuccess() const;
    virtual const Exception* getCause() const;
    virtual bool isCancelled() const;

    virtual ChannelFuturePtr addListener(const CompletedCallback& listener, int priority = 0);
    virtual ChannelFuturePtr addProgressListener(const ProgressedCallback& listener);
    virtual ChannelFuturePtr removeListener(const CompletedCallback& listener);
    virtual ChannelFuturePtr removeProgressListener(const ProgressedCallback& listener);

    virtual ChannelFuturePtr await();
    virtual bool await(int64_t timeoutMillis);

    virtual ChannelFuturePtr awaitUninterruptibly();
    virtual bool awaitUninterruptibly(int64_t timeoutMillis);

    virtual bool setSuccess();
    virtual bool setFailure(const Exception& cause);
    virtual bool setProgress(int amount, int current, int total);

    virtual bool cancel();

private:
    bool await0(int64_t timeoutMillis, bool interruptable);

    void checkDeadLock();

    void notifyListeners();
    void notifyListener(const CompletedCallback& l);
    void notifyProgressListener(const ProgressedCallback& l,
                                int amount,
                                int current,
                                int total);

    boost::condition_variable& condition();
    boost::mutex& getMutex() const;

private:
    static Exception CANCELLED;

    static bool useDeadLockChecker;
    static bool disabledDeadLockCheckerOnce;

private:
    struct PriorityCallback {
        int priority;
        CompletedCallback callback;

        PriorityCallback() : priority(0) {}
        PriorityCallback(const CompletedCallback& callback)
            : priority(0), callback(callback) {}
        PriorityCallback(const CompletedCallback& callback, int priority)
            : priority(priority), callback(callback) {}

        bool empty() const {
            return !callback;
        }

        void clear() {
            callback.clear();
        }
    };

    struct PriorityCallbackCmp {
        bool operator()(const PriorityCallback& lcb, const PriorityCallback& rcb) {  
            if (lcb.priority == rcb.priority) {
                return true; // if the two have same priority, just FIFO, like normal queue.
            }
            else {
                return lcb.priority <= rcb.priority;
            }
        }
    };

    typedef std::priority_queue<PriorityCallback, std::deque<PriorityCallback>, PriorityCallbackCmp> PriorityCallbackQueue;
    typedef std::deque<ProgressedCallback> ProgressedCallbackQueue;

private:
    bool cancellable;
    bool done;

    bool threadUnsafe;

    ChannelPtr channel;

    PriorityCallback firstListener;

    PriorityCallbackQueue* completedListeners;
    ProgressedCallbackQueue* progressListeners;

    Exception* cause;

    mutable boost::mutex* mutex;
    boost::condition_variable* cond;

    int waiters;
};

}
}


#endif //#if !defined(CETTY_CHANNEL_DEFAULTCHANNELFUTURE_H)

// Local Variables:
// mode: c++
// End:
