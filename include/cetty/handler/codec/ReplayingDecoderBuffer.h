#if !defined(CETTY_HANDLER_CODEC_REPLAYINGDECODERBUFFER_H)
#define CETTY_HANDLER_CODEC_REPLAYINGDECODERBUFFER_H

/*
 * Copyright 2009 Red Hat, Inc.
 *
 * Red Hat licenses this file to you under the Apache License, version 2.0
 * (the "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at:
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 */
/*
 * Copyright (c) 2010-2011 frankee zhou (frankee.zhou at gmail dot com)
 * Distributed under under the Apache License, version 2.0 (the "License").
 */

#include <cetty/buffer/ChannelBuffer.h>
#include <cetty/handler/codec/UnreplayableOperationException.h>

namespace cetty {
namespace handler {
namespace codec {

using namespace cetty::buffer;

class ReplayingDecoderBuffer;

/**
 *
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 *
 * @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
 *
 */

typedef boost::intrusive_ptr<ReplayingDecoderBuffer> ReplayingDecoderBufferPtr;

class ReplayingDecoderBuffer : public cetty::buffer::ChannelBuffer {
public:
    ReplayingDecoderBuffer(const ChannelBufferPtr& buffer)
        : needMore(false), terminated(false), buffer(buffer) {
            readerIdx = buffer->readerIndex();
            writerIdx = buffer->writerIndex();
    }

    void terminate() {
        terminated = true;
    }

    bool needMoreBytes() const {
        return needMore;
    }

    void needMoreBytes(bool needMore) {
        this->needMore = needMore;
    }

    virtual int capacity() const;
    virtual void capacity(int newCapacity) {
        buffer->capacity(newCapacity);
    }

    int maxCapacity() const {
        return buffer->capacity();
    }

    const ByteOrder& order() const {
        return buffer->order();
    }

    int readerIndex() const {
        return buffer->readerIndex();
    }
    void readerIndex(int readerIdx) { buffer->readerIndex(readerIdx); }
    void offsetReaderIndex(int offset) { buffer->offsetReaderIndex(offset); }

    int writerIndex() const { return buffer->writerIndex(); }
    void writerIndex(int writerIdx) { throw UnreplayableOperationException(); }
    void offsetWriterIndex(int offset) { throw UnreplayableOperationException(); }

    void setIndex(int readerIndex, int writerIndex) {
        throw UnreplayableOperationException();
    }

    int readableBytes(StringPiece* bytes) { return buffer->readableBytes(bytes); }

    int readableBytes() const;
    int writableBytes() const;
    int aheadWritableBytes() const;

    virtual const char* readableBytes(int* length);
    virtual char* writableBytes(int* length);
    virtual char* aheadWritableBytes(int* length);

    bool readable() const { return buffer->readable(); }
    bool writable() const { return false; }

    void markReaderIndex() { buffer->markReaderIndex(); }
    void resetReaderIndex() { buffer->resetReaderIndex(); }

    void markWriterIndex() { throw UnreplayableOperationException(); }
    void resetWriterIndex() { throw UnreplayableOperationException(); }

    ChannelBufferPtr copy() {
        throw UnreplayableOperationException();
    }
    virtual ChannelBufferPtr copy(int index, int length) const;

    virtual void discardReadBytes();
    virtual void ensureWritableBytes(int writableBytes);
    virtual int ensureWritableBytes(int minWritableBytes, bool force) {
        return ChannelBuffer::ensureWritableBytes(minWritableBytes, force);
    }

    virtual int8_t   getByte(int index) const;
    virtual int16_t  getShort(int index) const;
    virtual int32_t  getInt(int index) const;
    virtual int64_t  getLong(int index) const;

    virtual int getBytes(int index, char* dst, int length) const;
    virtual int getBytes(int index, const ChannelBufferPtr& dst, int dstIndex, int length) const;
    virtual int getBytes(int index, OutputStream* out, int length) const;

    virtual int indexOf(int fromIndex, int toIndex, int8_t value) const;
    virtual int indexOf(int fromIndex, int toIndex, const StringPiece& value) const {
        return ChannelBuffer::indexOf(fromIndex, toIndex, value);
    }

    virtual int setByte(int index, int value);
    virtual int setShort(int index, int value);
    virtual int setInt(int index, int value);
    virtual int setLong(int index, int64_t value);

    virtual int setBytes(int index, const char* src, int length);
    virtual int setBytes(int index, const ConstChannelBufferPtr& src, int srcIndex, int length);
    virtual int setBytes(int index, InputStream* in, int length);

    virtual ChannelBufferPtr slice(int index, int length);
    virtual int slice(int index, int length, GatheringBuffer* gatheringBuffer);
    
    int8_t  readByte() { return buffer->readByte(); }
    uint8_t readUnsignedByte() { return buffer->readUnsignedByte(); }
    int16_t readShort() { return buffer->readShort(); }
    uint16_t readUnsignedShort() { return buffer->readUnsignedShort(); }
    int32_t readInt() { return buffer->readInt(); }
    uint32_t readUnsignedInt() { return buffer->readUnsignedInt(); }
    int64_t readLong() { return buffer->readLong(); }

    int readVarint(uint64_t* value) { return buffer->readVarint(value); }

    ChannelBufferPtr readBytes() {
        return readBytes();
    }
    ChannelBufferPtr readBytes(int length);
    int readBytes(const ChannelBufferPtr& dst) {
        throw UnreplayableOperationException();
    }
    int readBytes(const ChannelBufferPtr& dst, int length) {
        throw UnreplayableOperationException();
    }
    int readBytes(const ChannelBufferPtr& dst, int dstIndex, int length) {
        return buffer->readBytes(dst, dstIndex, length);
    }
    int readBytes(char* dst, int length) {
        return buffer->readBytes(dst, length);
    }
    int readBytes(std::string* dst) {
        return buffer->readBytes(dst);
    }
    int readBytes(std::string* dst, int length) {
        return buffer->readBytes(dst, length);
    }
    int readBytes(OutputStream* out, int length) {
        throw UnreplayableOperationException();
    }
    ChannelBufferPtr readSlice() {
        return buffer->readSlice();
    }
    ChannelBufferPtr readSlice(int length) {
        return buffer->readSlice(length);
    }
    int readSlice(GatheringBuffer* bytes) {
        return buffer->readSlice(bytes);
    }
    int readSlice(int length, GatheringBuffer* bytes) {
        return buffer->readSlice(length, bytes);
    }
    int skipBytes(int length) {
        return buffer->skipBytes(length);
    }

    void clear();

    virtual std::string toString() const;

private:
    bool checkIndex(int index, int length) const;

    virtual bool checkReadableBytes(int readableBytes, bool throwException) const;

private:
    mutable bool needMore;
    bool         terminated;

    ChannelBufferPtr buffer;
};

}
}
}

#endif //#if !defined(CETTY_HANDLER_CODEC_REPLAYINGDECODERBUFFER_H)

// Local Variables:
// mode: c++
// End:
