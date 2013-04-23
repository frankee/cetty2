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

#include <boost/assert.hpp>
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

AsioWriteOperation::AsioWriteOperation()
    : byteSize_() {
}

AsioWriteOperation::AsioWriteOperation(const ChannelBufferPtr& buffer,
                                       const ChannelFuturePtr& f)
    : byteSize_(0),
      future_(f),
      channelBuffer_(buffer) {
    GatheringBuffer gathering;
    buffer->slice(&gathering);

    byteSize_ = gathering.bytesCount();

    if (needCompactBuffers(gathering)) {
        channelBuffer_ = buffer->readBytes();
        int byteSize;
        char* bytes = const_cast<char*>(
                          channelBuffer_->readableBytes(&byteSize));

        BOOST_ASSERT(byteSize == byteSize_ &&
                     "buffer size should be same after compact");

        buffers_.push_back(AsioBuffer(bytes, byteSize_));
    }
    else {
        for (int i = 0, j = gathering.blockCount(); i < j; ++i) {
            const StringPiece& bytes = gathering.at(i);
            buffers_.push_back(
                AsioBuffer(const_cast<char*>(bytes.data()), bytes.size()));
        }
    }
}

AsioWriteOperation::AsioWriteOperation(const AsioWriteOperation& op)
    : byteSize_(op.byteSize_),
      buffers_(op.buffers_),
      future_(op.future_),
      channelBuffer_(op.channelBuffer_) {
}

AsioWriteOperation& AsioWriteOperation::operator=(const AsioWriteOperation& op) {
    byteSize_ = op.byteSize_;
    buffers_ = op.buffers_;
    channelBuffer_ = op.channelBuffer_;
    future_ = op.future_;
    return *this;
}

AsioWriteOperationQueue::AsioWriteOperationQueue(AsioSocketChannel& channel)
    : channel_(channel),
      writeBufferSize_(0) {
}

void AsioWriteOperationQueue::plusWriteBufferSize(int messageSize) {
    writeBufferSize_ += messageSize;
}

void AsioWriteOperationQueue::minusWriteBufferSize(int messageSize) {
    writeBufferSize_ -= messageSize;
}

}
}
}
