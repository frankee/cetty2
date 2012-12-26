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

#include <cetty/channel/asio/AsioWriteOperationQueue.h>

#include <boost/asio/buffer.hpp>

#include <cetty/buffer/ChannelBuffer.h>
#include <cetty/buffer/CompositeChannelBuffer.h>
#include <cetty/channel/ChannelPipeline.h>
#include <cetty/channel/asio/AsioSocketChannel.h>

namespace cetty {
namespace channel {
namespace asio {

using namespace cetty::channel;
using namespace cetty::buffer;

void AsioWriteOperationQueue::plusWriteBufferSize(int messageSize) {
    writeBufferSize_ += messageSize;

#if 0
    int highWaterMark = channel.config.getWriteBufferHighWaterMark();

    if (writeBufferSize >= highWaterMark) {
        if (writeBufferSize - messageSize < highWaterMark) {
            channel.handleAtHighWaterMark();
        }
    }
#endif
}

void AsioWriteOperationQueue::minusWriteBufferSize(int messageSize) {
    writeBufferSize_ -= messageSize;

#if 0
    int lowWaterMark = channel.config.getWriteBufferLowWaterMark();

    if (writeBufferSize == 0 || writeBufferSize < lowWaterMark) {
        if (writeBufferSize + messageSize >= lowWaterMark) {
            channel.handleAtLowWaterMark();
        }
    }
#endif
}

}
}
}
