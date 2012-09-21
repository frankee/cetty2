#if !defined(CETTY_BUFFER_HEAPCHANNELBUFFER_H)
#define CETTY_BUFFER_HEAPCHANNELBUFFER_H

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
namespace buffer {

/**
 * A skeletal implementation for Java heap buffers.
 *
 *
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 *
 * @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
 */

class HeapChannelBuffer : public ChannelBuffer {
public:
    /**
     * Creates a new heap buffer with a newly allocated byte array.
     *
     * @param length the length of the new byte array
     */
    HeapChannelBuffer(int initialCapacity, int aheadBytes, int maximumCapacity);

    /**
     * Creates a new heap buffer with an existing byte array.
     *
     * @param buf the char pointer
     * @param length the length of the buf.
     */
    HeapChannelBuffer(char* buf, int length);

    /**
     * Creates a new heap buffer with an existing byte array.
     *
     * @param array the byte array to wrap
     */
    HeapChannelBuffer(char* buf, int length, int maximumCapacity, bool takeCare);

    /**
     * Creates a new heap buffer with an existing byte array.
     *
     * @param arry        the byte array to wrap
     * @param readerIndex  the initial reader index of this buffer
     * @param writerIndex  the initial writer index of this buffer
     */
    HeapChannelBuffer(char* buf,
                      int length,
                      int readerIndex,
                      int writerIndex,
                      int maximumCapacity,
                      bool takeCare);

    void init(char* buf, int length, int maxCapacity, int readerIndex, int writerIndex);

    virtual ~HeapChannelBuffer();

    virtual int capacity() const;
    virtual void capacity(int newCapacity);

    virtual void readableBytes(StringPiece* bytes);
    virtual char* writableBytes(int* length);
    virtual char* aheadWritableBytes(int* length);

    virtual int8_t  getByte(int index) const;
    virtual int16_t getShort(int index) const;
    virtual int32_t getInt(int index) const;
    virtual int64_t getLong(int index) const;

    virtual int getBytes(int index, const ChannelBufferPtr& dst, int dstIndex, int length) const;
    virtual int getBytes(int index, char* dst, int dstIndex, int length) const;
    virtual int getBytes(int index, OutputStream* out, int length) const;

    virtual int setByte(int index, int value);
    virtual int setShort(int index, int value);
    virtual int setInt(int index, int value);
    virtual int setLong(int index, int64_t value);

    virtual int setBytes(int index, const ConstChannelBufferPtr& src, int srcIndex, int length);
    virtual int setBytes(int index, const StringPiece& src, int srcIndex, int length);
    virtual int setBytes(int index, InputStream* in, int length);

    virtual ChannelBufferPtr slice(int index, int length);
    virtual int slice(int index, int length, GatheringBuffer* gatheringBuffer);

    virtual ChannelBufferPtr copy(int index, int length) const;

    virtual ChannelBufferPtr newBuffer(int initialCapacity);

private:
    /**
     *  Indicated whether to maintain the life cycle of
     *  the underlying heap byte or not.
     */
    bool ownBuffer;

    /**
     * The underlying heap byte arry that this buffer is wrapping.
     */
    char* buf;
    int bufSize;
};

}
}


#endif //#if !defined(CETTY_BUFFER_HEAPCHANNELBUFFER_H)

// Local Variables:
// mode: c++
// End:
