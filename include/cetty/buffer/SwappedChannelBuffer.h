#if !defined(CETTY_BUFFER_SWAPPEDCHANNELBUFFER_H)
#define CETTY_BUFFER_SWAPPEDCHANNELBUFFER_H
/*
 * Copyright 2012 The Netty Project
 *
 * The Netty Project licenses this file to you under the Apache License,
 * version 2.0 (the "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at:
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations
 * under the License.
 */
/*
 * Copyright (c) 2010-2012 frankee zhou (frankee.zhou at gmail dot com)
 *
 * Distributed under under the Apache License, version 2.0 (the "License").
 *
 */

#include <cetty/buffer/ChannelBuffer.h>
#include <cetty/buffer/WrappedChannelBuffer.h>
#include <cetty/buffer/ChannelBufferUtil.h>

#include <cetty/util/Exception.h>

namespace cetty {
namespace buffer {

using namespace cetty::util;

class SwappedChannelBuffer : public ChannelBuffer, public WrappedChannelBuffer {
public:
    SwappedChannelBuffer(const ChannelBufferPtr& buf) {
        if (!buf) {
            throw NullPointerException("buf");
        }

        if (buf->order() == ByteOrder::BO_BIG_ENDIAN) {
            this->buf = buf;
            this->byteOrder = ByteOrder::BO_LITTLE_ENDIAN;
        }
        else {
            boost::intrusive_ptr<SwappedChannelBuffer> swapped
                = boost::dynamic_pointer_cast<SwappedChannelBuffer>(buf);

            this->buf = swapped->unwrap();
            this->byteOrder = ByteOrder::BO_LITTLE_ENDIAN;
        }
    }

    virtual ChannelBufferPtr order(const ByteOrder& endianness) {
        if (endianness == this->byteOrder) {
            return shared_from_this();
        }

        return this->buf;
    }

    virtual const ChannelBufferPtr& unwrap() {
        return this->buf;
    }

    virtual int capacity() const {
        return buf->capacity();
    }

    virtual void capacity(int newCapacity) {
        buf->capacity(newCapacity);
    }

    virtual void readableBytes(StringPiece* bytes) {
        buf->readableBytes(bytes);
    }
    virtual char* writableBytes(int* length) {
        return buf->writableBytes(length);
    }
    virtual char* aheadWritableBytes(int* length) {
        return buf->aheadWritableBytes(length);
    }

    virtual int8_t  getByte(int index) const {
        return buf->getByte(index);
    }
    virtual int16_t getShort(int index) const {
        return ChannelBufferUtil::swapShort(buf->getShort(index));
    }
    virtual int32_t getInt(int index) const {
        return ChannelBufferUtil::swapInt(buf->getInt(index));
    }
    virtual int64_t getLong(int index) const {
        return ChannelBufferUtil::swapLong(buf->getLong(index));
    }

    virtual int getBytes(int index, const ChannelBufferPtr& dst, int dstIndex, int length) const {
        return buf->getBytes(index, dst, dstIndex, length);
    }
    virtual int getBytes(int index, char* dst, int dstIndex, int length) const {
        return buf->getBytes(index, dst, dstIndex, length);
    }
    virtual int getBytes(int index, OutputStream* out, int length) const {
        return buf->getBytes(index, out, length);
    }

    virtual int setByte(int index, int value) {
        return buf->setByte(index, value);
    }
    virtual int setShort(int index, int value) {
        return buf->setShort(index, ChannelBufferUtil::swapShort(value));
    }
    virtual int setInt(int index, int value) {
        return buf->setInt(index, ChannelBufferUtil::swapInt(value));
    }
    virtual int setLong(int index, int64_t value) {
        return buf->setDouble(index, ChannelBufferUtil::swapLong(value));
    }

    virtual int setBytes(int index, const StringPiece& src, int srcIndex, int length){
        return buf->setBytes(index, src, srcIndex, length);
    }
    virtual int setBytes(int index, const ConstChannelBufferPtr& src, int srcIndex, int length) {
        return buf->setBytes(index, src, srcIndex, length);
    }
    virtual int setBytes(int index, InputStream* in, int length) {
        return buf->setBytes(index, in, length);
    }

    virtual ChannelBufferPtr slice(int index, int length) {
        return buf->slice(index, length);
    }
    virtual int slice(int index, int length, GatheringBuffer* gathering) {
        return buf->slice(index, length, gathering);
    }

    virtual ChannelBufferPtr copy(int index, int length) const {
        return buf->copy(index, length);
    }

    virtual ChannelBufferPtr newBuffer(int initialCapacity) {
        return buf->newBuffer(initialCapacity);
    }

    //String toString() {
    //    return "Swapped(" + buf.toString() + ')';
    //}
private:
    ChannelBufferPtr buf;
};


}
}

#endif //#if !defined(CETTY_BUFFER_SWAPPEDCHANNELBUFFER_H)

// Local Variables:
// mode: c++
// End:
