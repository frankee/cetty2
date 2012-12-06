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

#include <cetty/channel/VoidChannelFuture.h>

#include <cetty/channel/Channel.h>
#include <cetty/logging/LoggerHelper.h>

namespace cetty {
namespace channel {

ChannelPtr VoidChannelFuture::channel() const {
    return channel_.lock();
}

bool VoidChannelFuture::isDone() const {
    return false;
}

bool VoidChannelFuture::isCancelled() const {
    return false;
}

bool VoidChannelFuture::isSuccess() const {
    return false;
}

const Exception* VoidChannelFuture::failedCause() const {
    return reinterpret_cast<const Exception*>(0);
}

bool VoidChannelFuture::cancel() {
    return false;
}

bool VoidChannelFuture::setSuccess() {
    return false;
}

bool VoidChannelFuture::setFailure(const Exception& cause) {
    return false;
}

bool VoidChannelFuture::setProgress(int amount, int current, int total) {
    return false;
}

cetty::channel::ChannelFuturePtr VoidChannelFuture::addListener(const CompletedCallback& listenerr, int priority /*= 0*/) {
    fail();
    return shared_from_this();
}

cetty::channel::ChannelFuturePtr VoidChannelFuture::addProgressListener(const ProgressedCallback& listener) {
    fail();
    return shared_from_this();
}

cetty::channel::ChannelFuturePtr VoidChannelFuture::removeListener(const CompletedCallback& listener) {
    return shared_from_this();
}

cetty::channel::ChannelFuturePtr VoidChannelFuture::removeProgressListener(const ProgressedCallback& listener) {
    return shared_from_this();
}

cetty::channel::ChannelFuturePtr VoidChannelFuture::await() {
    fail();
    return false;
}

bool VoidChannelFuture::await(int64_t timeoutMillis) {
    fail();
    return false;
}

cetty::channel::ChannelFuturePtr VoidChannelFuture::awaitUninterruptibly() {
    fail();
    return false;
}

bool VoidChannelFuture::awaitUninterruptibly(int64_t timeoutMillis) {
    fail();
    return false;
}

void VoidChannelFuture::fail() {
    LOG_ERROR << "not supported in void future";
}

}
}
