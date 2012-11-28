#if !defined(CETTY_CHANNEL_SOCKET_ASIO_ASIOWRITEREQUESTQUEUE_H)
#define CETTY_CHANNEL_SOCKET_ASIO_ASIOWRITEREQUESTQUEUE_H

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

#include <deque>
#include <boost/asio/buffer.hpp>
#include <boost/static_assert.hpp>

#include <cetty/buffer/ChannelBuffer.h>
#include <cetty/buffer/GatheringBuffer.h>
#include <cetty/channel/ChannelFuture.h>
#include <cetty/util/TruncatableArray.h>

namespace cetty {
namespace channel {
class MessageEvent;
}
}

namespace cetty {
namespace util {
class Exception;
}
}

namespace cetty {
namespace channel {
namespace asio {

using namespace cetty::buffer;
using namespace cetty::channel;

class AsioSocketChannel;

class AsioWriteOperation {
public:
    const static int  MAX_BUFFER_COUNT = 8;
    typedef boost::asio::mutable_buffer AsioBuffer;
    typedef cetty::util::TruncatableArray<AsioBuffer, MAX_BUFFER_COUNT> AsioBufferArray;

public:
    AsioWriteOperation()  {}

    AsioWriteOperation(const ChannelBufferPtr& buffer, const ChannelFuturePtr& f)
        : byteSize(0), channelBuffer(buffer), future(f) {
        GatheringBuffer gathering;
        buffer->slice(&gathering);

        byteSize = gathering.bytesCount();

        if (needCompactBuffers(gathering)) {
            channelBuffer = buffer->readBytes();
            char* bytes = (char*)channelBuffer->readableBytes((int*)NULL);
            buffers[buffers.truncatedCnt++] = AsioBuffer(bytes, byteSize);
        }
        else {
            for (int i = 0, j = gathering.blockCount(); i < j; ++i) {
                const StringPiece& bytes = gathering.at(i);
                buffers[buffers.truncatedCnt++] =
                    AsioBuffer((char*)bytes.data(), bytes.size());
            }
        }
    }

    AsioWriteOperation(const AsioWriteOperation& op)
        : byteSize(op.byteSize),
          buffers(op.buffers),
          channelBuffer(op.channelBuffer),
          future(op.future) {
    }

    AsioWriteOperation& operator=(const AsioWriteOperation& op) {
        byteSize = op.byteSize;
        buffers = op.buffers;
        channelBuffer = op.channelBuffer;
        future = op.future;
        return *this;
    }

    ~AsioWriteOperation() {}

    int writeBufferSize() const {
        return byteSize;
    }

    bool isMultiBuffers() const {
        return buffers.truncatedCnt > 1;
    }

    AsioBuffer& getAsioBuffer() {
        return buffers[0];
    }
    AsioBufferArray& getAsioBufferArray() {
        return buffers;
    }

    bool setSuccess() {
        return future ? future->setSuccess() : false;
    }
    bool setFailure(const Exception& cause) {
        return future ? future->setFailure(cause) : false;
    }

private:
    bool needCompactBuffers(const GatheringBuffer& gathering) {
        return gathering.blockCount() > 1;
    }

private:
    int byteSize;
    AsioBufferArray buffers;

    ChannelBufferPtr channelBuffer;
    ChannelFuturePtr    future;
};

// no need ensure the thread safe.
class AsioWriteOperationQueue {
public:
    AsioWriteOperationQueue(AsioSocketChannel& channel)
        : channel_(channel),
        writeBufferSize_(0) {
    }

    ~AsioWriteOperationQueue() {}

    bool  empty() const { return ops_.empty(); }
    size_t size() const { return ops_.size(); }

    AsioWriteOperation& peek() {
        return ops_.front();
    }

    void popup() {
        minusWriteBufferSize(ops_.front().writeBufferSize());
        ops_.pop_front();
    }

    AsioWriteOperation& offer(const ChannelBufferPtr& buffer, const ChannelFuturePtr& f) {
        ops_.push_back(AsioWriteOperation(buffer, f));
        plusWriteBufferSize(ops_.back().writeBufferSize());
        return ops_.back();
    }

    int  getWriteBufferSize() const {
        return writeBufferSize_;
    }

private:
    void plusWriteBufferSize(int messageSize);
    void minusWriteBufferSize(int messageSize);

private:
    AsioSocketChannel& channel_;

    int writeBufferSize_;
    std::deque<AsioWriteOperation> ops_;
};

}
}
}

#endif //#if !defined(CETTY_CHANNEL_SOCKET_ASIO_ASIOWRITEREQUESTQUEUE_H)
