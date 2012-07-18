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

#include <cetty/handler/codec/replay/ReplayingDecoderBuffer.h>
#include <cetty/handler/codec/replay/UnreplayableOperationException.h>

#include <cetty/buffer/ChannelBuffers.h>
#include <cetty/util/Integer.h>
#include <cetty/util/StringUtil.h>

namespace cetty {
namespace handler {
namespace codec {
namespace replay {

using namespace cetty::buffer;
using namespace cetty::util;

int ReplayingDecoderBuffer::capacity() const {
    if (terminated) {
        return buffer->capacity();
    }
    else {
        return Integer::MAX_VALUE;
    }
}

bool ReplayingDecoderBuffer::hasArray() const {
    return buffer->hasArray();
}

void ReplayingDecoderBuffer::clear() {
    throw UnreplayableOperationException();
}

cetty::buffer::ChannelBufferPtr ReplayingDecoderBuffer::copy(int index, int length) const {
    if (checkIndex(index, length)) {
        return buffer->copy(index, length);
    }

    return ChannelBuffers::EMPTY_BUFFER;
}

void ReplayingDecoderBuffer::discardReadBytes() {
    throw UnreplayableOperationException();
}

void ReplayingDecoderBuffer::ensureWritableBytes(int writableBytes) {
    throw UnreplayableOperationException();
}

boost::int8_t ReplayingDecoderBuffer::getByte(int index) const {
    if (checkIndex(index, 1)) {
        return buffer->getByte(index);
    }

    return 0;
}

int ReplayingDecoderBuffer::getBytes(int index, Array* dst, int dstIndex, int length) const {
    if (checkIndex(index, length)) {
        return buffer->getBytes(index, dst, dstIndex, length);
    }

    return 0;
}

int ReplayingDecoderBuffer::getBytes(int index, const ChannelBufferPtr& dst, int dstIndex, int length) const {
    if (checkIndex(index, length)) {
        return buffer->getBytes(index, dst, dstIndex, length);
    }

    return 0;
}

int ReplayingDecoderBuffer::getBytes(int index, OutputStream* out, int length) const {
    throw UnreplayableOperationException();
}

boost::int32_t ReplayingDecoderBuffer::getInt(int index) const {
    if (checkIndex(index, 4)) {
        return buffer->getInt(index);
    }

    return 0;
}

boost::int64_t ReplayingDecoderBuffer::getLong(int index) const {
    if (checkIndex(index, 8)) {
        return buffer->getLong(index);
    }

    return 0;
}

boost::int16_t ReplayingDecoderBuffer::getShort(int index) const {
    if (checkIndex(index, 2)) {
        return buffer->getShort(index);
    }

    return 0;
}

int ReplayingDecoderBuffer::indexOf(int fromIndex, int toIndex, boost::int8_t value) const {
    int endIndex = buffer->indexOf(fromIndex, toIndex, value);
    needMore = (endIndex < 0);

    return endIndex;
}

int ReplayingDecoderBuffer::indexOf(int fromIndex, int toIndex, const ChannelBufferIndexFinder::Functor& indexFinder) const {
    int endIndex = buffer->indexOf(fromIndex, toIndex, indexFinder);
    needMore = (endIndex < 0);

    return endIndex;
}

ChannelBufferFactory& ReplayingDecoderBuffer::factory() const {
    return buffer->factory();
}

cetty::buffer::ByteOrder ReplayingDecoderBuffer::order() const {
    return buffer->order();
}

int ReplayingDecoderBuffer::readableBytes() const {
    if (terminated) {
        return buffer->readableBytes();
    }
    else {
        return Integer::MAX_VALUE - buffer->readerIndex();
    }
}

void ReplayingDecoderBuffer::readableBytes(Array* arry) {
    return buffer->readableBytes(arry);
}

int ReplayingDecoderBuffer::setByte(int index, int value) {
    throw UnreplayableOperationException();
}

int ReplayingDecoderBuffer::setBytes(int index, const ConstArray& src, int srcIndex, int length) {
    throw UnreplayableOperationException();
}

int ReplayingDecoderBuffer::setBytes(int index, const ConstChannelBufferPtr& src, int srcIndex, int length) {
    throw UnreplayableOperationException();
}

int ReplayingDecoderBuffer::setBytes(int index, InputStream* in, int length) {
    throw UnreplayableOperationException();
}

int ReplayingDecoderBuffer::setInt(int index, int value) {
    throw UnreplayableOperationException();
}

int ReplayingDecoderBuffer::setLong(int index, boost::int64_t value) {
    throw UnreplayableOperationException();
}

int ReplayingDecoderBuffer::setShort(int index, int value) {
    throw UnreplayableOperationException();
}

ChannelBufferPtr ReplayingDecoderBuffer::slice(int index, int length) {
    if (checkIndex(index, length)) {
        return buffer->slice(index, length);
    }

    return ChannelBuffers::EMPTY_BUFFER;
}

int ReplayingDecoderBuffer::slice(int index, int length, GatheringBuffer* gatheringBuffer) {
    if (checkIndex(index, length)) {
        return buffer->slice(index, length, gatheringBuffer);
    }

    return 0;
}

std::string ReplayingDecoderBuffer::toString() const {
    return StringUtil::strprintf("ReplayingDecoderBuffer (ridx=%d, widx=%d)",
                                 readerIndex(), writerIndex());
}

void ReplayingDecoderBuffer::writableBytes(Array* arry) {
    throw UnreplayableOperationException();
}

int ReplayingDecoderBuffer::writableBytes() const {
    throw UnreplayableOperationException();
}

int ReplayingDecoderBuffer::aheadWritableBytes() const {
    throw UnreplayableOperationException();
}

void ReplayingDecoderBuffer::aheadWritableBytes(Array* arry) {
    throw UnreplayableOperationException();
}

bool ReplayingDecoderBuffer::checkIndex(int index, int length) const {
    if (index + length <= buffer->writerIndex()) {
        needMore = false;
        return true;
    }

    needMore = true;
    return false;
}

bool ReplayingDecoderBuffer::checkReadableBytes(int readableBytes) const {
    if (buffer->readableBytes() >= readableBytes) {
        needMore = false;
        return true;
    }

    needMore = true;
    return false;
}

}
}
}
}
