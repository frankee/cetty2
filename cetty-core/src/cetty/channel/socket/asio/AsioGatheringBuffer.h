#if !defined(CETTY_CHANNEL_SOCKET_ASIO_ASIOGATHERINGBUFFER_H)
#define CETTY_CHANNEL_SOCKET_ASIO_ASIOGATHERINGBUFFER_H

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

#include <boost/asio/buffer.hpp>
#include <cetty/buffer/GatheringBuffer.h>
#include <cetty/buffer/ChannelBufferFwd.h>
#include <cetty/util/TruncatableArray.h>

namespace cetty { namespace channel {
    class MessageEvent;
}}

namespace cetty { namespace channel { namespace socket { namespace asio { 

    using namespace cetty::buffer;

class AsioGatheringBuffer : public cetty::buffer::GatheringBuffer {
public:
    const static int  MAX_BUFFER_COUNT = 8;

    typedef boost::asio::mutable_buffer AsioBuffer;
    typedef cetty::util::TruncatableArray<AsioBuffer, MAX_BUFFER_COUNT> AsioBufferArray;

public:
    AsioGatheringBuffer() : byteSize(0) {}
    AsioGatheringBuffer(const AsioGatheringBuffer& gathering)
        : byteSize(gathering.byteSize),
        channelBuffer(gathering.channelBuffer),
        buffers(gathering.buffers) {
    }
    AsioGatheringBuffer& operator=(const AsioGatheringBuffer& gathering) {
        byteSize = gathering.byteSize;
        channelBuffer = gathering.channelBuffer;
        buffers = gathering.buffers;
        return *this;
    }

    virtual ~AsioGatheringBuffer() {}

    virtual bool empty() const;
    virtual int  blockCount() const;
    virtual int  bytesCount() const;

    virtual void clear();

    virtual void append(char* data, int size);
    virtual std::pair<char*, int> at(int index);

    bool isMultiBuffers() const {
        return buffers.truncatedIndex > 1;
    }

    AsioBuffer& getAsioBuffer() {
        return buffers[0];
    }
    
    AsioBufferArray& getAsioBufferArray() {
        return buffers;
    }

    void mergeFrom(const MessageEvent& evt);

private:
    mutable int byteSize;
    ChannelBufferPtr channelBuffer;
    
    AsioBufferArray buffers;
};

}}}}

#endif //#if !defined(CETTY_CHANNEL_SOCKET_ASIO_ASIOGATHERINGBUFFER_H)
