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

#include <cetty/buffer/ChannelBuffers.h>
#include <cetty/buffer/SlicedChannelBuffer.h>
#include <cetty/buffer/GatheringBuffer.h>

#include <cetty/util/InputStream.h>
#include <cetty/util/OutputStream.h>
#include <cetty/util/Exception.h>
#include <cetty/util/NestedDiagnosticContext.h>

namespace cetty {
namespace buffer {

using namespace cetty::util;

HeapChannelBuffer::HeapChannelBuffer(int length, int aheadBytes)
    : maintainArrayBuffer(true) {
    if (length < 0 || aheadBytes < 0) {
        CETTY_NDC_SCOPE(HeapChannelBuffer);
        throw InvalidArgumentException("length must greater than 0.");
    }

    if (length > 0) {
        arry = Array(new char[length + aheadBytes], length + aheadBytes);
        ChannelBuffer::setIndex(aheadBytes, aheadBytes);
    }
    else {
        arry = Array(new char[1], 0);
    }
}

HeapChannelBuffer::HeapChannelBuffer(char* buf, int length)
    : ChannelBuffer(0, length), maintainArrayBuffer(false), arry(buf, length) {
}

HeapChannelBuffer::HeapChannelBuffer(const Array& arry)
    : ChannelBuffer(0, arry.length()), maintainArrayBuffer(false), arry(arry) {
}

HeapChannelBuffer::HeapChannelBuffer(const Array& arry, bool maintainedBuf)
    : ChannelBuffer(0, arry.length()),
      maintainArrayBuffer(maintainedBuf),
      arry(arry) {
}

HeapChannelBuffer::HeapChannelBuffer(const Array& arry, int readerIndex, int writerIndex)
    :  maintainArrayBuffer(false), arry(arry) {
    setIndex(readerIndex, writerIndex);
}

HeapChannelBuffer::~HeapChannelBuffer() {
    if (maintainArrayBuffer && arry.data()) {
        delete[] arry.data();
    }
}

int HeapChannelBuffer::capacity() const {
    return this->arry.length();
}

bool HeapChannelBuffer::hasArray() const {
    return true;
}

void HeapChannelBuffer::readableBytes(Array* arry) {
    if (arry) {
        arry->reset(this->arry.data(readerIdx), writerIdx - readerIdx);
    }
}

void HeapChannelBuffer::writableBytes(Array* arry) {
    if (arry) {
        arry->reset(this->arry.data(writerIdx), this->arry.length() - writerIdx);
    }
}

void HeapChannelBuffer::aheadWritableBytes(Array* arry) {
    if (arry) {
        arry->reset(this->arry.data(), readerIdx);
    }
}

boost::int8_t HeapChannelBuffer::getByte(int index) const {
    return this->arry[index];
}

int HeapChannelBuffer::getBytes(int index, const ChannelBufferPtr& dst, int dstIndex, int length) const {
    boost::intrusive_ptr<HeapChannelBuffer> dstBuffer =
        boost::dynamic_pointer_cast<HeapChannelBuffer>(dst);

    if (dstBuffer) {
        return getBytes(index, &(dstBuffer->arry), dstIndex, length);
    }
    else {
        return dst->setBytes(dstIndex, this->arry, index, length);
    }
}

int HeapChannelBuffer::getBytes(int index, Array* dst, int dstIndex, int length) const {
    if (dst) {
        Array::copy(*dst, dstIndex, this->arry, index, length);
        return length;
    }

    return 0;
}

int HeapChannelBuffer::getBytes(int index, OutputStream* out, int length) const {
    if (out) {
        return out->write((const boost::int8_t*)arry.data(), index, length);
    }

    return 0;
}

int HeapChannelBuffer::setByte(int index, int value) {
    this->arry[index] = (char)value;
    return 1;
}

int HeapChannelBuffer::setBytes(int index, const ConstChannelBufferPtr& src, int srcIndex, int length) {
    if (!src) {
        return 0;
    }

    boost::intrusive_ptr<HeapChannelBuffer const> heap =
        boost::dynamic_pointer_cast<HeapChannelBuffer const>(src);

    if (heap) {
        return setBytes(index, heap->arry, srcIndex, length);
    }
    else {
        src->getBytes(srcIndex, &(this->arry), index, length);
        return length;
    }
}

int HeapChannelBuffer::setBytes(int index, const ConstArray& src, int srcIndex, int length) {
    Array::copy(this->arry, index, src, srcIndex, length);
    return length;
}

int HeapChannelBuffer::setBytes(int index, InputStream* in, int length) {
    int readBytes = 0;

    if (!in) {
        return readBytes;
    }

    do {
        int localReadBytes = in->read((boost::int8_t*)arry.data(), index, length);

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

cetty::buffer::ChannelBufferPtr HeapChannelBuffer::slice(int index, int length) {
    if (index == 0) {
        if (length <= 0) {
            return ChannelBuffers::EMPTY_BUFFER;
        }

        if (length == this->arry.length()) {
            ChannelBufferPtr slice = duplicate();
            slice->setIndex(0, length);
            return slice;
        }
        else {
            return ChannelBufferPtr(
                       new SlicedChannelBuffer(shared_from_this(), 0, length));
        }
    }
    else {
        if (length <= 0) {
            return ChannelBuffers::EMPTY_BUFFER;
        }

        return ChannelBufferPtr(new SlicedChannelBuffer(shared_from_this(), index, length));
    }
}

int HeapChannelBuffer::slice(int index, int length, GatheringBuffer* gatheringBuffer) {
    if (gatheringBuffer && length > 0) {

        if (index < 0 || index + length > arry.length()) {
            CETTY_NDC_SCOPE("slice(int, int, GatheringBuffer*)");
            throw RangeException("slice to gathering buffer out of range.");
        }

        gatheringBuffer->append(arry.data(index), length);
        return length;
    }

    return 0;
}

}
}
