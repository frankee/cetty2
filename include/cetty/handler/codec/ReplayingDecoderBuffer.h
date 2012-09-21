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

    void syncIndex() {
        buffer->setIndex(readerIdx, writerIdx);
    }

    virtual int capacity() const;
    virtual void capacity(int newCapacity);

    virtual ChannelBufferPtr copy(int index, int length) const;

    virtual void discardReadBytes();
    virtual void ensureWritableBytes(int writableBytes);

    virtual int8_t   getByte(int index) const;
    virtual int16_t  getShort(int index) const;
    virtual int32_t  getInt(int index) const;
    virtual int64_t  getLong(int index) const;

    virtual int getBytes(int index, char* dst, int dstIndex, int length) const;
    virtual int getBytes(int index, const ChannelBufferPtr& dst, int dstIndex, int length) const;
    virtual int getBytes(int index, OutputStream* out, int length) const;

    virtual int indexOf(int fromIndex, int toIndex, int8_t value) const;
    virtual int indexOf(int fromIndex,
                        int toIndex,
                        const ChannelBufferIndexFinder::Functor& indexFinder) const;

    int readableBytes() const;
    int writableBytes() const;
    int aheadWritableBytes() const;

    virtual void readableBytes(StringPiece* bytes);
    virtual char* writableBytes(int* length);
    virtual char* aheadWritableBytes(int* length);

    virtual int setByte(int index, int value);
    virtual int setShort(int index, int value);
    virtual int setInt(int index, int value);
    virtual int setLong(int index, int64_t value);

    virtual int setBytes(int index, const StringPiece& src, int srcIndex, int length);
    virtual int setBytes(int index, const ConstChannelBufferPtr& src, int srcIndex, int length);
    virtual int setBytes(int index, InputStream* in, int length);

    virtual ChannelBufferPtr slice(int index, int length);
    virtual int slice(int index, int length, cetty::buffer::GatheringBuffer* gatheringBuffer);
    
    virtual ChannelBufferPtr newBuffer(int initialCapacity);

    void clear();

    virtual std::string toString() const;

private:
    bool checkIndex(int index, int length) const;

    virtual bool checkReadableBytes(int readableBytes) const;

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
