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
#include <cetty/channel/ChannelFuture.h>
#include <cetty/channel/socket/asio/AsioGatheringBuffer.h>

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
    namespace logging {
        class InternalLogger;
    }
}

namespace cetty {
namespace channel {
namespace socket {
namespace asio {

using namespace cetty::buffer;
using namespace cetty::channel;
using namespace cetty::logging;

class AsioSocketChannel;

class AsioWriteOperation {
public:
    typedef AsioGatheringBuffer::AsioBuffer AsioBuffer;
    typedef AsioGatheringBuffer::AsioBufferArray AsioBufferArray;

public:
    AsioWriteOperation()  {}

    AsioWriteOperation(const MessageEvent& evt, const ChannelFuturePtr& f)
        : future(f) {
            gathering.mergeFrom(evt);
    }

    AsioWriteOperation(const AsioWriteOperation& op)
        : gathering(op.gathering),
          future(op.future) {
    }

    AsioWriteOperation& operator=(const AsioWriteOperation& op) {
        gathering = op.gathering;
        future = op.future;
        return *this;
    }

    ~AsioWriteOperation() {}

    int writeBufferSize() const {
        return gathering.bytesCount();
    }

    bool isMultiBuffers() const {
        return gathering.isMultiBuffers();
    }

    AsioBuffer& getAsioBuffer() {
        return gathering.getAsioBuffer();
    }

    AsioBufferArray& getAsioBufferArray() {
        return gathering.getAsioBufferArray();
    }

    bool setSuccess() {
        return future ? future->setSuccess() : false;
    }
    bool setFailure(const Exception& cause) {
        return future ? future->setFailure(cause) : false;
    }

private:
    AsioGatheringBuffer gathering;
    ChannelFuturePtr    future;
};

// no need ensure the thread safe.
class AsioWriteOperationQueue {
public:
    AsioWriteOperationQueue(AsioSocketChannel& channel, InternalLogger* logger)
        : channel(channel), logger(logger), writeBufferSize(0) {}

    ~AsioWriteOperationQueue() {}

    bool  empty() const { return ops.empty(); }
    size_t size() const { return ops.size(); }

    AsioWriteOperation& peek() {
        return ops.front();
    }

    AsioWriteOperation& poll() {
        polledOp = ops.front();
        ops.pop_front();
        minusWriteBufferSize(polledOp.writeBufferSize());
        return polledOp;
    }

    AsioWriteOperation& offer(const MessageEvent& evt, const ChannelFuturePtr& f) {
        ops.push_back(AsioWriteOperation(evt, f));
        plusWriteBufferSize(ops.back().writeBufferSize());
		return ops.back();
    }

    int  getWriteBufferSize() const {
        return writeBufferSize;
    }

private:
    void plusWriteBufferSize(int messageSize);
    void minusWriteBufferSize(int messageSize);

private:
    AsioSocketChannel& channel;
    InternalLogger* logger;

    int writeBufferSize;

    AsioWriteOperation polledOp;
    std::deque<AsioWriteOperation> ops;
};

}
}
}
}

#endif //#if !defined(CETTY_CHANNEL_SOCKET_ASIO_ASIOWRITEREQUESTQUEUE_H)
