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
    AsioWriteOperation();
    AsioWriteOperation(const ChannelBufferPtr& buffer,
                       const ChannelFuturePtr& f);

    AsioWriteOperation(const AsioWriteOperation& op);
    AsioWriteOperation& operator=(const AsioWriteOperation& op);

    ~AsioWriteOperation() {}

    int writeBufferSize() const;
    bool hasWrittenAllBuffer(int size) const;

    bool isSingleBuffer() const;

    AsioBuffer& asioBuffer();
    AsioBufferArray& asioBufferArray();

    bool setSuccess();
    bool setFailure(const Exception& cause);

private:
    bool needCompactBuffers(const GatheringBuffer& gathering);

private:
    int byteSize_;
    AsioBufferArray buffers_;

    ChannelFuturePtr future_;
    ChannelBufferPtr channelBuffer_;
};

// no need ensure the thread safe.
class AsioWriteOperationQueue {
public:
    AsioWriteOperationQueue(AsioSocketChannel& channel);

    ~AsioWriteOperationQueue() {}

    bool  empty() const;
    std::size_t size() const;

    AsioWriteOperation& front();

    void popFront();

    AsioWriteOperation& offer(const ChannelBufferPtr& buffer, const ChannelFuturePtr& f);

    AsioWriteOperation& offer(const AsioWriteOperation& operation);

    int writeBufferSize() const;

private:
    void plusWriteBufferSize(int messageSize);
    void minusWriteBufferSize(int messageSize);

private:
    AsioSocketChannel& channel_;

    int writeBufferSize_;
    std::deque<AsioWriteOperation> ops_;
};

inline
int AsioWriteOperation::writeBufferSize() const {
    return byteSize_;
}

inline
bool AsioWriteOperation::hasWrittenAllBuffer(int size) const {
    return byteSize_ == size;
}

inline
bool AsioWriteOperation::isSingleBuffer() const {
    return buffers_.truncatedSize() == 1;
}

inline
AsioWriteOperation::AsioBuffer& AsioWriteOperation::asioBuffer() {
    return buffers_[0];
}

inline
AsioWriteOperation::AsioBufferArray& AsioWriteOperation::asioBufferArray() {
    return buffers_;
}

inline
bool AsioWriteOperation::setSuccess() {
    return future_ ? future_->setSuccess() : false;
}

inline
bool AsioWriteOperation::setFailure(const Exception& cause) {
    return future_ ? future_->setFailure(cause) : false;
}

inline
bool AsioWriteOperation::needCompactBuffers(const GatheringBuffer& gathering) {
    return gathering.blockCount() > 1;
}

inline
bool AsioWriteOperationQueue::empty() const {
    return ops_.empty();
}

inline
size_t AsioWriteOperationQueue::size() const {
    return ops_.size();
}

inline
int AsioWriteOperationQueue::writeBufferSize() const {
    return writeBufferSize_;
}

inline
AsioWriteOperation& AsioWriteOperationQueue::front() {
    return ops_.front();
}

inline
void AsioWriteOperationQueue::popFront() {
    minusWriteBufferSize(ops_.front().writeBufferSize());
    ops_.pop_front();
}

inline
AsioWriteOperation& AsioWriteOperationQueue::offer(const ChannelBufferPtr& buffer,
        const ChannelFuturePtr& f) {
    ops_.push_back(AsioWriteOperation(buffer, f));
    plusWriteBufferSize(ops_.back().writeBufferSize());
    return ops_.back();
}

inline
AsioWriteOperation& AsioWriteOperationQueue::offer(const AsioWriteOperation& operation) {
    ops_.push_back(operation);
    plusWriteBufferSize(ops_.back().writeBufferSize());
    return ops_.back();
}

}
}
}

#endif //#if !defined(CETTY_CHANNEL_SOCKET_ASIO_ASIOWRITEREQUESTQUEUE_H)
