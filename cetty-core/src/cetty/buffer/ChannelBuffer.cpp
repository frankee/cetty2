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

#include <cetty/buffer/ChannelBuffers.h>
#include <cetty/buffer/ChannelBufferFactory.h>
#include <cetty/util/Exception.h>
#include <cetty/util/Integer.h>
#include <cetty/util/Charset.h>
#include <cetty/util/StringUtil.h>
#include <cetty/util/NestedDiagnosticContext.h>

namespace cetty {
namespace buffer {

using namespace cetty::util;

int ChannelBuffer::readerIndex() const {
    return this->readerIdx;
}

void ChannelBuffer::readerIndex(int readerIdx) {
    if (readerIdx < 0 || readerIdx > this->writerIdx) {
        CETTY_NDC_SCOPE(readerIndex);
        throw RangeException("readerIndex");
    }

    this->readerIdx = readerIdx;
}

void ChannelBuffer::offsetReaderIndex(int offset) {
    int newReaderIdx = readerIdx + offset;

    if (newReaderIdx < 0 || newReaderIdx > writerIdx) {
        CETTY_NDC_SCOPE(offsetReaderIndex);
        throw RangeException("offsetReaderIndex");
    }

    readerIdx = newReaderIdx;
}

int ChannelBuffer::writerIndex() const {
    return writerIdx;
}

void ChannelBuffer::writerIndex(int writerIdx) {
    if (writerIdx < readerIdx || writerIdx > capacity()) {
        CETTY_NDC_SCOPE(writerIndex);
        throw RangeException("writerIndex");
    }

    this->writerIdx = writerIdx;
}

void ChannelBuffer::offsetWriterIndex(int offset) {
    int newWriterIdx = this->writerIdx + offset;

    if (newWriterIdx < readerIdx || newWriterIdx > capacity()) {
        CETTY_NDC_SCOPE(offsetWriterIndex);
        throw RangeException("offsetWriterIndex");
    }

    this->writerIdx = newWriterIdx;
}

void ChannelBuffer::setIndex(int readerIndex, int writerIndex) {
    if (readerIndex < 0 || readerIndex > writerIndex || writerIndex > capacity()) {
        CETTY_NDC_SCOPE(setIndex);
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
    writerIdx = this->markedWriterIndex;
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
        this->markedReaderIndex = this->markedReaderIndex - readerIdx > 0 ? this->markedReaderIndex - readerIdx : 0;
        this->markedWriterIndex = this->markedWriterIndex - readerIdx > 0 ? this->markedWriterIndex - readerIdx : 0;
        readerIdx = 0;
    }
}

void ChannelBuffer::ensureWritableBytes(int wBytes) {
    if (wBytes > writableBytes()) {
        CETTY_NDC_SCOPE(ensureWritableBytes);
        throw RangeException("has no enough capacity to write");
    }
}

boost::uint8_t ChannelBuffer::getUnsignedByte(int index) const {
    return (boost::uint8_t)(getByte(index) & 0xFF);
}

boost::uint16_t ChannelBuffer::getUnsignedShort(int index) const {
    return getShort(index) & 0xFFFF;
}

boost::uint32_t ChannelBuffer::getUnsignedInt(int index) const {
    return getInt(index) & 0xFFFFFFFF;
}

float ChannelBuffer::getFloat(int index) const {
    //TODO
    CETTY_NDC_SCOPE(getFloat);
    throw UnsupportedOperationException("getFloat");
    return 0;

    //return Float.intBitsToFloat(getInt(index));
}

double ChannelBuffer::getDouble(int index) const {
    CETTY_NDC_SCOPE(getFloat);
    throw UnsupportedOperationException("getDouble");
    return 0;
    //return Double.longBitsToDouble(getLong(index));
}

int ChannelBuffer::getBytes(int index, Array* dst) const {
    if (dst) {
        return getBytes(index, dst, 0, dst->length());
    }
    return 0;
}

int ChannelBuffer::getBytes(int index, std::string* dst, int length) const {
    if (dst) {
        dst->resize(length);
        Array arry = Array::wrappedArray(*dst);
        return getBytes(index, &arry);
    }

    return 0;
}

int ChannelBuffer::getBytes(int index, std::string* dst, int dstIndex, int length) const {
    if (dst) {
        dst->resize(dstIndex + length);
        Array arry = Array::wrappedArray(*dst);
        return getBytes(index, &arry, dstIndex, length);
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
    CETTY_NDC_SCOPE(setFloat);
    throw NotImplementedException("setFloat has not implement.");
    return 0;
    //TODO
    //setInt(index, Float.floatToRawIntBits(value));
}

int ChannelBuffer::setDouble(int index, double value) {
    CETTY_NDC_SCOPE(setDouble);
    throw NotImplementedException("setFloat has not implement.");
    return 0;
    //TODO
    //setLong(index, Double.doubleToRawLongBits(value));
}

int ChannelBuffer::setBytes(int index, const ConstArray& src) {
    return setBytes(index, src, 0, src.length());
}

int ChannelBuffer::setBytes(int index, const std::string& src) {
    return setBytes(index, ConstArray::fromString(src));
}

int ChannelBuffer::setBytes(int index, const std::string& src, int srcIndex, int length) {
    return setBytes(index, ConstArray::fromString(src), srcIndex, length);
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
        CETTY_NDC_SCOPE(setBytes);
        throw RangeException("exceed readable bytes");
    }

    int srcReaderIdx = src->readerIndex();

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
    if (length > leftBtyes) length = leftBtyes;

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

boost::int8_t ChannelBuffer::readByte() {
    boost::int8_t v = 0;
    if (checkReadableBytes(1)) {
         v = getByte(readerIdx++);
    }
    
    return v;
}

boost::uint8_t ChannelBuffer::readUnsignedByte() {
    return (boost::uint8_t)(readByte() & 0xFF);
}

boost::int16_t ChannelBuffer::readShort() {
    boost::int16_t v = 0;
    if (checkReadableBytes(2)) {
        v = getShort(readerIdx);
        readerIdx += 2;
    }
    return v;
}

boost::uint16_t ChannelBuffer::readUnsignedShort() {
    return boost::uint16_t(readShort() & 0xFFFF);
}

boost::int32_t ChannelBuffer::readInt() {
    boost::int32_t v = 0;
    if (checkReadableBytes(4)) {
        v = getInt(readerIdx);
        readerIdx += 4;
    }
    
    return v;
}

boost::uint32_t ChannelBuffer::readUnsignedInt() {
    return boost::uint32_t(readInt() & 0xFFFFFFFF);
}

boost::int64_t ChannelBuffer::readLong() {
    boost::int64_t v = 0;
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
        return ChannelBuffers::EMPTY_BUFFER;
    }

    if (checkReadableBytes(length)) {
        ChannelBufferPtr buf = factory().getBuffer(order(), length);
        buf->writeBytes(shared_from_this(), readerIdx, length);
        readerIdx += length;

        if (readerIdx == writerIdx) {
            readerIdx = writerIdx = 0;
        }

        return buf;
    }

    return ChannelBuffers::EMPTY_BUFFER;
}

int ChannelBuffer::readBytes(Array* dst, int dstIndex, int length) {
    if (!dst || 0 == length) {
        return 0;
    }

    if (checkReadableBytes(length)) {
        getBytes(readerIdx, dst, dstIndex, length);
        readerIdx += length;
        return length;
    }
    return 0;
}

int ChannelBuffer::readBytes(Array* dst) {
    if (!dst || dst->empty()) {
        return 0;
    }

    return readBytes(dst, 0, dst->length());
}

int ChannelBuffer::readBytes(std::string* dst) {
    if (dst) {
        dst->resize(readableBytes());

        Array arry = Array::wrappedArray(*dst);
        return readBytes(&arry, 0, arry.length());
    }

    return 0;
}

int ChannelBuffer::readBytes(std::string* dst, int length) {
    if (!dst || 0 == length) {
        return 0;
    }

    dst->resize(length);
    Array arry = Array::wrappedArray(*dst);
    return readBytes(&arry, 0, length);
}

int ChannelBuffer::readBytes(std::string* dst, int dstIndex, int length) {
    if (!dst || 0 == length) {
        return 0;
    }

    dst->resize(dstIndex + length);
    Array arry = Array::wrappedArray(*dst);
    return readBytes(&arry, dstIndex, length);
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
    
    return ChannelBuffers::EMPTY_BUFFER;
}

int ChannelBuffer::readSlice(GatheringBuffer* gatheringBuffer) {
    if (gatheringBuffer) {
        int transferBytes = readSlice(gatheringBuffer, readableBytes());
        if (transferBytes) {
            readerIdx = writerIdx = 0;
            return transferBytes;
        }
    }

    return 0;
}

int ChannelBuffer::readSlice(GatheringBuffer* gatheringBuffer, int length) {
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
    if (setByte(writerIdx + 1, value)) {
        ++writerIdx;
        return 1;
    }

    return 0;
}

int ChannelBuffer::writeShort(int value) {
    if (setShort(writerIdx, value)) {
        writerIdx += 2;
        return 2;
    }

    return 0;
}

int ChannelBuffer::writeInt(int value) {
    if (setInt(writerIdx, value)) {
        writerIdx += 4;
        return 4;
    }

    return 0;
}

int ChannelBuffer::writeLong(boost::int64_t value) {
    if (setLong(writerIdx, value)) {
        writerIdx += 8;
        return 8;
    }

    return 0;
}

int ChannelBuffer::writeFloat(float value) {
    if (setFloat(writerIdx, value)) {
        writerIdx += 4;
        return 4;
    }

    return 0;
}

int ChannelBuffer::writeDouble(double value) {
    if (setDouble(writerIdx, value)) {
        writerIdx += 8;
        return 8;
    }

    return 0;
}

int ChannelBuffer::writeBytes(const ConstArray& src, int srcIndex, int length) {
    if (setBytes(this->writerIdx, src, srcIndex, length)) {
        this->writerIdx += length;
        return length;
    }

    return 0;
}

int ChannelBuffer::writeBytes(const ConstArray& src) {
    return writeBytes(src, 0, src.length());
}

int ChannelBuffer::writeBytes(const std::string& src) {
    return writeBytes(ConstArray::fromString(src), 0, (int)src.length());
}

int ChannelBuffer::writeBytes(const std::string& src, int srcIndex, int length) {
    return writeBytes(ConstArray::fromString(src), srcIndex, length);
}

int ChannelBuffer::writeBytes(const ChannelBufferPtr& src) {
    if (src) {
        return writeBytes(src, src->readableBytes());
    }

    return 0;
}

int ChannelBuffer::writeBytes(const ChannelBufferPtr& src, int length) {
    if (src && writeBytes(src, src->readerIndex(), length)) {
        src->readerIndex(src->readerIndex() + length);
        return length;
    }
    
    return 0;
}

int ChannelBuffer::writeBytes(const ConstChannelBufferPtr& src, int srcIndex, int length) {
    if (src && setBytes(writerIdx, src, srcIndex, length)) {
        writerIdx += length;
        return length;
    }

    return 0;
}

int ChannelBuffer::writeBytes(InputStream* in, int length) {
    if (in) {
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

cetty::buffer::ChannelBufferPtr ChannelBuffer::duplicate() {
    return slice(0, this->capacity());
}

int ChannelBuffer::indexOf(int fromIndex, int toIndex, boost::int8_t value) const {
    return ChannelBuffers::indexOf(shared_from_this(), fromIndex, toIndex, value);
}

int ChannelBuffer::indexOf(int fromIndex, int toIndex, const ChannelBufferIndexFinder::Functor& indexFinder) const {
    return ChannelBuffers::indexOf(shared_from_this(), fromIndex, toIndex, indexFinder);
}

int ChannelBuffer::bytesBefore(boost::int8_t value) const {
    return bytesBefore(readerIndex(), readableBytes(), value);
}

int ChannelBuffer::bytesBefore(const ChannelBufferIndexFinder::Functor& indexFinder) const {
    return bytesBefore(readerIndex(), readableBytes(), indexFinder);
}

int ChannelBuffer::bytesBefore(int length, boost::int8_t value) const {
    if (checkReadableBytes(length)) {
        return bytesBefore(readerIndex(), length, value);
    }
    return -1;
}

int ChannelBuffer::bytesBefore(int length, const ChannelBufferIndexFinder::Functor& indexFinder) const {
    if (checkReadableBytes(length)) {
        return bytesBefore(readerIndex(), length, indexFinder);
    }
    return -1;
}

int ChannelBuffer::bytesBefore(int index, int length, boost::int8_t value) const {
    int endIndex = indexOf(index, index + length, value);

    if (endIndex < index) {
        return -1;
    }

    return endIndex - index;
}

int ChannelBuffer::bytesBefore(int index, int length, const ChannelBufferIndexFinder::Functor& indexFinder) const {
    int endIndex = indexOf(index, index + length, indexFinder);

    if (endIndex < index) {
        return -1;
    }

    return endIndex - index;
}

int ChannelBuffer::compare(const ChannelBufferPtr& that) const {
    return ChannelBuffers::compare(shared_from_this(), that);
}

bool ChannelBuffer::checkReadableBytes(int minReadableBytes) const {
    if (readableBytes() < minReadableBytes) {
        CETTY_NDC_SCOPE(writeBytes);
        throw RangeException("no data to read");
    }

    return true;
}

std::string ChannelBuffer::toString() const {
    std::string buf = typeid(this).name();
    StringUtil::strprintf(&buf, "(ridx=&d, widx=%d, cap=%d)",
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

int ChannelBuffer::writeLongAhead(boost::int64_t value) {
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

int ChannelBuffer::writeBytesAhead(const ConstChannelBufferPtr& src, int srcIndex, int length) {
    if (src && setBytes(readerIdx - length, src, srcIndex, length)) {
        readerIdx -= length;
        return length;
    }
    return 0;
}

int ChannelBuffer::writeBytesAhead(const ConstArray& src) {
    return writeBytesAhead(src, 0, src.length());
}

int ChannelBuffer::writeBytesAhead(const ConstArray& src, int srcIndex, int length) {
    if (setBytes(readerIdx - length, src, srcIndex, length)) {
        readerIdx -= length;
        return length;
    }
    return 0;
}

int ChannelBuffer::writeBytesAhead(const std::string& src) {
    return writeBytesAhead(ConstArray::fromString(src), 0, (int)src.length());
}

int ChannelBuffer::writeBytesAhead(const std::string& src, int srcIndex, int length) {
    return writeBytesAhead(ConstArray::fromString(src), srcIndex, length);
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

}
}

