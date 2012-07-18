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

class ChannelBufferInputStream : public InputStream, public DataInput {
public:
    /**
     * Creates a new stream which reads data from the specified <tt>buffer</tt>
     * starting at the current <tt>readerIndex</tt> and ending at the current
     * <tt>writerIndex</tt>.
     */
    ChannelBufferInputStream(const ChannelBuffer& buffer) {
        this(buffer, buffer.readableBytes());
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
    ChannelBufferInputStream(const ChannelBuffer& buffer, int length) {
        if (length < 0) {
            throw new InvalidArgumentException("length: " + length);
        }

        if (length > buffer.readableBytes()) {
            throw new RangeException();
        }

        this.buffer = buffer;
        startIndex = buffer.readerIndex();
        endIndex = startIndex + length;
        buffer.markReaderIndex();
    }

    /**
     * Returns the number of read bytes by this stream so far.
     */
    int readBytes() const {
        return buffer.readerIndex() - startIndex;
    }

    int available() const { /*throws IOException*/
        return endIndex - buffer.readerIndex();
    }

    void mark(int readlimit) {
        buffer.markReaderIndex();
    }

    bool markSupported() const {
        return true;
    }

    int read() { /*throws IOException*/
        if (!buffer.readable()) {
            return -1;
        }

        return buffer.readByte() & 0xff;
    }

    int read(boost::int8_t* b, int off, int len) { /*throws IOException */
        int available = available();

        if (available == 0) {
            return -1;
        }

        len = Math.min(available, len);
        buffer.readBytes(b, off, len);
        return len;
    }

    void reset() { /*throws IOException*/
        buffer.resetReaderIndex();
    }

    long skip(long n) { /*throws IOException */
        if (n > Integer.MAX_VALUE) {
            return skipBytes(Integer.MAX_VALUE);
        }
        else {
            return skipBytes((int) n);
        }
    }

    bool readBoolean() { /*throws IOException */
        checkAvailable(1);
        return read() != 0;
    }

    byte readByte() { /*throws IOException*/
        if (!buffer.readable()) {
            throw new EOFException();
        }

        return buffer.readByte();
    }

    public char readChar() throws IOException {
        return (char) readShort();
    }

    public double readDouble() throws IOException {
        return Double.longBitsToDouble(readLong());
    }

    public float readFloat() throws IOException {
        return Float.intBitsToFloat(readInt());
    }

    public void readFully(byte[] b) throws IOException {
        readFully(b, 0, b.length());
    }

    public void readFully(byte[] b, int off, int len) throws IOException {
        checkAvailable(len);
        buffer.readBytes(b, off, len);
    }

    public int readInt() throws IOException {
        checkAvailable(4);
        return buffer.readInt();
    }

    private final StringBuilder lineBuf = new StringBuilder();

    bool readLine(std::string& line) { /*throws IOException*/
        lineBuf.setLength(0);

        for (;;) {
            int b = read();

            if (b < 0 || b == '\n') {
                break;
            }

            lineBuf.append((char) b);
        }

        while (lineBuf.charAt(lineBuf.length() - 1) == '\r') {
            lineBuf.setLength(lineBuf.length() - 1);
        }

        return lineBuf.toString();
    }

    public long readLong() throws IOException {
        checkAvailable(8);
        return buffer.readLong();
    }

    public short readShort() throws IOException {
        checkAvailable(2);
        return buffer.readShort();
    }

    public String readUTF() throws IOException {
        return DataInputStream.readUTF(this);
    }

    public int readUnsignedByte() throws IOException {
        return readByte() & 0xff;
    }

    public int readUnsignedShort() throws IOException {
        return readShort() & 0xffff;
    }

    public int skipBytes(int n) throws IOException {
        int nBytes = Math.min(available(), n);
        buffer.skipBytes(nBytes);
        return nBytes;
    }

private:
    void checkAvailable(int fieldSize) throws IOException {
        if (fieldSize < 0) {
            throw new RangeException();
        }

        if (fieldSize > available()) {
            throw new EOFException();
        }
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
