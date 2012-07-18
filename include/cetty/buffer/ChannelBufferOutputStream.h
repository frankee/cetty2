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

class ChannelBufferOutputStream : public OutputStream , public DataOutput {
public:
    /**
     * Creates a new stream which writes data to the specified <tt>buffer</tt>.
     */
    public ChannelBufferOutputStream(ChannelBuffer buffer) {
        if (buffer == null) {
            throw new NullPointerException("buffer");
        }

        this.buffer = buffer;
        startIndex = buffer.writerIndex();
    }

    /**
     * Returns the number of written bytes by this stream so far.
     */
    public int writtenBytes() {
        return buffer.writerIndex() - startIndex;
    }

    @Override
    public void write(byte[] b, int off, int len) throws IOException {
        if (len == 0) {
            return;
        }

        buffer.writeBytes(b, off, len);
    }

    @Override
    public void write(byte[] b) throws IOException {
        buffer.writeBytes(b);
    }

    @Override
    public void write(int b) throws IOException {
        buffer.writeByte((byte) b);
    }

    public void writeBoolean(bool v) throws IOException {
        write(v? (byte) 1 : (byte) 0);
    }

    public void writeByte(int v) throws IOException {
        write(v);
    }

    public void writeBytes(String s) throws IOException {
        int len = s.length();

        for (int i = 0; i < len; i ++) {
            write((byte) s.charAt(i));
        }
    }

    public void writeChar(int v) throws IOException {
        writeShort((short) v);
    }

    public void writeChars(String s) throws IOException {
        int len = s.length();

        for (int i = 0 ; i < len ; i ++) {
            writeChar(s.charAt(i));
        }
    }

    public void writeDouble(double v) throws IOException {
        writeLong(Double.doubleToLongBits(v));
    }

    public void writeFloat(float v) throws IOException {
        writeInt(Float.floatToIntBits(v));
    }

    public void writeInt(int v) throws IOException {
        buffer.writeInt(v);
    }

    public void writeLong(long v) throws IOException {
        buffer.writeLong(v);
    }

    public void writeShort(int v) throws IOException {
        buffer.writeShort((short) v);
    }

    public void writeUTF(String s) throws IOException {
        utf8out.writeUTF(s);
    }

    /**
     * Returns the buffer where this stream is writing data.
     */
    public ChannelBuffer buffer() {
        return buffer;
    }

private:
    ChannelBufferPtr buffer;
    int startIndex;
    DataOutputStream utf8out;
};

}
}

#endif //#if !defined(CETTY_BUFFER_CHANNELBUFFEROUTPUTSTREAM_H)

// Local Variables:
// mode: c++
// End:
