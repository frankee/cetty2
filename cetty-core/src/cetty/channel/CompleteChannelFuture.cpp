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
#include <cetty/channel/CompleteChannelFuture.h>

#include <boost/thread/thread.hpp>

#include <cetty/channel/Channel.h>
#include <cetty/util/Exception.h>
#include <cetty/logging/LoggerHelper.h>

namespace cetty {
namespace channel {

using namespace cetty::util;

CompleteChannelFuture::CompleteChannelFuture(const ChannelPtr& channel)
    : channel_(channel) {
}

CompleteChannelFuture::CompleteChannelFuture(const ChannelWeakPtr& channel)
    : channel_(channel) {
}

ChannelFuturePtr CompleteChannelFuture::addListener(
    const CompletedCallback& listener,
    int priority) {
    if (listener.empty()) {
        return shared_from_this();
    }

    try {
        listener(*this);
    }
    catch (const Exception& e) {
        LOG_WARN << "An exception ("
                 << e.getMessage()
                 << ") thrown by ChannelFutureListener.";
    }

    return shared_from_this();
}

ChannelFuturePtr CompleteChannelFuture::addProgressListener(
    const ProgressedCallback& listener) {
    if (listener.empty()) {
        return shared_from_this();
    }

    try {
        listener(*this, 0, 0, 0);
    }
    catch (const Exception& e) {
        LOG_WARN << "An exception ("
                 << e.getMessage()
                 << ") thrown by ChannelFutureListener.";
    }

    return shared_from_this();
}

ChannelFuturePtr CompleteChannelFuture::removeListener(
    const CompletedCallback& listener) {
    return shared_from_this();
}

ChannelFuturePtr CompleteChannelFuture::removeProgressListener(
    const ProgressedCallback& listener) {
    return shared_from_this();
}

ChannelFuturePtr CompleteChannelFuture::await() {
    if (boost::this_thread::interruption_requested()) {
        throw InterruptedException();
    }

    return shared_from_this();
}

bool CompleteChannelFuture::await(int64_t timeoutMillis) {
    if (boost::this_thread::interruption_requested()) {
        throw InterruptedException();
    }

    return true;
}

ChannelFuturePtr CompleteChannelFuture::awaitUninterruptibly() {
    return shared_from_this();
}

bool CompleteChannelFuture::awaitUninterruptibly(int64_t timeoutMillis) {
    return true;
}

const ChannelWeakPtr& CompleteChannelFuture::channel() const {
    return channel_;
}

ChannelPtr CompleteChannelFuture::sharedChannel() const {
    return channel_.lock();
}

bool CompleteChannelFuture::isDone() const {
    return true;
}

bool CompleteChannelFuture::setProgress(int amount, int current, int total) {
    return false;
}

bool CompleteChannelFuture::setFailure(const Exception& cause) {
    return false;
}

bool CompleteChannelFuture::setSuccess() {
    return false;
}

bool CompleteChannelFuture::cancel() {
    return false;
}

bool CompleteChannelFuture::isCancelled() const {
    return false;
}

}
}
