
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

#include <cetty/channel/DefaultWriteCompletionEvent.h>

#include <cetty/channel/Channel.h>
#include <cetty/channel/ChannelFuture.h>
#include <cetty/util/Exception.h>

namespace cetty {
namespace channel {

DefaultWriteCompletionEvent::DefaultWriteCompletionEvent(const ChannelPtr& channel, int writtenAmount)
    : channel(channel),
    writtenAmount(writtenAmount) {
        if (writtenAmount < 0) {
            throw InvalidArgumentException("writtenAmount must be a positive integer: ");
        }
}

DefaultWriteCompletionEvent::~DefaultWriteCompletionEvent() {
}

const ChannelPtr& DefaultWriteCompletionEvent::getChannel() const {
    return channel;
}

const ChannelFuturePtr& DefaultWriteCompletionEvent::getFuture() const {
    return channel->getSucceededFuture();
}

int DefaultWriteCompletionEvent::getWrittenAmount() const {
    return writtenAmount;
}

std::string DefaultWriteCompletionEvent::toString() const {
    char buf[512];
    sprintf(buf, "%s WRITTEN_AMOUNT: %d",
        channel->toString().c_str(), getWrittenAmount());

    return buf;
}

}
}