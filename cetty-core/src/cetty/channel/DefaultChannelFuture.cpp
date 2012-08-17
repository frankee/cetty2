/*
 * Copyright (c) 2010-2011 frankee zhou (frankee.zhou at gmail dot com)
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
#include <cetty/channel/DefaultChannelFuture.h>

#include <boost/date_time.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>

#include <boost/thread/detail/thread.hpp>

#include <cetty/channel/Channel.h>

#include <cetty/logging/LoggerHelper.h>

#include <cetty/util/Exception.h>

namespace cetty {
namespace channel {

using namespace cetty::util;
using namespace cetty::logging;

bool DefaultChannelFuture::useDeadLockChecker = true;
bool DefaultChannelFuture::disabledDeadLockCheckerOnce = false;

Exception DefaultChannelFuture::CANCELLED("Future canceled");

DefaultChannelFuture::DefaultChannelFuture(const ChannelPtr& channel,
        bool cancellable)
    : cancellable(cancellable),
      done(false),
      channel(channel),
      completedListeners(NULL),
      progressListeners(NULL),
      cause(NULL),
      mutex(NULL),
      cond(NULL),
      waiters(0) {
}

DefaultChannelFuture::~DefaultChannelFuture() {
    if (completedListeners) {
        delete completedListeners;
    }

    if (progressListeners) {
        delete progressListeners;
    }

    if (cause && cause != &CANCELLED) {
        delete cause;
    }

    if (cond) {
        delete cond;
    }

    if (mutex) {
        delete mutex;
    }
}

void DefaultChannelFuture::setUseDeadLockChecker(bool useDeadLockChecker) {
    if (!useDeadLockChecker && !disabledDeadLockCheckerOnce) {
        disabledDeadLockCheckerOnce = true;
        LOG_DEBUG << "The dead lock checker in ChannelFuture \
                            has been disabled as requested at your own risk.";
    }

    DefaultChannelFuture::useDeadLockChecker = useDeadLockChecker;
}

bool DefaultChannelFuture::isDone() const {
    boost::lock_guard<boost::mutex> guard(getMutex());
    return done;
}

bool DefaultChannelFuture::isSuccess() const {
    boost::lock_guard<boost::mutex> guard(getMutex());
    return done && cause == NULL;
}

const Exception* DefaultChannelFuture::getCause() const {
    boost::lock_guard<boost::mutex> guard(getMutex());

    if (cause != &CANCELLED) {
        return cause;
    }
    else {
        return NULL;
    }
}

bool DefaultChannelFuture::isCancelled() const {
    boost::lock_guard<boost::mutex> guard(getMutex());
    return cause == &CANCELLED;
}

void DefaultChannelFuture::addListener(const CompletedCallback& listener, int priority) {
    if (listener.empty()) {
        return;
    }

    bool notifyNow = false;

    {
        boost::lock_guard<boost::mutex> guard(getMutex());

        if (done) {
            notifyNow = true;
        }
        else {
            if (firstListener.empty() && completedListeners == NULL) {
                firstListener.callback = listener;
                firstListener.priority = priority;
            }
            else {
                if (completedListeners == NULL) {
                    completedListeners = new PriorityCallbackQueue();
                }

                completedListeners->push(firstListener);
                completedListeners->push(PriorityCallback(listener, priority));
                firstListener.clear();
            }
        }
    }

    if (notifyNow) {
        notifyListener(listener);
    }
}

void DefaultChannelFuture::addProgressListener(const ProgressedCallback& listener) {
    if (!listener) {
        return;
    }

    {
        boost::lock_guard<boost::mutex> guard(getMutex());

        if (!done) {
            if (progressListeners == NULL) {
                progressListeners = new ProgressedCallbackQueue();
            }

            progressListeners->push_back(listener);
        }
    }
}

void DefaultChannelFuture::removeListener(const CompletedCallback& listener) {
    if (!listener) {
        return;
    }

#if 0
    boost::lock_guard<boost::mutex> guard(getMutex());

    if (!done) {
        if (firstListener.contains(listener)) {
            if (completedListeners != NULL && !completedListeners->empty()) {
                firstListener = completedListeners->front();
                completedListeners->pop_front();
            }
            else {
                firstListener.clear();
            }
        }
        else if (completedListeners != NULL) {
            completedListeners->remove(listener);
        }
    }

#endif
}

void DefaultChannelFuture::removeProgressListener(const ProgressedCallback& listener) {
    if (!listener) {
        return;
    }

#if 0
    boost::lock_guard<boost::mutex> guard(getMutex());

    if (!done && completedListeners != NULL) {
        completedListeners->remove(listener);
    }

#endif
}


ChannelFuturePtr DefaultChannelFuture::await() {
    if (boost::this_thread::interruption_requested()) {
        throw InterruptedException();
    }

    {
        boost::unique_lock<boost::mutex> lock(getMutex());

        while (!done) {
            checkDeadLock();
            waiters++;

            try {
                condition().wait(lock);
            }
            catch (...) {
                waiters--;
            }
        }
    }

    return shared_from_this();
}

bool DefaultChannelFuture::await(int timeoutMillis) {
    return await0(timeoutMillis, true);
}

ChannelFuturePtr DefaultChannelFuture::awaitUninterruptibly() {
    bool interrupted = false;
    {
        boost::unique_lock<boost::mutex> lock(getMutex());

        while (!done) {
            checkDeadLock();
            waiters++;

            try {
                condition().wait(lock);
            }
            catch (const boost::thread_interrupted& e) {
                (void)e;
                interrupted = true;
                waiters--;

                LOG_WARN << "thread interrupted while awaiting";
            }
        }
    }

    if (interrupted) {
        //TODO must get
        //Thread.currentThread().interrupt();
    }

    return shared_from_this();
}

bool DefaultChannelFuture::awaitUninterruptibly(int timeoutMillis) {
    try {
        return await0(timeoutMillis, false);
    }
    catch (const boost::thread_interrupted& e) {
        (void)e;
        throw RuntimeException("thread interrupted.");
    }
}

bool DefaultChannelFuture::setSuccess() {
    {
        boost::lock_guard<boost::mutex> guard(getMutex());

        // Allow only once.
        if (done) {
            return false;
        }

        done = true;

        if (waiters > 0) {
            cond->notify_all();
        }
    }

    notifyListeners();
    return true;
}

bool DefaultChannelFuture::setFailure(const Exception& cause) {
    {
        boost::lock_guard<boost::mutex> guard(getMutex());

        // Allow only once.
        if (done) {
            return false;
        }

        if (this->cause && this->cause != &CANCELLED) {
            delete this->cause;
            this->cause = NULL;
        }

        this->cause = new Exception(cause);

        done = true;

        if (waiters > 0) {
            cond->notify_all();
        }
    }

    notifyListeners();
    return true;
}

bool DefaultChannelFuture::cancel() {
    if (!cancellable) {
        return false;
    }

    {
        boost::lock_guard<boost::mutex> guard(getMutex());

        // Allow only once.
        if (done) {
            return false;
        }

        cause = &CANCELLED;
        done = true;

        if (waiters > 0) {
            cond->notify_all();
        }
    }

    notifyListeners();
    return true;
}

bool DefaultChannelFuture::setProgress(int amount, int current, int total) {
    ProgressedCallbackQueue tmplist;

    {
        boost::lock_guard<boost::mutex> guard(getMutex());

        // Do not generate progress event after completion.
        if (done) {
            return false;
        }

        if (progressListeners == NULL || progressListeners->empty()) {
            // Nothing to notify - no need to create an empty array.
            return true;
        }

        tmplist = *progressListeners;
    }

    while (!tmplist.empty()) {
        notifyProgressListener(tmplist.front(), amount, current, total);
        tmplist.pop_front();
    }

    return true;
}

bool DefaultChannelFuture::await0(int timeoutMillis, bool interruptable) {
    if (interruptable && boost::this_thread::interruption_requested()) {
        throw InterruptedException("");
    }

    boost::posix_time::time_duration duration =
        boost::posix_time::milliseconds(timeoutMillis);

    boost::posix_time::ptime expiredTime(
        timeoutMillis <= 0 ? boost::get_system_time()
        : boost::get_system_time() + duration);

    bool interrupted = false;

    try {
        boost::unique_lock<boost::mutex> lock(getMutex());

        if (done) {
            return done;
        }
        else if (timeoutMillis <= 0) {
            return done;
        }

        checkDeadLock();
        waiters++;

        try {
            for (;;) {
                try {
                    condition().timed_wait(lock, expiredTime);
                }
                catch (const boost::thread_interrupted& e) {
                    (void)e;
                    if (interruptable) {
                        throw InterruptedException("");
                    }
                    else {
                        interrupted = true;
                    }
                }

                if (done) {
                    return true;
                }
                else {
                    if (boost::get_system_time() >= expiredTime) {
                        return done;
                    }
                }
            }
        }
        catch (...) {
            waiters--;
        }
    }
    catch (...) {
        if (interrupted) {
            //Thread.currentThread().interrupt();
        }
    }

    return false;
}

void DefaultChannelFuture::checkDeadLock() {
    //     if (isUseDeadLockChecker() && IoWorkerRunnable.IN_IO_THREAD.get()) {
    //         throw IllegalStateException(
    //                 "await*() in I/O thread causes a dead lock or "
    //                 "sudden performance drop. Use addListener() instead or "
    //                 "call await*() from a different thread.");
    //     }
}

void DefaultChannelFuture::notifyListeners() {
    // This method doesn't need synchronization because:
    // 1) This method is always called after synchronized (this) block.
    //    Hence any listener list modification happens-before this method.
    // 2) This method is called only when 'done' is true.  Once 'done'
    //    becomes true, the listener list is never modified - see add/removeListener()
    if (!firstListener.empty()) {
        notifyListener(firstListener.callback);
        firstListener.clear();
    }

    if (completedListeners && !completedListeners->empty()) {
        while (!completedListeners->empty()) {
            const PriorityCallback& callback = completedListeners->top();
            notifyListener(callback.callback);
            completedListeners->pop();
        }
    }

    if (progressListeners) {
        progressListeners->clear();
    }
}

void DefaultChannelFuture::notifyListener(const CompletedCallback& l) {
    if (l.empty()) { return; }

    try {
        l(*this);
    }
    catch (const Exception& e) {
        LOG_WARN_E(e) << "An exception was thrown by ChannelFutureListener .";
    }
}

void DefaultChannelFuture::notifyProgressListener(const ProgressedCallback& l,
        int amount,
        int current,
        int total) {

    if (l.empty()) { return; }

    try {
        l(*this, amount, current, total);
    }
    catch (const Exception& e) {
        LOG_WARN_E(e) << "An exception was thrown by process callback.";
    }
}

boost::condition_variable& DefaultChannelFuture::condition() {
    if (!cond) {
        cond = new boost::condition_variable();
    }

    return *cond;
}

const ChannelPtr& DefaultChannelFuture::getChannel() const {
    return this->channel;
}

boost::mutex& DefaultChannelFuture::getMutex() const {
    if (!mutex) {
        mutex = new boost::mutex();
    }

    return *mutex;
}

}
}
