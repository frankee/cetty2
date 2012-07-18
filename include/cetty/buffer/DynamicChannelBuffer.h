#if !defined(CETTY_BUFFER_DYNAMICCHANNELBUFFER_H)
#define CETTY_BUFFER_DYNAMICCHANNELBUFFER_H

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
* A dynamic capacity buffer which increases its capacity as needed.  It is
* recommended to use {@link ChannelBuffers#dynamicBuffer(int)} instead of
* calling the constructor explicitly.
*
*
* @author <a href="http://gleamynode.net/">Trustin Lee</a>
*
* @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
*
*/

class DynamicChannelBuffer : public ChannelBuffer {
public:
    DynamicChannelBuffer(int estimatedLength);

    DynamicChannelBuffer(ByteOrder endianness, int estimatedLength);

    DynamicChannelBuffer(ByteOrder endianness, int estimatedLength, ChannelBufferFactory& factory);

    virtual ~DynamicChannelBuffer() {}

    virtual void ensureWritableBytes(int minWritableBytes);

    virtual ChannelBufferFactory& factory() const;

    virtual ByteOrder order() const;

    virtual int capacity() const;

    virtual void readableBytes(Array* arry);
    virtual void writableBytes(Array* arry);
    virtual void aheadWritableBytes(Array* arry);

    virtual bool hasArray() const;

    virtual boost::int8_t getByte(int index) const;
    virtual boost::int16_t getShort(int index) const;
    virtual boost::int32_t getInt(int index) const;
    virtual boost::int64_t getLong(int index) const;

    virtual int getBytes(int index, Array* dst, int dstIndex, int length) const;
    virtual int getBytes(int index, const ChannelBufferPtr& dst, int dstIndex, int length) const;
    virtual int getBytes(int index, OutputStream* out, int length) const;

    virtual int setByte(int index, int value);
    virtual int setShort(int index, int value);
    virtual int setInt(int index, int value);
    virtual int setLong(int index, boost::int64_t value);

    virtual int setBytes(int index, const ConstArray& src, int srcIndex, int length);
    virtual int setBytes(int index, const ConstChannelBufferPtr& src, int srcIndex, int length);
    virtual int setBytes(int index, InputStream* in, int length);

    virtual ChannelBufferPtr copy(int index, int length) const;

    virtual ChannelBufferPtr slice(int index, int length);
    virtual int slice(int index, int length, GatheringBuffer* gatheringBuffer);

private:
    ByteOrder endianness;
    ChannelBufferFactory& bufferFactory;

    ChannelBufferPtr buffer;
};

}
}

#endif //#if !defined(CETTY_BUFFER_DYNAMICCHANNELBUFFER_H)

// Local Variables:
// mode: c++
// End:
