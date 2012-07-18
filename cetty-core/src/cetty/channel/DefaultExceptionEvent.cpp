
/**
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

#include <cetty/channel/DefaultExceptionEvent.h>
#include <cetty/channel/Channel.h>
#include <cetty/util/Exception.h>

namespace cetty {
namespace channel {

using namespace cetty::util;

DefaultExceptionEvent::DefaultExceptionEvent(const ChannelPtr& channel,
        const Exception& cause)
    : channel(channel),
      future(channel->getSucceededFuture()),
      cause(cause) {
    //StackTraceSimplifier.simplify(cause);
}

DefaultExceptionEvent::~DefaultExceptionEvent() {

}

const ChannelPtr& DefaultExceptionEvent::getChannel() const {
    return this->channel;
}

const ChannelFuturePtr& DefaultExceptionEvent::getFuture() const {
    return (this->future);
}

const Exception& DefaultExceptionEvent::getCause() const {
    return this->cause;
}

std::string DefaultExceptionEvent::toString() const {
    std::string buf = this->channel->toString();
    buf.append(" EXCEPTION: ");
    buf.append(this->cause.what());

    return buf;
}

}
}
