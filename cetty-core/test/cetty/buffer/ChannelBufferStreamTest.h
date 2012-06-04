#if !defined(CETTY_BUFFER_CHANNELBUFFERSTREAMTEST_H)
#define CETTY_BUFFER_CHANNELBUFFERSTREAMTEST_H

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

/**
 * 
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 *
 * @version $Rev: 2080 $, $Date: 2010-01-26 18:04:19 +0900 (Tue, 26 Jan 2010) $
 *
 */

TEST(ChannelBufferStreamTest, testAll) {
    ChannelBufferPtr buf = ChannelBuffers.dynamicBuffer();

    try {
        new ChannelBufferOutputStream(null);
        FAIL();
    }
    catch (NullPointerException e) {
        // Expected
    }

    ChannelBufferOutputStream out = new ChannelBufferOutputStream(buf);
    assertSame(buf, out.buffer());
    out.writeBoolean(true);
    out.writeBoolean(false);
    out.writeByte(42);
    out.writeByte(224);
    out.writeBytes("Hello, World!");
    out.writeChars("Hello, World");
    out.writeChar('!');
    out.writeDouble(42.0);
    out.writeFloat(42.0f);
    out.writeInt(42);
    out.writeLong(42);
    out.writeShort(42);
    out.writeShort(49152);
    out.writeUTF("Hello, World!");
    out.writeBytes("The first line\r\r\n");
    out.write(new byte[0]);
    out.write(new byte[] { 1, 2, 3, 4 });
    out.write(new byte[] { 1, 3, 3, 4 }, 0, 0);
    out.close();

    try {
        new ChannelBufferInputStream(null);
        fail();
    } catch (NullPointerException e) {
        // Expected
    }

    try {
        new ChannelBufferInputStream(null, 0);
        fail();
    } catch (NullPointerException e) {
        // Expected
    }

    try {
        new ChannelBufferInputStream(buf, -1);
    } catch (IllegalArgumentException e) {
        // Expected
    }

    try {
        new ChannelBufferInputStream(buf, buf.capacity() + 1);
    } catch (IndexOutOfBoundsException e) {
        // Expected
    }

    ChannelBufferInputStream in = new ChannelBufferInputStream(buf);

    ASSERT_TRUE(in.markSupported());
    in.mark(Integer.MAX_VALUE);

    ASSERT_EQ(buf.writerIndex(), in.skip(Long.MAX_VALUE));
    ASSERT_FALSE(buf.readable());

    in.reset();
    ASSERT_EQ(0, buf.readerIndex());

    ASSERT_EQ(4, in.skip(4));
    ASSERT_EQ(4, buf.readerIndex());
    in.reset();

    ASSERT_TRUE(in.readBoolean());
    ASSERT_FALSE(in.readBoolean());
    ASSERT_EQ(42, in.readByte());
    ASSERT_EQ(224, in.readUnsignedByte());

    byte[] tmp = new byte[13];
    in.readFully(tmp);
    ASSERT_EQ("Hello, World!", new String(tmp, "ISO-8859-1"));

    ASSERT_EQ('H', in.readChar());
    ASSERT_EQ('e', in.readChar());
    ASSERT_EQ('l', in.readChar());
    ASSERT_EQ('l', in.readChar());
    ASSERT_EQ('o', in.readChar());
    ASSERT_EQ(',', in.readChar());
    ASSERT_EQ(' ', in.readChar());
    ASSERT_EQ('W', in.readChar());
    ASSERT_EQ('o', in.readChar());
    ASSERT_EQ('r', in.readChar());
    ASSERT_EQ('l', in.readChar());
    ASSERT_EQ('d', in.readChar());
    ASSERT_EQ('!', in.readChar());

    ASSERT_EQ(42.0, in.readDouble(), 0.0);
    ASSERT_EQ(42.0f, in.readFloat(), 0.0);
    ASSERT_EQ(42, in.readInt());
    ASSERT_EQ(42, in.readLong());
    ASSERT_EQ(42, in.readShort());
    ASSERT_EQ(49152, in.readUnsignedShort());

    ASSERT_EQ("Hello, World!", in.readUTF());
    ASSERT_EQ("The first line", in.readLine());

    ASSERT_EQ(4, in.read(tmp));
    ASSERT_EQ(1, tmp[0]);
    ASSERT_EQ(2, tmp[1]);
    ASSERT_EQ(3, tmp[2]);
    ASSERT_EQ(4, tmp[3]);

    ASSERT_EQ(-1, in.read());
    ASSERT_EQ(-1, in.read(tmp));

    try {
        in.readByte();
        fail();
    } catch (EOFException e) {
        // Expected
    }

    try {
        in.readFully(tmp, 0, -1);
        fail();
    } catch (IndexOutOfBoundsException e) {
        // Expected
    }

    try {
        in.readFully(tmp);
        fail();
    } catch (EOFException e) {
        // Expected
    }

    in.close();

    ASSERT_EQ(buf.readerIndex(), in.readBytes());
}


#endif //#if !defined(CETTY_BUFFER_CHANNELBUFFERSTREAMTEST_H)
