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

#include <cetty/buffer/DynamicChannelBuffer.h>
#include <cetty/buffer/ChannelBuffers.h>
#include <cetty/buffer/SlicedChannelBuffer.h>
#include <cetty/buffer/HeapChannelBufferFactory.h>

#include <cetty/util/Exception.h>
#include <cetty/util/NestedDiagnosticContext.h>

namespace cetty {
namespace buffer {

using namespace cetty::util;

DynamicChannelBuffer::DynamicChannelBuffer(int estimatedLength)
    : endianness(ByteOrder::BIG_ENDIAN),
      bufferFactory(HeapChannelBufferFactory::getInstance(ByteOrder::BIG_ENDIAN)) {
    if (estimatedLength < 0) {
        throw InvalidArgumentException("estimatedLength is negtive: " /*+ estimatedLength*/);
    }

    buffer = bufferFactory.getBuffer(endianness, estimatedLength);
}

DynamicChannelBuffer::DynamicChannelBuffer(ByteOrder endianness, int estimatedLength)
    : endianness(endianness),
      bufferFactory(HeapChannelBufferFactory::getInstance(endianness)) {
    if (estimatedLength < 0) {
        throw InvalidArgumentException("estimatedLength is negative.");
    }

    buffer = bufferFactory.getBuffer(endianness, estimatedLength);
}

DynamicChannelBuffer::DynamicChannelBuffer(ByteOrder endianness, int estimatedLength, ChannelBufferFactory& factory)
    : endianness(endianness), bufferFactory(factory) {
    if (estimatedLength < 0) {
        throw InvalidArgumentException("estimatedLength is negative: ");
    }

    buffer = bufferFactory.getBuffer(endianness, estimatedLength);
}

void DynamicChannelBuffer::ensureWritableBytes(int minWritableBytes) {
    if (minWritableBytes <= ChannelBuffer::writableBytes()) {
        return;
    }

    int newCapacity;

    if (capacity() == 0) {
        newCapacity = 1;
    }
    else {
        newCapacity = capacity();
    }

    int minNewCapacity = writerIndex() + minWritableBytes;

    while (newCapacity < minNewCapacity) {
        newCapacity <<= 1;
    }

    ChannelBufferPtr newBuffer = factory().getBuffer(order(), newCapacity);
    newBuffer->writeBytes(buffer, 0, writerIndex());
    buffer.swap(newBuffer);
}

ChannelBufferFactory& DynamicChannelBuffer::factory() const {
    return this->bufferFactory;
}

cetty::buffer::ByteOrder DynamicChannelBuffer::order() const {
    return endianness;
}

int DynamicChannelBuffer::capacity() const {
    return buffer->capacity();
}

void DynamicChannelBuffer::readableBytes(Array* arry) {
    buffer->readableBytes(arry);
}

void DynamicChannelBuffer::writableBytes(Array* arry) {
    buffer->writableBytes(arry);
}

void DynamicChannelBuffer::aheadWritableBytes(Array* arry) {
    buffer->aheadWritableBytes(arry);
}

bool DynamicChannelBuffer::hasArray() const {
    return buffer->hasArray();
}

boost::int8_t DynamicChannelBuffer::getByte(int index) const {
    return buffer->getByte(index);
}

boost::int16_t DynamicChannelBuffer::getShort(int index) const {
    return buffer->getShort(index);
}

boost::int32_t DynamicChannelBuffer::getInt(int index) const {
    return buffer->getInt(index);
}

boost::int64_t DynamicChannelBuffer::getLong(int index) const {
    return buffer->getLong(index);
}

int DynamicChannelBuffer::getBytes(int index, Array* dst, int dstIndex, int length) const {
    return buffer->getBytes(index, dst, dstIndex, length);
}

int DynamicChannelBuffer::getBytes(int index, const ChannelBufferPtr& dst, int dstIndex, int length) const {
    return buffer->getBytes(index, dst, dstIndex, length);
}

int DynamicChannelBuffer::getBytes(int index, OutputStream* out, int length) const {
    return buffer->getBytes(index, out, length);
}

int DynamicChannelBuffer::setByte(int index, int value) {
    return buffer->setByte(index, value);
}

int DynamicChannelBuffer::setShort(int index, int value) {
    return buffer->setShort(index, value);
}

int DynamicChannelBuffer::setInt(int index, int value) {
    return buffer->setInt(index, value);
}

int DynamicChannelBuffer::setLong(int index, boost::int64_t value) {
    return buffer->setLong(index, value);
}

int DynamicChannelBuffer::setBytes(int index, const ConstArray& src, int srcIndex, int length) {
    return buffer->setBytes(index, src, srcIndex, length);
}

int DynamicChannelBuffer::setBytes(int index, const ConstChannelBufferPtr& src, int srcIndex, int length) {
    return buffer->setBytes(index, src, srcIndex, length);
}

int DynamicChannelBuffer::setBytes(int index, InputStream* in, int length) {
    return buffer->setBytes(index, in, length);
}

cetty::buffer::ChannelBufferPtr DynamicChannelBuffer::copy(int index, int length) const {
    DynamicChannelBuffer* copiedBuffer =
        new DynamicChannelBuffer(order(), length > 64 ? length : 64, factory());

    copiedBuffer->buffer = buffer->copy(index, length);
    copiedBuffer->setIndex(0, length);
    return ChannelBufferPtr(copiedBuffer);
}

cetty::buffer::ChannelBufferPtr DynamicChannelBuffer::slice(int index, int length) {
    if (index == 0) {
        if (length == 0) {
            return ChannelBuffers::EMPTY_BUFFER;
        }
    }
    else if (length == 0) {
        return ChannelBuffers::EMPTY_BUFFER;
    }

    return ChannelBufferPtr(new SlicedChannelBuffer(shared_from_this(), index, length));
}

int DynamicChannelBuffer::slice(int index, int length, GatheringBuffer* gatheringBuffer) {
    return buffer->slice(index, length, gatheringBuffer);
}

}
}
