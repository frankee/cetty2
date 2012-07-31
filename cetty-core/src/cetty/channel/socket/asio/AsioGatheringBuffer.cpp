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

#include <cetty/channel/socket/asio/AsioGatheringBuffer.h>
#include <boost/assert.hpp>
#include <cetty/buffer/ChannelBuffer.h>


namespace cetty {
namespace channel {
namespace socket {
namespace asio {

using namespace cetty::channel;
using namespace cetty::buffer;

bool AsioGatheringBuffer::empty() const {
    return buffers.truncatedIndex == 0;
}

int AsioGatheringBuffer::blockCount() const {
    return buffers.truncatedIndex;
}

int AsioGatheringBuffer::bytesCount() const {
    if (byteSize == 0) {
        for (int i = 0; i < buffers.truncatedIndex; ++i) {
            byteSize += (int)boost::asio::buffer_size(buffers[i]);
        }
    }

    return byteSize;
}

void AsioGatheringBuffer::clear() {
    buffers.truncatedIndex = 0;
    byteSize = 0;
}

void AsioGatheringBuffer::append(char* data, int size) {
    BOOST_ASSERT(buffers.truncatedIndex < MAX_BUFFER_COUNT);
    buffers[buffers.truncatedIndex++] = AsioBuffer(data, size);
    byteSize += size;
}

std::pair<char*, int> AsioGatheringBuffer::at(int index) {
    BOOST_ASSERT(index >= 0 && index < MAX_BUFFER_COUNT);
    AsioBuffer& buffer = buffers[index];
    return std::make_pair<char*,int>(
               boost::asio::buffer_cast<char*>(buffer),
               (int)boost::asio::buffer_size(buffer));
}

void AsioGatheringBuffer::mergeFrom(const ChannelBufferPtr& buffer) {
    if (buffer) {
        clear();
        buffer->readSlice(this);
        this->channelBuffer = buffer;
    }
}

}
}
}
}
