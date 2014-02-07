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

#include <cetty/buffer/HeapChannelBuffer.h>

#include <cetty/buffer/Unpooled.h>
#include <cetty/buffer/GatheringBuffer.h>
#include <cetty/buffer/SlicedChannelBuffer.h>

#include <cetty/util/InputStream.h>
#include <cetty/util/OutputStream.h>
#include <cetty/util/Exception.h>
#include <cetty/util/StringUtil.h>
#include <cetty/util/StringPiece.h>
#include <cetty/util/NestedDiagnosticContext.h>

#include <cetty/logging/LoggerHelper.h>

namespace cetty {
namespace buffer {

using namespace cetty::util;

HeapChannelBuffer::HeapChannelBuffer(int initialCapacity, int aheadBytes, int maximunCapacity)
    : ChannelBuffer(maximunCapacity, ByteOrder::BIG),
      ownBuffer(true),
      buf(NULL),
      bufSize(0) {
    if (initialCapacity < 0
            || initialCapacity > maximunCapacity
            || aheadBytes < 0) {
        CETTY_NDC_SCOPE();
        throw InvalidArgumentException("length must greater than 0.");
    }

    if (initialCapacity > 0) {
        bufSize = initialCapacity + aheadBytes;
        buf = new char[bufSize];

        ChannelBuffer::setIndex(aheadBytes, aheadBytes);
    }
    else {
        bufSize = 1;
        buf = new char[bufSize];
    }
}

HeapChannelBuffer::HeapChannelBuffer(char* buf, int length)
    : ChannelBuffer(length, ByteOrder::BIG),
      ownBuffer(false),
      buf(NULL),
      bufSize(0)  {
    init(buf, length, length, 0, length);
}

HeapChannelBuffer::HeapChannelBuffer(char* buf, int length, int maximunCapacity, bool takeCare)
    : ChannelBuffer(maximunCapacity, ByteOrder::BIG),
      ownBuffer(takeCare),
      buf(NULL),
      bufSize(0)  {
    init(buf, length, maximunCapacity, 0, length);
}

HeapChannelBuffer::HeapChannelBuffer(char* buf,
                                     int length,
                                     int readerIndex,
                                     int writerIndex,
                                     int maximunCapacity,
                                     bool takeCare)
    : ChannelBuffer(maximunCapacity, ByteOrder::BIG),
      ownBuffer(takeCare) {
    init(buf, length, maximunCapacity, readerIndex, writerIndex);
}

HeapChannelBuffer::~HeapChannelBuffer() {
    if (ownBuffer && buf) {
        delete[] buf;
    }
}

int HeapChannelBuffer::capacity() const {
    return bufSize;
}

void HeapChannelBuffer::capacity(int newCapacity) {
    if (newCapacity < 0 || newCapacity > maxCapacity()) {
        LOG_ERROR << "newCapacity: " << newCapacity << " is illegal";
        return;
    }

    int readerIdx = this->readerIdx;
    int writerIdx = this->writerIdx;

    int oldCapacity = bufSize;

    if (newCapacity > oldCapacity) {
        char* newArray = new char[newCapacity];
        memcpy(newArray + readerIdx, buf + readerIdx, writerIdx - readerIdx);

        this->buf = newArray;
        this->bufSize = newCapacity;
    }
    else if (newCapacity < oldCapacity) {
        char* newArray = new char[newCapacity];
        int readerIndex = this->readerIdx;

        if (readerIndex < newCapacity) {
            if (writerIdx > newCapacity) {
                writerIdx = newCapacity;
                writerIndex(writerIdx);
            }

            memcpy(newArray + readerIdx, buf + readerIdx, writerIdx - readerIdx);
        }
        else {
            setIndex(newCapacity, newCapacity);
        }

        buf = newArray;
        bufSize = newCapacity;
    }
}

const char* HeapChannelBuffer::readableBytes(int* length) {
    if (length) {
        *length = writerIdx - readerIdx;
    }

    return buf + readerIdx;
}

char* HeapChannelBuffer::writableBytes(int* length) {
    if (length) {
        *length = bufSize - writerIdx;
    }

    return buf + writerIdx;
}

char* HeapChannelBuffer::aheadWritableBytes(int* length) {
    if (length) {
        *length = readerIdx;
    }

    return buf;
}

int8_t HeapChannelBuffer::getByte(int index) const {
    if (index < 0 || index >= bufSize) {
        CETTY_NDC_SCOPE();
        LOG_ERROR << "RangeException, getByte(" << index << ")"; 
        throw RangeException(StringUtil::printf("getByte with %d", index));
    }

    return buf[index];
}

int16_t HeapChannelBuffer::getShort(int index) const {
    if (index < 0 || index + 1 >= bufSize) {
        CETTY_NDC_SCOPE();
        LOG_ERROR << "RangeException, getShort(" << index << ")"; 
        throw RangeException(StringUtil::printf("getShort with %d", index));
    }

    return static_cast<int16_t>((buf[index] << 8) | (buf[index + 1] & 0xFF));
}

int32_t HeapChannelBuffer::getInt(int index) const {
    if (index < 0 || index + 3 >= bufSize) {
        CETTY_NDC_SCOPE();
        LOG_ERROR << "RangeException, getInt(" << index << ")"; 
        throw RangeException(StringUtil::printf("getInt with %d", index));
    }

    return ((buf[index]   & 0xff) << 24) |
           ((buf[index+1] & 0xff) << 16) |
           ((buf[index+2] & 0xff) <<  8) |
           ((buf[index+3] & 0xff) <<  0);
}

int64_t HeapChannelBuffer::getLong(int index) const {
    if (index < 0 || index + 7 >= bufSize) {
        CETTY_NDC_SCOPE();
        LOG_ERROR << "RangeException, getLong(" << index << ")"; 
        throw RangeException(StringUtil::printf("getLong with %d", index));
    }

    return ((static_cast<int64_t>(buf[index])  & 0xff) << 56) |
           ((static_cast<int64_t>(buf[index+1]) & 0xff) << 48) |
           ((static_cast<int64_t>(buf[index+2]) & 0xff) << 40) |
           ((static_cast<int64_t>(buf[index+3]) & 0xff) << 32) |
           ((static_cast<int64_t>(buf[index+4]) & 0xff) << 24) |
           ((static_cast<int64_t>(buf[index+5]) & 0xff) << 16) |
           ((static_cast<int64_t>(buf[index+6]) & 0xff) <<  8) |
           ((static_cast<int64_t>(buf[index+7]) & 0xff) <<  0);
}

int HeapChannelBuffer::getBytes(int index, const ChannelBufferPtr& dst, int dstIndex, int length) const {
    boost::intrusive_ptr<HeapChannelBuffer> dstBuffer =
        boost::dynamic_pointer_cast<HeapChannelBuffer>(dst);

    if (dstBuffer) {
        if (length < 0 || index < 0 || dstIndex < 0 || index + length > bufSize || dstIndex + length > dstBuffer->bufSize) {
            return 0;
        }
        else {
            memmove(dstBuffer->buf + dstIndex, buf + index, length);
            return length;
        }
    }
    else if (dst) {
        return dst->setBytes(dstIndex, buf + index, length);
    }

    return 0;
}

int HeapChannelBuffer::getBytes(int index, char* dst, int length) const {
    if (NULL == dst
        || index < 0
        || length < 0
        || index + length > bufSize) {
                CETTY_NDC_SCOPE();
                LOG_ERROR << "RangeException, getBytes("
                    << index << ", char*, " << length << ")";
                throw RangeException("getBytes");
    }

    memcpy(dst, buf + index, length);
    return length;
}

int HeapChannelBuffer::getBytes(int index, OutputStream* out, int length) const {
    //if (out) {
    //    return out->write(reinterpret_cast<const int8_t*>(buf), index, length);
    //}

    return 0;
}

int HeapChannelBuffer::setByte(int index, int value) {
    if (index < 0 || index >= bufSize) {
        CETTY_NDC_SCOPE();
        LOG_ERROR << "RangeException, setByte(" << index << ")"; 
        throw RangeException(StringUtil::printf("setByte with %d", index));
    }

    buf[index] = static_cast<char>(value);
    return 1;
}

int HeapChannelBuffer::setShort(int index, int value) {
    if (index < 0 || index + 1 >= bufSize) {
        CETTY_NDC_SCOPE();
        LOG_ERROR << "RangeException, setShort(" << index << ")"; 
        throw RangeException(StringUtil::printf("setShort with %d", index));
    }

    buf[index  ] = static_cast<int8_t>(value >> 8);
    buf[index+1] = static_cast<int8_t>(value >> 0);

    return 2;
}

int HeapChannelBuffer::setInt(int index, int value) {
    if (index < 0 || index + 3 >= bufSize) {
        CETTY_NDC_SCOPE();
        LOG_ERROR << "RangeException, setInt(" << index << ")"; 
        throw RangeException(StringUtil::printf("setInt with %d", index));
    }

    buf[index  ] = static_cast<int8_t>(value >> 24);
    buf[index+1] = static_cast<int8_t>(value >> 16);
    buf[index+2] = static_cast<int8_t>(value >> 8);
    buf[index+3] = static_cast<int8_t>(value >> 0);

    return 4;
}

int HeapChannelBuffer::setLong(int index, int64_t value) {
    if (index < 0 || index + 7 >= bufSize) {
        LOG_ERROR << "RangeException, setLong(" << index << ")"; 
        return 0;
    }

    buf[index  ] = static_cast<int8_t>(value >> 56);
    buf[index+1] = static_cast<int8_t>(value >> 48);
    buf[index+2] = static_cast<int8_t>(value >> 40);
    buf[index+3] = static_cast<int8_t>(value >> 32);
    buf[index+4] = static_cast<int8_t>(value >> 24);
    buf[index+5] = static_cast<int8_t>(value >> 16);
    buf[index+6] = static_cast<int8_t>(value >>  8);
    buf[index+7] = static_cast<int8_t>(value >>  0);

    return 8;
}

int HeapChannelBuffer::setBytes(int index, const ConstChannelBufferPtr& src, int srcIndex, int length) {
    if (!src) {
        return 0;
    }

    boost::intrusive_ptr<HeapChannelBuffer const> heap =
        boost::dynamic_pointer_cast<HeapChannelBuffer const>(src);

    if (heap) {
        if (length < 0 || index < 0 || srcIndex < 0 || index + length > bufSize || srcIndex + length > heap->bufSize) {
            return 0;
        }
        else {
            memmove(buf + index, heap->buf + srcIndex, length);
            return length;
        }
    }
    else {
        src->getBytes(srcIndex, buf + index, length);
        return length;
    }
}

int HeapChannelBuffer::setBytes(int index, const char* src, int length) {
    if (NULL == src
        || 0 == length
        || index < 0
        || length < 0
        || index + length > bufSize) {
            CETTY_NDC_SCOPE();
            LOG_ERROR << "RangeException, getBytes("
                << index << ", char*, " << length << ")";
            throw RangeException("getBytes");
    }

    memcpy(buf + index, src, length);
    return length;
}

int HeapChannelBuffer::setBytes(int index, InputStream* in, int length) {
    int readBytes = 0;

    if (!in) {
        return readBytes;
    }

    do {
        int localReadBytes = in->read((int8_t*)buf, index, length);

        if (localReadBytes < 0) {
            if (readBytes == 0) {
                return -1;
            }
            else {
                break;
            }
        }

        readBytes += localReadBytes;
        index += localReadBytes;
        length -= localReadBytes;
    }
    while (length > 0);

    return readBytes;
}

ChannelBufferPtr HeapChannelBuffer::slice(int index, int length) {
    if (index < 0 || length <= 0) {
        return Unpooled::EMPTY_BUFFER;
    }

    return new SlicedChannelBuffer(shared_from_this(), index, length);
}

int HeapChannelBuffer::slice(int index, int length, GatheringBuffer* gatheringBuffer) {
    if (gatheringBuffer && length > 0) {

        if (index < 0 || index + length > bufSize) {
            CETTY_NDC_SCOPE();
            LOG_ERROR << "RangeException, slice("<< index << ", " << length << ")";
            throw RangeException("slice to gathering buffer out of range.");
        }

        gatheringBuffer->append(buf + index, length);
        return length;
    }

    return 0;
}

void HeapChannelBuffer::init(char* buf, int length, int maxCapacity, int readerIndex, int writerIndex) {
    if (!buf) {
        throw InvalidArgumentException("input buf can not be NULL.");
    }

    if (length <= 0 || length > maxCapacity) {
        CETTY_NDC_SCOPE();
        std::string msg;
        StringUtil::printf(&msg,
            "length(%d) must greater than 0 and less then maxCapacity(%d).",
            length,
            maxCapacity);

        LOG_ERROR << "RangeException: " << msg;
        throw InvalidArgumentException(msg);
    }

    this->buf = buf;
    this->bufSize = length;

    setIndex(readerIndex, writerIndex);
}

ChannelBufferPtr HeapChannelBuffer::copy(int index, int length) const {
    if (index < 0 || length < 0 || index + length > bufSize) {
        CETTY_NDC_SCOPE();
        LOG_ERROR << "RangeException, copy(" << index << ", " << length << ");";
        throw RangeException("copy out of range.");
    }

    char* copiedBytes = new char[length];
    memcpy(copiedBytes, buf + index, length);
    return ChannelBufferPtr(
               new HeapChannelBuffer(copiedBytes, length, maximumCapacity, true));
}

}
}
