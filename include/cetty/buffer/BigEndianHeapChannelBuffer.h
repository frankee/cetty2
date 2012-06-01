#if !defined(CETTY_BUFFER_BIGENDIANHEAPCHANNELBUFFER_H)
#define CETTY_BUFFER_BIGENDIANHEAPCHANNELBUFFER_H

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

#include <cetty/buffer/HeapChannelBuffer.h>

namespace cetty {
namespace buffer {

/**
 * A big-endian Java heap buffer.  It is recommended to use {@link ChannelBuffers#buffer(int)}
 * and {@link ChannelBuffers#wrappedBuffer(byte[])} instead of calling the
 * constructor explicitly.
 *
 *
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 *
 * @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
 */

class BigEndianHeapChannelBuffer : public HeapChannelBuffer {
public:
    /**
     * Creates a new big-endian heap buffer with a newly allocated byte array.
     *
     * @param length the length of the new byte array
     */
    BigEndianHeapChannelBuffer(int length, int aheadBytes)
        : HeapChannelBuffer(length, aheadBytes) {}

    /**
     * Creates a new big-endian heap buffer with an existing byte array.
     *
     * @param array the byte array to wrap
     */
    BigEndianHeapChannelBuffer(const Array& arry)
        : HeapChannelBuffer(arry) {}

    BigEndianHeapChannelBuffer(const Array& arry, bool maintainedBuf)
        : HeapChannelBuffer(arry, maintainedBuf) {
    }

    virtual ~BigEndianHeapChannelBuffer() {}

    virtual ChannelBufferFactory& factory() const;
    virtual ByteOrder order() const;

    virtual boost::int16_t getShort(int index) const;
    virtual boost::int32_t getInt(int index) const;
    virtual boost::int64_t getLong(int index) const;

    virtual int setShort(int index, int value);
    virtual int setInt(int index, int value);
    virtual int setLong(int index, boost::int64_t value);

    virtual ChannelBufferPtr copy(int index, int length) const;

private:
    BigEndianHeapChannelBuffer(const Array& arry, int readerIndex, int writerIndex)
        : HeapChannelBuffer(arry, readerIndex, writerIndex) {
    }
};

}
}

#endif //#if !defined(CETTY_BUFFER_BIGENDIANHEAPCHANNELBUFFER_H)

// Local Variables:
// mode: c++
// End:

