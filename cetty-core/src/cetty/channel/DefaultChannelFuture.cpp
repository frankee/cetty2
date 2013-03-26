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

#include <boost/thread.hpp>
#include <boost/date_time.hpp>

#include <cetty/channel/Channel.h>
#include <cetty/util/Exception.h>
#include <cetty/logging/LoggerHelper.h>

namespace cetty {
namespace channel {

using namespace cetty::util;
using namespace cetty::logging;

bool DefaultChannelFuture::useDeadLockChecker = true;
bool DefaultChannelFuture::disabledDeadLockCheckerOnce = false;

Exception DefaultChannelFuture::CANCELLED("Future canceled");

DefaultChannelFuture::DefaultChannelFuture(const ChannelPtr& channel,
        bool cancellable,
        bool threadUnsafe)
    : cancellable_(cancellable),
      done_(false),
      threadUnsafe_(threadUnsafe),
      channel_(channel),
      completedListeners_(NULL),
      progressListeners_(NULL),
      cause_(NULL),
      mutex_(NULL),
      cond_(NULL),
      waiters_(0) {

    if (!threadUnsafe) {
        condition();
        getMutex();
    }
}

DefaultChannelFuture::DefaultChannelFuture(const ChannelWeakPtr& channel,
        bool cancellable,
        bool threadUnsafe /*= false*/)
    : cancellable_(cancellable),
      done_(false),
      threadUnsafe_(threadUnsafe),
      channel_(channel),
      completedListeners_(NULL),
      progressListeners_(NULL),
      cause_(NULL),
      mutex_(NULL),
      cond_(NULL),
      waiters_(0) {

    if (!threadUnsafe) {
        condition();
        getMutex();
    }
}

DefaultChannelFuture::~DefaultChannelFuture() {
    if (completedListeners_) {
        delete completedListeners_;
    }

    if (progressListeners_) {
        delete progressListeners_;
    }

    if (cause_ && cause_ != &CANCELLED) {
        delete cause_;
    }

    if (cond_) {
        delete cond_;
    }

    if (mutex_) {
        delete mutex_;
    }
}

ChannelPtr DefaultChannelFuture::channel() const {
    return channel_.lock();
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
    return done_;
}

bool DefaultChannelFuture::isSuccess() const {
    boost::lock_guard<boost::mutex> guard(getMutex());
    return done_ && cause_ == NULL;
}

const Exception* DefaultChannelFuture::failedCause() const {
    boost::lock_guard<boost::mutex> guard(getMutex());

    if (cause_ != &CANCELLED) {
        return cause_;
    }
    else {
        return NULL;
    }
}

bool DefaultChannelFuture::isCancelled() const {
    boost::lock_guard<boost::mutex> guard(getMutex());
    return cause_ == &CANCELLED;
}

ChannelFuturePtr DefaultChannelFuture::addListener(
    const CompletedCallback& listener,
    int priority) {
    if (!listener) {
        return shared_from_this();
    }

    bool notifyNow = false;

    {
        boost::lock_guard<boost::mutex> guard(getMutex());

        if (done_) {
            notifyNow = true;
        }
        else {
            if (firstListener_.empty() && completedListeners_ == NULL) {
                firstListener_.callback = listener;
                firstListener_.priority = priority;
            }
            else {
                if (completedListeners_ == NULL) {
                    completedListeners_ = new PriorityCallbackQueue();
                }

                completedListeners_->push(firstListener_);
                completedListeners_->push(PriorityCallback(listener, priority));
                firstListener_.clear();
            }
        }
    }

    if (notifyNow) {
        notifyListener(listener);
    }

    return shared_from_this();
}

ChannelFuturePtr DefaultChannelFuture::addProgressListener(
    const ProgressedCallback& listener) {
    if (!listener) {
        return shared_from_this();
    }

    {
        boost::lock_guard<boost::mutex> guard(getMutex());

        if (!done_) {
            if (progressListeners_ == NULL) {
                progressListeners_ = new ProgressedCallbackQueue();
            }

            progressListeners_->push_back(listener);
        }
    }

    return shared_from_this();
}

ChannelFuturePtr DefaultChannelFuture::removeListener(
    const CompletedCallback& listener) {
    if (!listener) {
        return shared_from_this();
    }

#if 0 // FIXME
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

    return shared_from_this();
}

ChannelFuturePtr DefaultChannelFuture::removeProgressListener(
    const ProgressedCallback& listener) {
    if (!listener) {
        return shared_from_this();
    }

#if 0 // FIXME
    boost::lock_guard<boost::mutex> guard(getMutex());

    if (!done && completedListeners != NULL) {
        completedListeners->remove(listener);
    }

#endif
    return shared_from_this();
}

ChannelFuturePtr DefaultChannelFuture::await() {
    if (boost::this_thread::interruption_requested()) {
        throw InterruptedException();
    }

    {
        boost::unique_lock<boost::mutex> lock(getMutex());

        while (!done_) {
            checkDeadLock();
            waiters_++;

            try {
                condition().wait(lock);
            }
            catch (...) {
                waiters_--;
            }
        }
    }

    return shared_from_this();
}

bool DefaultChannelFuture::await(int64_t timeoutMillis) {
    return await0(timeoutMillis, true);
}

ChannelFuturePtr DefaultChannelFuture::awaitUninterruptibly() {
    bool interrupted = false;
    {
        boost::unique_lock<boost::mutex> lock(getMutex());

        while (!done_) {
            checkDeadLock();
            waiters_++;

            try {
                condition().wait(lock);
            }
            catch (const boost::thread_interrupted& e) {
                (void)e;
                interrupted = true;
                waiters_--;

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

bool DefaultChannelFuture::awaitUninterruptibly(int64_t timeoutMillis) {
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
        if (done_) {
            return false;
        }

        done_ = true;

        if (waiters_ > 0) {
            cond_->notify_all();
        }
    }

    notifyListeners();
    return true;
}

bool DefaultChannelFuture::setFailure(const Exception& cause) {
    {
        boost::lock_guard<boost::mutex> guard(getMutex());

        // Allow only once.
        if (done_) {
            return false;
        }

        if (this->cause_ && this->cause_ != &CANCELLED) {
            delete this->cause_;
            this->cause_ = NULL;
        }

        this->cause_ = new Exception(cause);

        done_ = true;

        if (waiters_ > 0) {
            cond_->notify_all();
        }
    }

    notifyListeners();
    return true;
}

bool DefaultChannelFuture::cancel() {
    if (!cancellable_) {
        return false;
    }

    {
        boost::lock_guard<boost::mutex> guard(getMutex());

        // Allow only once.
        if (done_) {
            return false;
        }

        cause_ = &CANCELLED;
        done_ = true;

        if (waiters_ > 0) {
            cond_->notify_all();
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
        if (done_) {
            return false;
        }

        if (progressListeners_ == NULL || progressListeners_->empty()) {
            // Nothing to notify - no need to create an empty array.
            return true;
        }

        tmplist = *progressListeners_;
    }

    while (!tmplist.empty()) {
        notifyProgressListener(tmplist.front(), amount, current, total);
        tmplist.pop_front();
    }

    return true;
}

bool DefaultChannelFuture::await0(int64_t timeoutMillis, bool interruptable) {
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

        if (done_) {
            return done_;
        }
        else if (timeoutMillis <= 0) {
            return done_;
        }

        checkDeadLock();
        waiters_++;

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

                if (done_) {
                    return true;
                }
                else {
                    if (boost::get_system_time() >= expiredTime) {
                        return done_;
                    }
                }
            }
        }
        catch (...) {
            waiters_--;
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
    if (!firstListener_.empty()) {
        notifyListener(firstListener_.callback);
        firstListener_.clear();
    }

    if (completedListeners_ && !completedListeners_->empty()) {
        while (!completedListeners_->empty()) {
            const PriorityCallback& callback = completedListeners_->top();
            notifyListener(callback.callback);
            completedListeners_->pop();
        }
    }

    if (progressListeners_) {
        progressListeners_->clear();
    }
}

void DefaultChannelFuture::notifyListener(const CompletedCallback& l) {
    if (l.empty()) { return; }

    try {
        l(*this);
    }
    catch (const Exception& e) {
        LOG_WARN << "An exception ("
                 << e.message()
                 << ") thrown by ChannelFutureListener.";
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
        LOG_WARN << "An exception ("
                 << e.message()
                 << ") thrown by ChannelFutureListener.";
    }
}

boost::condition_variable& DefaultChannelFuture::condition() {
    if (!cond_) {
        cond_ = new boost::condition_variable();
    }

    return *cond_;
}

boost::mutex& DefaultChannelFuture::getMutex() const {
    if (!mutex_) {
        mutex_ = new boost::mutex();
    }

    return *mutex_;
}

}
}
