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

#include <cetty/buffer/ChannelBuffer.h>

#include <cetty/buffer/Unpooled.h>
#include <cetty/buffer/ChannelBufferUtil.h>

#include <cetty/util/Exception.h>
#include <cetty/util/StlUtil.h>
#include <cetty/util/StringUtil.h>
#include <cetty/util/StringPiece.h>
#include <cetty/util/NestedDiagnosticContext.h>

#include <cetty/logging/LoggerHelper.h>

namespace cetty {
namespace buffer {

using namespace cetty::util;

int ChannelBuffer::maxCapacity() const {
    return this->maximumCapacity;
}

const ByteOrder& ChannelBuffer::order() const {
    return this->byteOrder;
}

#if 0
ChannelBufferPtr ChannelBuffer::order(const ByteOrder& endianness) {
    if (endianness == byteOrder) {
        return shared_from_this();
    }

    return new SwappedChannelBuffer(shared_from_this());
}
#endif

int ChannelBuffer::readerIndex() const {
    return this->readerIdx;
}

void ChannelBuffer::readerIndex(int readerIdx) {
    if (readerIdx < 0 || readerIdx > this->writerIdx) {
        CETTY_NDC_SCOPE();
        throw RangeException("readerIndex");
    }

    this->readerIdx = readerIdx;
}

void ChannelBuffer::offsetReaderIndex(int offset) {
    int newReaderIdx = readerIdx + offset;

    if (newReaderIdx < 0 || newReaderIdx > writerIdx) {
        CETTY_NDC_SCOPE();
        throw RangeException("offsetReaderIndex");
    }

    readerIdx = newReaderIdx;
}

int ChannelBuffer::writerIndex() const {
    return writerIdx;
}

void ChannelBuffer::writerIndex(int writerIdx) {
    if (writerIdx < readerIdx || writerIdx > capacity()) {
        CETTY_NDC_SCOPE();
        throw RangeException("writerIndex");
    }

    this->writerIdx = writerIdx;
}

void ChannelBuffer::offsetWriterIndex(int offset) {
    int newWriterIdx = this->writerIdx + offset;

    if (newWriterIdx < readerIdx || newWriterIdx > capacity()) {
        CETTY_NDC_SCOPE();
        throw RangeException("offsetWriterIndex");
    }

    this->writerIdx = newWriterIdx;
}

void ChannelBuffer::setIndex(int readerIndex, int writerIndex) {
    if (readerIndex < 0 || readerIndex > writerIndex || writerIndex > capacity()) {
        CETTY_NDC_SCOPE();
        throw RangeException("readerIndex or writerIndex out of range");
    }

    this->readerIdx = readerIndex;
    this->writerIdx = writerIndex;
}

void ChannelBuffer::clear() {
    readerIdx = writerIdx = 0;
}

bool ChannelBuffer::readable() const {
    return readableBytes() > 0;
}

bool ChannelBuffer::writable() const {
    return writableBytes() > 0;
}

int ChannelBuffer::readableBytes() const {
    return writerIdx - readerIdx;
}

int ChannelBuffer::readableBytes(StringPiece* bytes) {
    if (bytes) {
        int length = 0;
        const char* buf = readableBytes(&length);
        bytes->set(buf, length);
        return length;
    }

    return readableBytes();
}

int ChannelBuffer::writableBytes() const {
    return capacity() - writerIdx;
}

int ChannelBuffer::aheadWritableBytes() const {
    return readerIdx;
}

void ChannelBuffer::markReaderIndex() {
    this->markedReaderIndex = readerIdx;
}

void ChannelBuffer::resetReaderIndex() {
    readerIndex(this->markedReaderIndex);
}

void ChannelBuffer::markWriterIndex() {
    this->markedWriterIndex = writerIdx;
}

void ChannelBuffer::resetWriterIndex() {
    writerIdx = markedWriterIndex;
}

void ChannelBuffer::discardReadBytes() {
    if (readerIdx == 0) {
        return;
    }

    if (readerIdx == writerIdx) {
        readerIdx = writerIdx = 0;
        return;
    }

    if (setBytes(0, shared_from_this(), readerIdx, writerIdx - readerIdx)) {
        writerIdx -= readerIdx;
        markedReaderIndex = markedReaderIndex - readerIdx;
        markedWriterIndex = markedWriterIndex - readerIdx;

        if (markedReaderIndex < 0) {
            markedReaderIndex = 0;
        }

        if (markedWriterIndex < 0) {
            markedWriterIndex = 0;
        }

        readerIdx = 0;
    }
}

void ChannelBuffer::ensureWritableBytes(int minWritableBytes) {
    if (minWritableBytes <= writableBytes()) {
        return;
    }

    if (minWritableBytes > maximumCapacity - writerIdx) {
        CETTY_NDC_SCOPE();
        throw RangeException(
            StringUtil::printf(
                "writerIndex(%d) + minWritableBytes(%d) exceeds maxCapacity(%d)",
                writerIdx, minWritableBytes, maximumCapacity));
    }

    // Normalize the current capacity to the power of 2.
    int newCapacity = calculateNewCapacity(writerIdx + minWritableBytes);

    // Adjust to the new capacity.
    capacity(newCapacity);
}

int ChannelBuffer::ensureWritableBytes(int minWritableBytes, bool force) {
    if (minWritableBytes <= writableBytes()) {
        return 1;
    }

    if (minWritableBytes + writerIdx > maximumCapacity) {
        if (force) {
            if (capacity() == maxCapacity()) {
                return -1;
            }

            capacity(maxCapacity());
            return -2;
        }
    }

    // Normalize the current capacity to the power of 2.
    int newCapacity = calculateNewCapacity(writerIdx + minWritableBytes);

    // Adjust to the new capacity.
    capacity(newCapacity);
    return 2;
}

uint8_t ChannelBuffer::getUnsignedByte(int index) const {
    return (uint8_t)(getByte(index) & 0xFF);
}

uint16_t ChannelBuffer::getUnsignedShort(int index) const {
    return getShort(index) & 0xFFFF;
}

uint32_t ChannelBuffer::getUnsignedInt(int index) const {
    return getInt(index) & 0xFFFFFFFF;
}

float ChannelBuffer::getFloat(int index) const {
    //TODO
    CETTY_NDC_SCOPE();
    throw UnsupportedOperationException("getFloat");
    return 0;
}

double ChannelBuffer::getDouble(int index) const {
    CETTY_NDC_SCOPE();
    throw UnsupportedOperationException("getDouble");
    return 0;
}

int ChannelBuffer::getVarint(int index, uint64_t* value) {
    return 0;
}

int ChannelBuffer::getBytes(int index, std::string* dst, int length) const {
    if (dst) {
        int oldSize = dst->size();

        STLStringResizeUninitialized(dst, oldSize + length);
        char* start = reinterpret_cast<char*>(stringAsArray(dst) + oldSize);

        return getBytes(index, start, length);
    }

    return 0;
}

int ChannelBuffer::getBytes(int index, const ChannelBufferPtr& dst) const {
    if (dst) {
        return getBytes(index, dst, dst->writableBytes());
    }

    return 0;
}

int ChannelBuffer::getBytes(int index, const ChannelBufferPtr& dst, int length) const {
    if (!dst) {
        //log here.
        return 0;
    }

    if (getBytes(index, dst, dst->writerIndex(), length)) {
        dst->offsetWriterIndex(length);
        return length;
    }

    return 0;
}

int ChannelBuffer::setFloat(int index, float value) {
    CETTY_NDC_SCOPE();
    throw NotImplementedException("setFloat has not implement.");
    return 0;
    //TODO
}

int ChannelBuffer::setDouble(int index, double value) {
    CETTY_NDC_SCOPE();
    throw NotImplementedException("setFloat has not implement.");
    return 0;
    //TODO
}

int ChannelBuffer::setVarint(int index, uint64_t value) {
    return 0;
}

int ChannelBuffer::setBytes(int index, const StringPiece& src) {
    if (!src.empty()) {
        return setBytes(index, src.data(), src.length());
    }

    return 0;
}

int ChannelBuffer::setBytes(int index,
                            const StringPiece& src,
                            int srcIndex,
                            int length) {
    if (src.empty()
            || srcIndex < 0
            || length < 0
            || srcIndex + length > src.length()) {
        return 0;
    }

    return setBytes(index, src.data() + srcIndex, length);
}

int ChannelBuffer::setBytes(int index, const ChannelBufferPtr& src) {
    if (src) {
        setBytes(index, src, src->readableBytes());
    }

    return 0;
}

int ChannelBuffer::setBytes(int index, const ChannelBufferPtr& src, int length) {
    if (!src) {
        return 0;
    }

    if (length > src->readableBytes()) {
        CETTY_NDC_SCOPE();
        throw RangeException("exceed readable bytes");
    }

    if (setBytes(index, src, src->readerIndex(), length)) {
        src->offsetReaderIndex(length);
        return length;
    }

    return 0;
}

int ChannelBuffer::setZero(int index, int length) {
    if (length <= 0) {
        return 0;
    }

    //if length out_of_range, may cause out_of_range exception or just reduce the length.
    int leftBtyes = capacity() - index;

    if (length > leftBtyes) { length = leftBtyes; }

    int nLong = length >> 3;
    int nBytes = length & 7;

    for (int i = nLong; i > 0; --i) {
        setLong(index, 0);
        index += 8;
    }

    if (nBytes == 4) {
        setInt(index, 0);
    }
    else if (nBytes < 4) {
        for (int i = nBytes; i > 0; --i) {
            setByte(index, 0);
            ++index;
        }
    }
    else {
        setInt(index, 0);
        index += 4;

        for (int i = nBytes - 4; i > 0; --i) {
            setByte(index, 0);
            ++index;
        }
    }

    return length;
}

int8_t ChannelBuffer::readByte() {
    int8_t v = 0;

    if (checkReadableBytes(1)) {
        v = getByte(readerIdx++);
    }

    return v;
}

uint8_t ChannelBuffer::readUnsignedByte() {
    return (uint8_t)(readByte() & 0xFF);
}

int16_t ChannelBuffer::readShort() {
    int16_t v = 0;

    if (checkReadableBytes(2)) {
        v = getShort(readerIdx);
        readerIdx += 2;
    }

    return v;
}

uint16_t ChannelBuffer::readUnsignedShort() {
    return uint16_t(readShort() & 0xFFFF);
}

int32_t ChannelBuffer::readInt() {
    int32_t v = 0;

    if (checkReadableBytes(4)) {
        v = getInt(readerIdx);
        readerIdx += 4;
    }

    return v;
}

uint32_t ChannelBuffer::readUnsignedInt() {
    return uint32_t(readInt() & 0xFFFFFFFF);
}

int64_t ChannelBuffer::readLong() {
    int64_t v = 0;

    if (checkReadableBytes(8)) {
        v = getLong(readerIdx);
        readerIdx += 8;
    }

    return v;
}

float ChannelBuffer::readFloat() {
    float v = 0;

    if (checkReadableBytes(4)) {
        v = getFloat(readerIdx);
        readerIdx += 4;
    }

    return v;
}

double ChannelBuffer::readDouble() {
    double v = 0;

    if (checkReadableBytes(8)) {
        v = getDouble(readerIdx);
        readerIdx += 8;
    }

    return v;
}

ChannelBufferPtr ChannelBuffer::readBytes() {
    return readBytes(readableBytes());
}

ChannelBufferPtr ChannelBuffer::readBytes(int length) {
    if (0 == length) {
        return Unpooled::EMPTY_BUFFER;
    }

    if (checkReadableBytes(length)) {
        ChannelBufferPtr buf = Unpooled::buffer(length);
        buf->writeBytes(shared_from_this(), readerIdx, length);
        readerIdx += length;

        if (readerIdx == writerIdx) {
            readerIdx = writerIdx = 0;
        }

        return buf;
    }

    return Unpooled::EMPTY_BUFFER;
}

int ChannelBuffer::readBytes(char* dst, int length) {
    if (!dst || length <= 0) {
        return 0;
    }

    if (checkReadableBytes(length)) {
        getBytes(readerIdx, dst, length);
        readerIdx += length;
        return length;
    }

    return 0;
}

int ChannelBuffer::readBytes(std::string* dst) {
    if (dst) {
        int oldSize = dst->size();
        int length = readableBytes();

        STLStringResizeUninitialized(dst, oldSize + length);
        char* start = reinterpret_cast<char*>(stringAsArray(dst) + oldSize);

        return readBytes(start, length);
    }

    return 0;
}

int ChannelBuffer::readBytes(std::string* dst, int length) {
    if (dst && length > 0) {
        int oldSize = dst->size();

        STLStringResizeUninitialized(dst, oldSize + length);
        char* start = reinterpret_cast<char*>(stringAsArray(dst) + oldSize);

        return readBytes(start, length);
    }

    return 0;
}

int ChannelBuffer::readBytes(const ChannelBufferPtr& dst) {
    if (dst) {
        return readBytes(dst, dst->writableBytes());
    }

    return 0;
}

int ChannelBuffer::readBytes(const ChannelBufferPtr& dst, int length) {
    if (dst && readBytes(dst, dst->writerIndex(), length)) {
        dst->writerIndex(dst->writerIndex() + length);
        return length;
    }

    return 0;
}

int ChannelBuffer::readBytes(const ChannelBufferPtr& dst, int dstIndex, int length) {
    if (checkReadableBytes(length)) {
        if (getBytes(readerIdx, dst, dstIndex, length)) {
            readerIdx += length;
            return length;
        }
    }

    return 0;
}

int ChannelBuffer::readBytes(OutputStream* out, int length) {
    if (checkReadableBytes(length)) {
        int transferBytes = getBytes(readerIdx, out, length);

        if (transferBytes > 0) {
            readerIdx += transferBytes;
        }

        return transferBytes;
    }

    return 0;
}

ChannelBufferPtr ChannelBuffer::readSlice() {
    ChannelBufferPtr buffer = readSlice(readableBytes());
    readerIdx = writerIdx = 0;
    return buffer;
}

ChannelBufferPtr ChannelBuffer::readSlice(int length) {
    if (checkReadableBytes(length)) {
        ChannelBufferPtr sliceBuffer = slice(readerIdx, length);
        readerIdx += length;
        return sliceBuffer;
    }

    return Unpooled::EMPTY_BUFFER;
}

int ChannelBuffer::readSlice(GatheringBuffer* gatheringBuffer) {
    if (gatheringBuffer) {
        int transferBytes = readSlice(readableBytes(), gatheringBuffer);

        if (transferBytes) {
            readerIdx = writerIdx = 0;
            return transferBytes;
        }
    }

    return 0;
}

int ChannelBuffer::readSlice(int length, GatheringBuffer* gatheringBuffer) {
    if (gatheringBuffer && checkReadableBytes(length)) {
        if (slice(readerIdx, length, gatheringBuffer)) {
            readerIdx += length;
            return length;
        }
    }

    return 0;
}

int ChannelBuffer::skipBytes(int length) {
    if (checkReadableBytes(length)) {
        readerIdx += length;
        return length;
    }

    return 0;
}

int ChannelBuffer::writeByte(int value) {
    if (ensureWritableBytes(1, false) > 0 && setByte(writerIdx, value)) {
        ++writerIdx;
        return 1;
    }

    return 0;
}

int ChannelBuffer::writeShort(int value) {
    if (ensureWritableBytes(2, false) > 0 && setShort(writerIdx, value)) {
        writerIdx += 2;
        return 2;
    }

    return 0;
}

int ChannelBuffer::writeInt(int value) {
    if (ensureWritableBytes(4, false) > 0 && setInt(writerIdx, value)) {
        writerIdx += 4;
        return 4;
    }

    return 0;
}

int ChannelBuffer::writeLong(int64_t value) {
    if (ensureWritableBytes(8, false) > 0 && setLong(writerIdx, value)) {
        writerIdx += 8;
        return 8;
    }

    return 0;
}

int ChannelBuffer::writeFloat(float value) {
    if (ensureWritableBytes(4, false) > 0 && setFloat(writerIdx, value)) {
        writerIdx += 4;
        return 4;
    }

    return 0;
}

int ChannelBuffer::writeDouble(double value) {
    if (ensureWritableBytes(8, false) > 0 && setDouble(writerIdx, value)) {
        writerIdx += 8;
        return 8;
    }

    return 0;
}

int ChannelBuffer::writeVarint(uint64_t value) {
    return 0;
}

int ChannelBuffer::writeBytes(const StringPiece& src, int srcIndex, int length) {
    if (!src.empty()
            && srcIndex >= 0
            && length > 0
            && srcIndex + length <= src.length()) {
        return writeBytes(src.data() + srcIndex, length);
    }

    return 0;
}

int ChannelBuffer::writeBytes(const StringPiece& src) {
    if (!src.empty()) {
        return writeBytes(src.data(), src.length());
    }

    return 0;
}

int ChannelBuffer::writeBytes(const char* src, int length) {
    if (src
            && length > 0
            && ensureWritableBytes(length, false) > 0
            && setBytes(this->writerIdx, src, length)) {

        this->writerIdx += length;
        return length;
    }

    return 0;
}

int ChannelBuffer::writeBytes(const ChannelBufferPtr& src) {
    if (src) {
        return writeBytes(src, src->readableBytes());
    }

    return 0;
}

int ChannelBuffer::writeBytes(const ChannelBufferPtr& src, int length) {
    if (src
            && ensureWritableBytes(length, false) > 0
            && writeBytes(src, src->readerIndex(), length)) {
        src->readerIndex(src->readerIndex() + length);
        return length;
    }

    return 0;
}

int ChannelBuffer::writeBytes(const ConstChannelBufferPtr& src,
                              int srcIndex,
                              int length) {
    if (src
            && ensureWritableBytes(length, false) > 0
            && setBytes(writerIdx, src, srcIndex, length)) {
        writerIdx += length;
        return length;
    }

    return 0;
}

int ChannelBuffer::writeBytes(InputStream* in, int length) {
    if (in && ensureWritableBytes(length, false) > 0) {
        int writtenBytes = setBytes(writerIdx, in, length);

        if (writtenBytes > 0) {
            writerIdx += writtenBytes;
        }

        return writtenBytes;
    }

    return 0;
}

int ChannelBuffer::writeZero(int length) {
    if (length <= 0) {
        // log here
        return 0;
    }

    //if length more than writableBytes(), may cause out_of_range exception
    //or auto reduce the length to writableBytes().
    int wb = writableBytes();

    if (length > wb) {
        length = wb;
    }

    int nLong = length >> 3;
    int nBytes = length & 7;

    for (int i = nLong; i > 0; --i) {
        writeLong(0);
    }

    if (nBytes == 4) {
        writeInt(0);
    }
    else if (nBytes < 4) {
        for (int i = nBytes; i > 0; --i) {
            writeByte(0);
        }
    }
    else {
        writeInt(0);

        for (int i = nBytes - 4; i > 0; --i) {
            writeByte(0);
        }
    }

    return length;
}

cetty::buffer::ChannelBufferPtr ChannelBuffer::copy() const {
    return copy(readerIdx, readableBytes());
}

cetty::buffer::ChannelBufferPtr ChannelBuffer::slice() {
    return slice(readerIdx, readableBytes());
}

int ChannelBuffer::slice(GatheringBuffer* gatheringBuffer) {
    if (gatheringBuffer) {
        return slice(readerIdx, readableBytes(), gatheringBuffer);
    }

    return 0;
}

int ChannelBuffer::indexOf(int fromIndex, int toIndex, int8_t value) const {
    return ChannelBufferUtil::indexOf(shared_from_this(), fromIndex, toIndex, value);
}

int ChannelBuffer::indexOf(int fromIndex, int toIndex, const StringPiece& value) const {
    return 0;
}

int ChannelBuffer::indexNotOf(int fromIndex, int toIndex, int8_t value) const {
    return 0;
}

int ChannelBuffer::indexNotOf(int fromIndex, int toIndex, const StringPiece& value) const {
    return 0;
}

int ChannelBuffer::bytesBefore(int8_t value, int offset, int length) const {
    int readableCnt = readableBytes();

    if (offset > readableCnt || length == 0) {
        return 0;
    }

    if (offset < 0) {
        offset = 0;
    }

    int endIndex = length < 0 ? writerIdx : offset + readerIdx + length;

    if (endIndex > writerIdx) {
        endIndex = writerIdx;
    }

    return indexOf(readerIdx + offset, endIndex, value) - readerIdx - offset;
}

int ChannelBuffer::bytesBefore(const StringPiece& value, int offset, int length) const {
    return 0;
}

int ChannelBuffer::bytesBeforeNot(int8_t value, int offset, int length) const {
    return 0;
}

int ChannelBuffer::bytesBeforeNot(const StringPiece& value, int offset, int length) const {
    return 0;
}

int ChannelBuffer::compare(const ChannelBufferPtr& that) const {
    return ChannelBufferUtil::compare(shared_from_this(), that);
}

bool ChannelBuffer::checkReadableBytes(int minReadableBytes, bool throwException) const {
    if (readableBytes() < minReadableBytes) {
        if (throwException) {
            CETTY_NDC_SCOPE();
            throw RangeException("no data to read");
        }

        return false;
    }

    return true;
}

std::string ChannelBuffer::toString() const {
    std::string buf = typeid(this).name();
    StringUtil::printf(&buf, "(ridx=&d, widx=%d, cap=%d)",
                          readerIdx, writerIdx, capacity());
    return buf;
}

int ChannelBuffer::writeByteAhead(int value) {
    if (setByte(readerIdx - 1, value)) {
        --readerIdx;
        return 1;
    }

    return 1;
}

int ChannelBuffer::writeShortAhead(int value) {
    if (setShort(readerIdx - 2, value)) {
        readerIdx -= 2;
        return 2;
    }

    return 0;
}

int ChannelBuffer::writeIntAhead(int value) {
    if (setInt(readerIdx - 4, value)) {
        readerIdx -= 4;
        return 4;
    }

    return 0;
}

int ChannelBuffer::writeLongAhead(int64_t value) {
    if (setLong(readerIdx - 8, value)) {
        readerIdx -= 8;
        return 8;
    }

    return 0;
}

int ChannelBuffer::writeFloatAhead(float value) {
    if (setFloat(readerIdx - 4, value)) {
        readerIdx -= 4;

        return 4;
    }

    return 0;
}

int ChannelBuffer::writeDoubleAhead(double value) {
    if (setDouble(readerIdx - 8, value)) {
        readerIdx -= 8;
        return 8;
    }

    return 0;
}

int ChannelBuffer::writeVarintAhead(uint64_t value) {
    return 0;
}

int ChannelBuffer::writeBytesAhead(const ChannelBufferPtr& src) {
    if (src) {
        return writeBytesAhead(src, src->readableBytes());
    }

    return 0;
}

int ChannelBuffer::writeBytesAhead(const ChannelBufferPtr& src, int length) {
    if (src && writeBytesAhead(src, src->readerIndex(), length)) {
        src->readerIndex(src->readerIndex() + length);
        return length;
    }

    return 0;
}

int ChannelBuffer::writeBytesAhead(const ConstChannelBufferPtr& src,
                                   int srcIndex,
                                   int length) {
    if (src && setBytes(readerIdx - length, src, srcIndex, length)) {
        readerIdx -= length;
        return length;
    }

    return 0;
}

int ChannelBuffer::writeBytesAhead(const StringPiece& src) {
    return writeBytesAhead(src.data(), src.length());
}

int ChannelBuffer::writeBytesAhead(const char* src, int length) {
    if (src && setBytes(readerIdx - length, src, length)) {
        readerIdx -= length;
        return length;
    }

    return 0;
}

int ChannelBuffer::writeBytesAhead(const StringPiece& src,
                                   int srcIndex,
                                   int length) {
    if (src.empty()
            || length < 0
            || srcIndex < 0
            || srcIndex + length > src.length()) {
        return 0;
    }

    return writeBytesAhead(src.data() + srcIndex, length);
}

int ChannelBuffer::writeZeroAhead(int length) {
    if (length <= 0) {
        return 0;
    }

    //if length more than writableBytes(), may cause out_of_range exception
    //or auto reduce the length to writableBytes().
    if (length > writableBytes()) {
        length = writableBytes();
    }

    int nLong = length >> 3;
    int nBytes = length & 7;

    for (int i = nLong; i > 0; --i) {
        writeLongAhead(0);
    }

    if (nBytes == 4) {
        writeIntAhead(0);
    }
    else if (nBytes < 4) {
        for (int i = nBytes; i > 0; --i) {
            writeByteAhead(0);
        }
    }
    else {
        writeInt(0);

        for (int i = nBytes - 4; i > 0; --i) {
            writeByteAhead(0);
        }
    }

    return length;
}

int ChannelBuffer::calculateNewCapacity(int minNewCapacity) {
    int maximumCapacity = this->maximumCapacity;
    int threshold = 1048576 * 4; // 4 MiB page

    if (minNewCapacity == threshold) {
        return minNewCapacity;
    }

    // If over threshold, do not double but just increase by threshold.
    if (minNewCapacity > threshold) {
        int newCapacity = minNewCapacity / threshold * threshold;

        if (newCapacity > maximumCapacity - threshold) {
            newCapacity = maximumCapacity;
        }
        else {
            newCapacity += threshold;
        }

        return newCapacity;
    }

    // Not over threshold. Double up to 4 MiB, starting from 64.
    int newCapacity = 64;

    while (newCapacity < minNewCapacity) {
        newCapacity <<= 1;
    }

    return std::min(newCapacity, maximumCapacity);
}

}
}
