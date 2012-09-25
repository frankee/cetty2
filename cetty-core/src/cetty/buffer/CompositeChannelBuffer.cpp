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

#include <cetty/buffer/CompositeChannelBuffer.h>

#include <boost/assert.hpp>

#include <cetty/buffer/Unpooled.h>
#include <cetty/buffer/GatheringBuffer.h>

#include <cetty/util/Exception.h>
#include <cetty/util/StringUtil.h>
#include <cetty/util/StringPiece.h>
#include <cetty/util/NestedDiagnosticContext.h>

namespace cetty {
namespace buffer {

using namespace cetty::util;

CompositeChannelBuffer::CompositeChannelBuffer(
    const std::vector<ChannelBufferPtr>& buffers) {
    setComponents(buffers);
}

CompositeChannelBuffer::CompositeChannelBuffer(CompositeChannelBuffer& buffer)
    : components(buffer.components),
    indices(buffer.indices),
    lastAccessedComponentId(0) {
    setIndex(buffer.readerIndex(), buffer.writerIndex());
}

std::vector<ChannelBufferPtr> CompositeChannelBuffer::decompose(int index, int length) {
    std::vector<ChannelBufferPtr> slice;

    if (length == 0) {
        return slice;
    }

    if (index + length > capacity()) {
        CETTY_NDC_SCOPE();
        throw RangeException("CompositeChannelBuffer decompose out of range.");
    }

    int componentId = getComponentId(index);

    // The first component
    ChannelBufferPtr first = components[componentId]->slice();
    first->readerIndex(index - indices[componentId]);

    ChannelBufferPtr buf = first;
    int bytesToSlice = length;

    do {
        int readableBytes = buf->readableBytes();

        if (bytesToSlice <= readableBytes) {
            // Last component
            buf->writerIndex(buf->readerIndex() + bytesToSlice);
            slice.push_back(buf);
            break;
        }
        else {
            // Not the last component
            slice.push_back(buf);
            bytesToSlice -= readableBytes;
            componentId ++;

            // Fetch the next component.
            buf = components[componentId]->slice();
        }
    }
    while (bytesToSlice > 0);

    // Slice all components because only readable bytes are interesting.
    for (size_t i = 0; i < slice.size(); i ++) {
        slice[i] = slice[i]->slice();
    }

    return slice;
}

std::vector<ChannelBufferPtr> CompositeChannelBuffer::decompose() {
    return decompose(ChannelBuffer::readerIndex(), ChannelBuffer::readableBytes());
}

int CompositeChannelBuffer::capacity() const {
    return indices[components.size()];
}

void CompositeChannelBuffer::capacity(int newCapacity) {
    BOOST_ASSERT(false && "has not implement.");
}

int8_t CompositeChannelBuffer::getByte(int index) const {
    int componentId = getComponentId(index);
    return components[componentId]->getByte(index - indices[componentId]);
}

int16_t CompositeChannelBuffer::getShort(int index) const {
    int componentId = getComponentId(index);

    if (index + 2 <= indices[componentId + 1]) {
        return components[componentId]->getShort(index - indices[componentId]);
    }
    else if (order() == ByteOrder::BIG) {
        return (int16_t)(((getByte(index) & 0xff) << 8) | (getByte(index + 1) & 0xff));
    }
    else {
        return (int16_t)((getByte(index) & 0xff) | ((getByte(index + 1) & 0xff) << 8));
    }
}

int32_t CompositeChannelBuffer::getInt(int index) const {
    int componentId = getComponentId(index);

    if (index + 4 <= indices[componentId + 1]) {
        return components[componentId]->getInt(index - indices[componentId]);
    }
    else if (order() == ByteOrder::BIG) {
        return ((getShort(index) & 0xffff) << 16) | (getShort(index + 2) & 0xffff);
    }
    else {
        return (getShort(index) & 0xFFFF) | ((getShort(index + 2) & 0xFFFF) << 16);
    }
}

int64_t CompositeChannelBuffer::getLong(int index) const {
    int componentId = getComponentId(index);

    if (index + 8 <= indices[componentId + 1]) {
        return components[componentId]->getLong(index - indices[componentId]);
    }
    else if (order() == ByteOrder::BIG) {
        return (((int64_t)getInt(index) & 0xffffffffL) << 32) | (getInt(index + 4) & 0xffffffffL);
    }
    else {
        return (getInt(index) & 0xFFFFFFFFL) | (((int64_t)getInt(index + 4) & 0xFFFFFFFFL) << 32);
    }
}

int CompositeChannelBuffer::getBytes(int index, char* dst, int length) const {
    int componentId = getComponentId(index);

    if (index > (capacity() - length)) {
        CETTY_NDC_SCOPE();
        throw RangeException("CompositeChannelBuffer getBytes out of range.");
    }

    int i = componentId;
    int transferredBytes = length;
    int dstIndex = 0;

    while (length > 0) {
        const ChannelBufferPtr& s = components[i];
        int adjustment = indices[i];
        int localLength = std::min(length, s->capacity() - (index - adjustment));
        s->getBytes(index - adjustment, dst + dstIndex, localLength);

        index += localLength;
        dstIndex += localLength;
        length -= localLength;
        ++i;
    }

    return transferredBytes;
}

int CompositeChannelBuffer::getBytes(int index, const ChannelBufferPtr& dst, int dstIndex, int length) const {
    int componentId = getComponentId(index);

    if (index > (capacity() - length) || dstIndex > (dst->capacity() - length)) {
        CETTY_NDC_SCOPE();
        throw RangeException("CompositeChannelBuffer getBytes out of range.");
    }

    int i = componentId;
    int transferredBytes = length;

    while (length > 0) {
        const ChannelBufferPtr& s = components[i];
        int adjustment = indices[i];
        int localLength = std::min(length, s->capacity() - (index - adjustment));

        s->getBytes(index - adjustment, dst, dstIndex, localLength);

        index += localLength;
        dstIndex += localLength;
        length -= localLength;
        ++i;
    }

    return transferredBytes;
}

int CompositeChannelBuffer::getBytes(int index, OutputStream* out, int length) const {
    int componentId = getComponentId(index);

    if (index > capacity() - length) {
        CETTY_NDC_SCOPE();
        throw RangeException("CompositeChannelBuffer getBytes out of range.");
    }

    int i = componentId;
    int transferredBytes = length;

    while (length > 0) {
        const ChannelBufferPtr& s = components[i];
        int adjustment = indices[i];
        int localLength = std::min(length, s->capacity() - (index - adjustment));
        s->getBytes(index - adjustment, out, localLength);
        index += localLength;
        length -= localLength;
        ++i;
    }

    return transferredBytes;
}

int CompositeChannelBuffer::setByte(int index, int value) {
    int componentId = getComponentId(index);
    return components[componentId]->setByte(index - indices[componentId], value);
}

int CompositeChannelBuffer::setShort(int index, int value) {
    int componentId = getComponentId(index);

    if (index + 2 <= indices[componentId + 1]) {
        return components[componentId]->setShort(index - indices[componentId], value);
    }
    else if (order() == ByteOrder::BIG) {
        setByte(index, (value >> 8));
        setByte(index + 1, value);
    }
    else {
        setByte(index    , value);
        setByte(index + 1, (value >> 8));
    }

    return 2;
}

int CompositeChannelBuffer::setInt(int index, int value) {
    int componentId = getComponentId(index);

    if (index + 4 <= indices[componentId + 1]) {
        return components[componentId]->setInt(index - indices[componentId], value);
    }
    else if (order() == ByteOrder::BIG) {
        setShort(index, (value >> 16));
        setShort(index + 2, value);
    }
    else {
        setShort(index    , value);
        setShort(index + 2, (value >> 16));
    }

    return 4;
}

int CompositeChannelBuffer::setLong(int index, int64_t value) {
    int componentId = getComponentId(index);

    if (index + 8 <= indices[componentId + 1]) {
        return components[componentId]->setLong(index - indices[componentId], value);
    }
    else if (order() == ByteOrder::BIG) {
        setInt(index, (int)(value >> 32));
        setInt(index + 4, (int) value);
    }
    else {
        setInt(index    , (int) value);
        setInt(index + 4, (int)(value >> 32));
    }

    return 8;
}

int CompositeChannelBuffer::setBytes(int index, const char* src, int length) {
    int componentId = getComponentId(index);

    if (NULL == src && index > capacity() - length) {
        CETTY_NDC_SCOPE();
        throw RangeException("CompositeChannelBuffer setBytes out of range.");
    }

    int i = componentId;
    int transferredBytes = length;
    int srcIndex = 0;

    while (length > 0) {
        ChannelBufferPtr& s = components[i];
        int adjustment = indices[i];
        int localLength = std::min(length, s->capacity() - (index - adjustment));

        s->setBytes(index - adjustment, src + srcIndex, localLength);

        index += localLength;
        srcIndex += localLength;
        length -= localLength;
        ++i;
    }

    return transferredBytes;
}

int CompositeChannelBuffer::setBytes(int index, const ConstChannelBufferPtr& src, int srcIndex, int length) {
    int componentId = getComponentId(index);

    if (index > capacity() - length || srcIndex > src->capacity() - length) {
        CETTY_NDC_SCOPE();
        throw RangeException("CompositeChannelBuffer setBytes out of range.");
    }

    int i = componentId;
    int transferredBytes = length;

    while (length > 0) {
        ChannelBufferPtr& s = components[i];
        int adjustment = indices[i];
        int localLength = std::min(length, s->capacity() - (index - adjustment));

        s->setBytes(index - adjustment, src, srcIndex, localLength);

        index += localLength;
        srcIndex += localLength;
        length -= localLength;
        ++i;
    }

    return transferredBytes;
}

int CompositeChannelBuffer::setBytes(int index, InputStream* in, int length) {
    int componentId = getComponentId(index);

    if (index > capacity() - length) {
        throw RangeException();
    }

    int i = componentId;
    int readBytes = 0;

    do {
        ChannelBufferPtr& s = components[i];
        int adjustment = indices[i];
        int localLength = std::min(length, s->capacity() - (index - adjustment));
        int localReadBytes = s->setBytes(index - adjustment, in, localLength);

        if (localReadBytes < 0) {
            if (readBytes == 0) {
                return -1;
            }
            else {
                break;
            }
        }

        if (localReadBytes == localLength) {
            index += localLength;
            length -= localLength;
            readBytes += localLength;
            i ++;
        }
        else {
            index += localReadBytes;
            length -= localReadBytes;
            readBytes += localReadBytes;
        }
    }
    while (length > 0);

    return readBytes;
}

ChannelBufferPtr CompositeChannelBuffer::copy(int index, int length) const {
    int componentId = getComponentId(index);

    if (index > (capacity() - length)) {
        CETTY_NDC_SCOPE();
        throw RangeException();
    }

    ChannelBufferPtr dst = Unpooled::buffer(length);
    BOOST_ASSERT(dst);

    copyTo(index, length, componentId, dst);
    return dst;
}

ChannelBufferPtr CompositeChannelBuffer::slice(int index, int length) {
    if (length == 0) {
        return Unpooled::EMPTY_BUFFER;
    }

    if (index < 0 || index > (capacity() - length)) {
        CETTY_NDC_SCOPE();
        throw RangeException();
    }

    std::vector<ChannelBufferPtr> components = decompose(index, length);

    switch (components.size()) {
    case 0:
        return Unpooled::EMPTY_BUFFER;

    case 1:
        return components[0];

    default:
        return ChannelBufferPtr(
                   new CompositeChannelBuffer(components));
    }
}

int CompositeChannelBuffer::slice(int index, int length, GatheringBuffer* gatheringBuffer) {
    if (!gatheringBuffer || length == 0) {
        return 0;
    }

    if (index < 0 || index + length > capacity()) {
        CETTY_NDC_SCOPE();
        throw RangeException("CompositeChannelBuffer slice out of range.");
    }

    size_t i,j;
    StringPiece arry;

    if (index == readerIdx && length == ChannelBuffer::readableBytes()) {
        for (i = 0, j = components.size(); i < j; ++i) {
            if (components[i]->readable()) {
                components[i]->readableBytes(&arry);
                gatheringBuffer->append((char*)arry.data(), arry.length());
            }
        }
    }
    else {
        std::vector<ChannelBufferPtr> decomposeds = decompose(index, length);

        for (i = 0, j = decomposeds.size(); i < j; ++i) {
            if (decomposeds[i]->readable()) {
                decomposeds[i]->readableBytes(&arry);
                gatheringBuffer->append((char*)arry.data(), arry.length());
            }
        }
    }

    return length;
}

const char* CompositeChannelBuffer::readableBytes(int* length) {
    CETTY_NDC_SCOPE();
    throw UnsupportedOperationException();
}

char* CompositeChannelBuffer::writableBytes(int* length) {
    CETTY_NDC_SCOPE();
    throw UnsupportedOperationException();
}

char* CompositeChannelBuffer::aheadWritableBytes(int* length) {
    CETTY_NDC_SCOPE();
    throw UnsupportedOperationException();
}

void CompositeChannelBuffer::discardReadBytes() {
    // Only the bytes between readerIndex and writerIndex will be kept.
    // New readerIndex and writerIndex will become 0 and
    // (previous writerIndex - previous readerIndex) respectively.

    int localReaderIndex = ChannelBuffer::readerIndex();

    if (localReaderIndex == 0) {
        return;
    }

    int localWriterIndex = this->writerIndex();

    int bytesToMove = capacity() - localReaderIndex;
    std::vector<ChannelBufferPtr> list = decompose(localReaderIndex, bytesToMove);

    // Add a new buffer so that the capacity of this composite buffer does
    // not decrease due to the discarded components.
    // XXX Might create too many components if discarded by small amount.
    ChannelBufferPtr padding = Unpooled::buffer(localReaderIndex);
    padding->writerIndex(localReaderIndex);
    list.push_back(padding);

    // Reset the index markers to get the index marker values.
    int localMarkedReaderIndex = localReaderIndex;

    try {
        resetReaderIndex();
        localMarkedReaderIndex = this->readerIndex();
    }
    catch (const RangeException& e) {
        e.what();
        // ignore
    }

    int localMarkedWriterIndex = localWriterIndex;

    try {
        resetWriterIndex();
        localMarkedWriterIndex = this->writerIndex();
    }
    catch (const RangeException& e) {
        e.what();
        // ignore
    }

    setComponents(list);

    // reset marked Indexes
    localMarkedReaderIndex = localMarkedReaderIndex > localReaderIndex ? localMarkedReaderIndex - localReaderIndex : 0;
    localMarkedWriterIndex = localMarkedWriterIndex > localReaderIndex ? localMarkedWriterIndex - localReaderIndex : 0;
    setIndex(localMarkedReaderIndex, localMarkedWriterIndex);
    markReaderIndex();
    markWriterIndex();

    // reset real indexes
    localWriterIndex = localWriterIndex > localReaderIndex ? localWriterIndex - localReaderIndex : 0;
    setIndex(0, localWriterIndex);
}

std::string CompositeChannelBuffer::toString() {
    std::string result = ChannelBuffer::toString();
    StringUtil::strprintf(&result, ", components=%d)", components.size());
    return result;
}

void CompositeChannelBuffer::setComponents(const std::vector<ChannelBufferPtr>& newComponents) {
    BOOST_ASSERT(!newComponents.empty());

    // Clear the cache.
    lastAccessedComponentId = 0;

    // Build the component array.
    components.clear();

    for (size_t i = 0; i < newComponents.size(); ++i) {
        const ChannelBufferPtr& c = newComponents.at(i);

        if (c->order() != order()) {
            CETTY_NDC_SCOPE();
            throw InvalidArgumentException("All buffers must have the same endianness.");
        }

        BOOST_ASSERT(c->readerIndex() == 0);
        BOOST_ASSERT(c->writerIndex() == c->capacity());

        components.push_back(c);
    }

    // Build the component lookup table.
    int indice = 0;
    this->indices.clear();
    this->indices.push_back(indice);

    for (size_t i = 1; i <= newComponents.size(); ++i) {
        indice = indices[i - 1] + newComponents[i - 1]->capacity();
        indices.push_back(indice);
    }

    // Reset the indexes.
    setIndex(0, capacity());
}

void CompositeChannelBuffer::copyTo(int index, int length, int componentId, const ChannelBufferPtr& dst) const {
    int dstIndex = 0;
    int i = componentId;

    while (length > 0) {
        ChannelBufferPtr s = components[i];
        int adjustment = indices[i];
        int localLength = std::min(length, s->capacity() - (index - adjustment));
        s->getBytes(index - adjustment, dst, dstIndex, localLength);

        index += localLength;
        dstIndex += localLength;
        length -= localLength;
        ++i;
    }

    dst->writerIndex(dst->capacity());
}

int CompositeChannelBuffer::getComponentId(int index) const {
    CompositeChannelBuffer* buf = const_cast<CompositeChannelBuffer*>(this);
    return buf->getComponentId(index);
}

int CompositeChannelBuffer::getComponentId(int index) {
    if (index < 0) {
        CETTY_NDC_SCOPE();
        throw RangeException("CompositeChannelBuffer getComponentId out of range.");
    }

    int lastComponentId = lastAccessedComponentId;

    if (index >= indices[lastComponentId]) {
        if (index < indices[lastComponentId + 1]) {
            return lastComponentId;
        }

        // Search right
        for (size_t i = lastComponentId + 1; i < components.size(); ++i) {
            if (index < indices[i + 1]) {
                lastAccessedComponentId = i;
                return i;
            }
        }
    }
    else {
        // Search left
        for (int i = lastComponentId - 1; i >= 0; --i) {
            if (index >= indices[i]) {
                lastAccessedComponentId = i;
                return i;
            }
        }
    }

    CETTY_NDC_SCOPE();
    throw RangeException("CompositeChannelBuffer getComponentId out of range.");
}

}
}
