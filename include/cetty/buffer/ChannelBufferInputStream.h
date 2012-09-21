#if !defined(CETTY_BUFFER_CHANNELBUFFERINPUTSTREAM_H)
#define CETTY_BUFFER_CHANNELBUFFERINPUTSTREAM_H

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
#include <cetty/util/InputStream.h>

namespace cetty {
namespace buffer {

/**
 * An {@link InputStream} which reads data from a {@link ChannelBuffer}.
 * <p>
 * A read operation against this stream will occur at the <tt>readerIndex</tt>
 * of its underlying buffer and the <tt>readerIndex</tt> will increase during
 * the read operation.
 * <p>
 * This stream implements {@link DataInput} for your convenience.
 * The endianness of the stream is not always big endian but depends on
 * the endianness of the underlying buffer.
 *
 *
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 *
 * @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
 *
 * @see ChannelBufferOutputStream
 * @apiviz.uses org.jboss.netty.buffer.ChannelBuffer
 */

class ChannelBufferInputStream : public cetty::util::InputStream {
public:
    /**
     * Creates a new stream which reads data from the specified <tt>buffer</tt>
     * starting at the current <tt>readerIndex</tt> and ending at the current
     * <tt>writerIndex</tt>.
     */
    ChannelBufferInputStream(const ChannelBufferPtr& buffer)
    : buffer(buffer),
      startIndex(buffer ? buffer->readerIndex() : 0),
      endIndex(buffer ? buffer->writerIndex() : 0) {
          BOOST_ASSERT(buffer);
          buffer->markReaderIndex();
    }

    /**
     * Creates a new stream which reads data from the specified <tt>buffer</tt>
     * starting at the current <tt>readerIndex</tt> and ending at
     * <tt>readerIndex + length</tt>.
     *
     * @throws RangeException
     *         if <tt>readerIndex + length</tt> is greater than
     *            <tt>writerIndex</tt>
     */
    ChannelBufferInputStream(const ChannelBufferPtr& buffer, int length)
    : buffer(buffer) {
        BOOST_ASSERT(length >= 0 && buffer && length <= buffer->readableBytes());

        startIndex = buffer->readerIndex();
        endIndex = startIndex + length;
        buffer->markReaderIndex();
    }

    virtual void close() {
        buffer.reset();
        startIndex = endIndex = 0;
    }

    /**
     * Returns the number of read bytes by this stream so far.
     */
    int readBytes() const {
        if (buffer) {
            return buffer->readerIndex() - startIndex;
        }
        return 0;
    }

    virtual int available() const {
        if (buffer) {
            return endIndex - buffer->readerIndex();
        }
        return 0;
    }

    virtual int mark(int readlimit) {
        if (buffer) {
            buffer->markReaderIndex();
            return buffer->readerIndex();
        }
        return 0;
    }

    virtual bool markSupported() const {
        return true;
    }

    virtual void reset() {
        if (buffer) {
            buffer->resetReaderIndex();
        }
    }

    virtual void read(int8_t* val) {
        if (val && buffer && buffer->readable()) {
                *val = buffer->readByte() & 0xff;
        }
    }

    virtual int read(int8_t* bytes, int offset, int length) {
        int left = available();

        if (left == 0) {
            return -1;
        }

        length = std::min(left, length);
        buffer->readBytes(bytes, offset, length);
        return length;
    }

    virtual int skip(int n) {
        return 0;
    }

private:
    ChannelBufferPtr buffer;
    int startIndex;
    int endIndex;
};

}
}

#endif //#if !defined(CETTY_BUFFER_CHANNELBUFFERINPUTSTREAM_H)

// Local Variables:
// mode: c++
// End:
