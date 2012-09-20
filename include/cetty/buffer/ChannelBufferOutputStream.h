#if !defined(CETTY_BUFFER_CHANNELBUFFEROUTPUTSTREAM_H)
#define CETTY_BUFFER_CHANNELBUFFEROUTPUTSTREAM_H

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
#include <cetty/util/OutputStream.h>

namespace cetty {
namespace buffer {

/**
 * An {@link OutputStream} which writes data to a {@link ChannelBuffer}.
 * <p>
 * A write operation against this stream will occur at the <tt>writerIndex</tt>
 * of its underlying buffer and the <tt>writerIndex</tt> will increase during
 * the write operation.
 * <p>
 * This stream implements {@link DataOutput} for your convenience.
 * The endianness of the stream is not always big endian but depends on
 * the endianness of the underlying buffer.
 *
 *
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 *
 * @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
 *
 * @see ChannelBufferInputStream
 * @apiviz.uses org.jboss.netty.buffer.ChannelBuffer
 */

class ChannelBufferOutputStream : public cetty::util::OutputStream {
public:
    /**
     * Creates a new stream which writes data to the specified <tt>buffer</tt>.
     */
    ChannelBufferOutputStream(const ChannelBufferPtr& buffer)
        : buffer(buffer), startIndex(buffer ? buffer->writerIndex() : 0) {
        BOOST_ASSERT(buffer);
    }

    /**
     * Returns the number of written bytes by this stream so far.
     */
    int writtenBytes() const {
        if (buffer) {
            return buffer->writerIndex() - startIndex;
        }
        return 0;
    }

    virtual int writeByte(int byte) {
        if (buffer) {
            return buffer->writeByte(byte);
        }
        return -1;
    }

    virtual int write(const int8_t* bytes, int offset, int length) {
        if (length == 0) {
            return 0;
        }

        if (buffer) {
            Array arry((char*)bytes, length);
            return buffer->writeBytes(arry, offset, length);
        }
        return -1;
    }

    virtual void flush() {}

    virtual void close() {
        buffer.reset();
        startIndex = 0;
    }

    /**
     * Returns the buffer where this stream is writing data.
     */
    const ChannelBufferPtr& getBuffer() const {
        return buffer;
    }

private:
    ChannelBufferPtr buffer;
    int startIndex;
};

}
}

#endif //#if !defined(CETTY_BUFFER_CHANNELBUFFEROUTPUTSTREAM_H)

// Local Variables:
// mode: c++
// End:
