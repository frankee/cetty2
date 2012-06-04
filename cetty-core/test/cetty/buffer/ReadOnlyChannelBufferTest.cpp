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

#include "gtest/gtest.h"
#include "cetty/buffer/ReadOnlyChannelBuffer.h"
#include "cetty/buffer/ReadOnlyBufferException.h"
#include "cetty/buffer/ChannelBuffers.h"

#include "cetty/util/Exception.h"


using namespace cetty::buffer;
using namespace cetty::util;

/**
* 
* @author <a href="http://gleamynode.net/">Trustin Lee</a>
*
* @version $Rev: 2080 $, $Date: 2010-01-26 18:04:19 +0900 (Tue, 26 Jan 2010) $
*
*/
// public class ReadOnlyChannelBufferTest {
// 
//     @Test(expected = NullPointerException.class)
//     public void shouldNotAllowNullInConstructor() {
//         new ReadOnlyChannelBuffer(null);
//     }

TEST(ReadOnlyChannelBufferTest, testUnmodifiableBuffer) {
    ChannelBufferPtr buf = ChannelBuffers::unmodifiableBuffer(ChannelBuffers::buffer(1));
    ASSERT_TRUE(boost::dynamic_pointer_cast<ReadOnlyChannelBuffer>(buf));
}

TEST(ReadOnlyChannelBufferTest, testUnwrap) {
    ChannelBufferPtr buf = ChannelBuffers::buffer(1);
    ReadOnlyChannelBufferPtr wrapBuf =
                                boost::dynamic_pointer_cast<ReadOnlyChannelBuffer>(
                                    ChannelBuffers::unmodifiableBuffer(buf));
    ASSERT_EQ(buf, wrapBuf->unwrap());
}

TEST(ReadOnlyChannelBufferTest, shouldHaveSameByteOrder) {
    ChannelBufferPtr buf = ChannelBuffers::buffer(ByteOrder::BYTE_ORDER_LITTLE, 1);
    ASSERT_TRUE(ByteOrder::BYTE_ORDER_LITTLE == ChannelBuffers::unmodifiableBuffer(buf)->order());
}

TEST(ReadOnlyChannelBufferTest, shouldReturnReadOnlyDerivedBuffer) {
    ChannelBufferPtr buf = ChannelBuffers::unmodifiableBuffer(ChannelBuffers::buffer(1));
    ASSERT_TRUE(boost::dynamic_pointer_cast<ReadOnlyChannelBuffer>(buf->duplicate()));
    ASSERT_TRUE(boost::dynamic_pointer_cast<ReadOnlyChannelBuffer>(buf->slice()));
    ASSERT_TRUE(boost::dynamic_pointer_cast<ReadOnlyChannelBuffer>(buf->slice(0, 1)));
    ASSERT_TRUE(boost::dynamic_pointer_cast<ReadOnlyChannelBuffer>(buf->duplicate()));
}

TEST(ReadOnlyChannelBufferTest, shouldReturnWritableCopy) {
    ChannelBufferPtr buf = ChannelBuffers::unmodifiableBuffer(ChannelBuffers::buffer(1));
    ASSERT_TRUE(!boost::dynamic_pointer_cast<ReadOnlyChannelBuffer>(buf->copy()));
}

TEST(ReadOnlyChannelBufferTest, shouldForwardReadCallsBlindly) {
#if 0
    ChannelBuffer buf = createStrictMock(ChannelBuffer.class);
    expect(buf.readerIndex()).andReturn(0).anyTimes();
    expect(buf.writerIndex()).andReturn(0).anyTimes();
    expect(buf.capacity()).andReturn(0).anyTimes();

    expect(buf.getBytes(1, (GatheringByteChannel) null, 2)).andReturn(3);
    buf.getBytes(4, (OutputStream) null, 5);
    buf.getBytes(6, (byte[]) null, 7, 8);
    buf.getBytes(9, (ChannelBuffer) null, 10, 11);
    buf.getBytes(12, (ByteBuffer) null);
    expect(buf.getByte(13)).andReturn(Byte.valueOf((byte) 14));
    expect(buf.getShort(15)).andReturn(Short.valueOf((short) 16));
    expect(buf.getUnsignedMedium(17)).andReturn(18);
    expect(buf.getInt(19)).andReturn(20);
    expect(buf.getLong(21)).andReturn(22L);

    ByteBuffer bb = ByteBuffer.allocate(100);
    ByteBuffer[] bbs = new ByteBuffer[] { ByteBuffer.allocate(101), ByteBuffer.allocate(102) };

    expect(buf.toByteBuffer(23, 24)).andReturn(bb);
    expect(buf.toByteBuffers(25, 26)).andReturn(bbs);
    expect(buf.capacity()).andReturn(27);

    replay(buf);

    ChannelBuffer roBuf = unmodifiableBuffer(buf);
    ASSERT_EQ(3, roBuf.getBytes(1, (GatheringByteChannel) null, 2));
    roBuf.getBytes(4, (OutputStream) null, 5);
    roBuf.getBytes(6, (byte[]) null, 7, 8);
    roBuf.getBytes(9, (ChannelBuffer) null, 10, 11);
    roBuf.getBytes(12, (ByteBuffer) null);
    ASSERT_EQ((byte) 14, roBuf.getByte(13));
    ASSERT_EQ((short) 16, roBuf.getShort(15));
    ASSERT_EQ(18, roBuf.getUnsignedMedium(17));
    ASSERT_EQ(20, roBuf.getInt(19));
    ASSERT_EQ(22L, roBuf.getLong(21));

    ByteBuffer roBB = roBuf.toByteBuffer(23, 24);
    ASSERT_EQ(100, roBB.capacity());
    ASSERT_TRUE(roBB.isReadOnly());

    ByteBuffer[] roBBs = roBuf.toByteBuffers(25, 26);
    ASSERT_EQ(2, roBBs.length);
    ASSERT_EQ(101, roBBs[0].capacity());
    ASSERT_TRUE(roBBs[0].isReadOnly());
    ASSERT_EQ(102, roBBs[1].capacity());
    ASSERT_TRUE(roBBs[1].isReadOnly());
    ASSERT_EQ(27, roBuf.capacity());

    verify(buf);
#endif
}

TEST(ReadOnlyChannelBufferTest, shouldRejectDiscardReadBytes) {
    ASSERT_THROW(
        ChannelBuffers::unmodifiableBuffer(ChannelBuffers::EMPTY_BUFFER)->discardReadBytes(),
        ReadOnlyBufferException);
}

TEST(ReadOnlyChannelBufferTest, shouldRejectSetByte) {
    ASSERT_THROW(
        ChannelBuffers::unmodifiableBuffer(ChannelBuffers::EMPTY_BUFFER)->setByte(0, 0),
        ReadOnlyBufferException);
}

TEST(ReadOnlyChannelBufferTest, shouldRejectSetShort) {
    ASSERT_THROW(
        ChannelBuffers::unmodifiableBuffer(ChannelBuffers::EMPTY_BUFFER)->setShort(0, 0),
        ReadOnlyBufferException);
}

TEST(ReadOnlyChannelBufferTest, shouldRejectSetMedium) {
    ASSERT_THROW(
        ChannelBuffers::unmodifiableBuffer(ChannelBuffers::EMPTY_BUFFER)->setMedium(0, 0),
        ReadOnlyBufferException);
}

TEST(ReadOnlyChannelBufferTest, shouldRejectSetInt) {
    ASSERT_THROW(
        ChannelBuffers::unmodifiableBuffer(ChannelBuffers::EMPTY_BUFFER)->setInt(0, 0),
        ReadOnlyBufferException);
}

TEST(ReadOnlyChannelBufferTest, shouldRejectSetLong) {
    ASSERT_THROW(
        ChannelBuffers::unmodifiableBuffer(ChannelBuffers::EMPTY_BUFFER)->setLong(0, 0),
        ReadOnlyBufferException);
}

// TEST(ReadOnlyChannelBufferTest, shouldRejectSetBytes1) {
//     ASSERT_THROW(
//         ChannelBuffers::unmodifiableBuffer(ChannelBuffers::EMPTY_BUFFER).setBytes(0, (InputStream) null, 0);
// }
// 
// TEST(ReadOnlyChannelBufferTest, shouldRejectSetBytes2) {
//     ASSERT_THROW(
//         ChannelBuffers::unmodifiableBuffer(ChannelBuffers::EMPTY_BUFFER).setBytes(0, (ScatteringByteChannel) null, 0);
// }

TEST(ReadOnlyChannelBufferTest, shouldRejectSetBytes3) {
    ASSERT_THROW(
        ChannelBuffers::unmodifiableBuffer(ChannelBuffers::EMPTY_BUFFER)->setBytes(0, Array(), 0, 0),
        ReadOnlyBufferException);
}

TEST(ReadOnlyChannelBufferTest, shouldRejectSetBytes4) {
    ASSERT_THROW(
        ChannelBuffers::unmodifiableBuffer(ChannelBuffers::EMPTY_BUFFER)->setBytes(0, *ChannelBuffers::EMPTY_BUFFER, 0, 0),
        ReadOnlyBufferException);
}

// TEST(ReadOnlyChannelBufferTest, shouldRejectSetBytes5) {
//     ASSERT_THROW(
//         ChannelBuffers::unmodifiableBuffer(ChannelBuffers::EMPTY_BUFFER).setBytes(0, (ByteBuffer) null),
//         ReadOnlyBufferException);
// }
