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
#include <vector>
#include "gtest/gtest.h"
#include "cetty/buffer/ChannelBuffers.h"
#include "cetty/buffer/ArrayUtil.h"
#include "cetty/util/Exception.h"
#include "cetty/buffer/ReadOnlyBufferException.h"

using namespace cetty::buffer;
using namespace cetty::util;

TEST(ChannelBuffersTest, testCompositeWrappedBuffer) {
    ChannelBufferPtr header = ChannelBuffers::dynamicBuffer(12);
    ChannelBufferPtr payload = ChannelBuffers::dynamicBuffer(512);

    char headerBytes[12];
    char payloadBytes[512];
    header->writeBytes(Array(headerBytes, 12));
    payload->writeBytes(Array(payloadBytes, 512));

    ChannelBufferPtr buffer = ChannelBuffers::wrappedBuffer(header, payload, ChannelBufferPtr());

    ASSERT_TRUE(header->readableBytes() == 12);
    ASSERT_TRUE(payload->readableBytes() == 512);

    ASSERT_EQ(12 + 512, buffer->readableBytes());
    //ASSERT_EQ(12 + 512, buffer->toByteBuffer(0, 12 + 512).remaining());
}

TEST(ChannelBuffersTest, testHashCode) {
    std::vector<std::pair<Array, int> > vec;

    vec.push_back(std::make_pair(Array(), 1));
    vec.push_back(std::make_pair(ArrayUtil::create(1, 1), 32));
    vec.push_back(std::make_pair(ArrayUtil::create(1, 2), 33));
    vec.push_back(std::make_pair(ArrayUtil::create(2, 0, 1), 962));
    vec.push_back(std::make_pair(ArrayUtil::create(2, 1, 2), 994));
    vec.push_back(std::make_pair(ArrayUtil::create(6, 0, 1, 2, 3, 4, 5), 63504931));
    vec.push_back(std::make_pair(ArrayUtil::create(6, 6, 7, 8, 9, 0, 1), (int) 97180294697));
    vec.push_back(std::make_pair(ArrayUtil::create(4, -1, -1, -1, 0xE1), 1));

    std::vector<std::pair<Array, int> >::iterator itr = vec.begin();
    while (itr != vec.end()) {
        ASSERT_EQ(
            itr->second,
            ChannelBuffers::hashCode(*ChannelBuffers::wrappedBuffer(itr->first)));
        ++itr;
    }
}


TEST(ChannelBuffersTest, testEquals) {
    ChannelBufferPtr a;
    ChannelBufferPtr b;

    // Different length.
    a = ChannelBuffers::wrappedBuffer(ArrayUtil::create(1, 1));
    b = ChannelBuffers::wrappedBuffer(ArrayUtil::create(2, 1, 2));
    ASSERT_FALSE(ChannelBuffers::equals(a, b));

    // Same content, same firstIndex, short length.
    a = ChannelBuffers::wrappedBuffer(ArrayUtil::create(3, 1, 2, 3));
    b = ChannelBuffers::wrappedBuffer(ArrayUtil::create(3, 1, 2, 3));
    ASSERT_TRUE(ChannelBuffers::equals(a, b));

    // Same content, different firstIndex, short length.
    a = ChannelBuffers::wrappedBuffer(ArrayUtil::create(3, 1, 2, 3));
    b = ChannelBuffers::wrappedBuffer(ArrayUtil::create(5, 0, 1, 2, 3, 4), 1, 3);
    ASSERT_TRUE(ChannelBuffers::equals(a, b));

    // Different content, same firstIndex, short length.
    a = ChannelBuffers::wrappedBuffer(ArrayUtil::create(3, 1, 2, 3));
    b = ChannelBuffers::wrappedBuffer(ArrayUtil::create(3, 1, 2, 4));
    ASSERT_FALSE(ChannelBuffers::equals(a, b));

    // Different content, different firstIndex, short length.
    a = ChannelBuffers::wrappedBuffer(ArrayUtil::create(3, 1, 2, 3));
    b = ChannelBuffers::wrappedBuffer(ArrayUtil::create(5, 0, 1, 2, 4, 5), 1, 3);
    ASSERT_FALSE(ChannelBuffers::equals(a, b));

    // Same content, same firstIndex, long length.
    a = ChannelBuffers::wrappedBuffer(ArrayUtil::create(10, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10));
    b = ChannelBuffers::wrappedBuffer(ArrayUtil::create(10, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10));
    ASSERT_TRUE(ChannelBuffers::equals(a, b));

    // Same content, different firstIndex, long length.
    a = ChannelBuffers::wrappedBuffer(ArrayUtil::create(10, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10));
    b = ChannelBuffers::wrappedBuffer(ArrayUtil::create(12, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11), 1, 10);
    ASSERT_TRUE(ChannelBuffers::equals(a, b));

    // Different content, same firstIndex, long length.
    a = ChannelBuffers::wrappedBuffer(ArrayUtil::create(10, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10));
    b = ChannelBuffers::wrappedBuffer(ArrayUtil::create(10, 1, 2, 3, 4, 6, 7, 8, 5, 9, 10));
    ASSERT_FALSE(ChannelBuffers::equals(a, b));

    // Different content, different firstIndex, long length.
    a = ChannelBuffers::wrappedBuffer(ArrayUtil::create(10, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10));
    b = ChannelBuffers::wrappedBuffer(ArrayUtil::create(12, 0, 1, 2, 3, 4, 6, 7, 8, 5, 9, 10, 11), 1, 10);
    ASSERT_FALSE(ChannelBuffers::equals(a, b));
}


TEST(ChannelBuffersTest, testCompare) {
    std::vector<ChannelBufferPtr> expected;

    expected.push_back(ChannelBuffers::wrappedBuffer(ArrayUtil::create( 1, 1)));
    expected.push_back(ChannelBuffers::wrappedBuffer(ArrayUtil::create( 2, 1, 2)));
    expected.push_back(ChannelBuffers::wrappedBuffer(ArrayUtil::create(10, 1, 2, 3, 4, 5, 6, 7, 8,  9, 10)));
    expected.push_back(ChannelBuffers::wrappedBuffer(ArrayUtil::create(12, 1, 2, 3, 4, 5, 6, 7, 8,  9, 10, 11, 12)));
    expected.push_back(ChannelBuffers::wrappedBuffer(ArrayUtil::create( 1, 2)));
    expected.push_back(ChannelBuffers::wrappedBuffer(ArrayUtil::create( 2, 2, 3)));
    expected.push_back(ChannelBuffers::wrappedBuffer(ArrayUtil::create(10, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11)));
    expected.push_back(ChannelBuffers::wrappedBuffer(ArrayUtil::create(12, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13)));
    expected.push_back(ChannelBuffers::wrappedBuffer(ArrayUtil::create( 3, 2, 3, 4), 1, 1));
    expected.push_back(ChannelBuffers::wrappedBuffer(ArrayUtil::create( 4, 1, 2, 3, 4), 2, 2));
    expected.push_back(ChannelBuffers::wrappedBuffer(ArrayUtil::create(13, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14), 1, 10));
    expected.push_back(ChannelBuffers::wrappedBuffer(ArrayUtil::create(14, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14), 2, 12));
    expected.push_back(ChannelBuffers::wrappedBuffer(ArrayUtil::create( 4, 2, 3, 4, 5), 2, 1));
    expected.push_back(ChannelBuffers::wrappedBuffer(ArrayUtil::create( 5, 1, 2, 3, 4, 5), 3, 2));
    expected.push_back(ChannelBuffers::wrappedBuffer(ArrayUtil::create(13, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14), 2, 10));
    expected.push_back(ChannelBuffers::wrappedBuffer(ArrayUtil::create(15, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15), 3, 12));

    for (size_t i = 0; i < expected.size(); i ++) {
        for (size_t j = 0; j < expected.size(); j ++) {
            if (i == j) {
                ASSERT_EQ(0, ChannelBuffers::compare(*expected.at(i), *expected.at(j)));
            }
            else if (i < j) {
                ASSERT_TRUE(ChannelBuffers::compare(*expected.at(i), *expected.at(j)) < 0);
            }
            else {
                ASSERT_TRUE(ChannelBuffers::compare(*expected.at(i), *expected.at(j)) > 0);
            }
        }
    }
}


TEST(ChannelBuffersTest, shouldReturnEmptyBufferWhenLengthIsZero) {
    ASSERT_EQ(ChannelBuffers::EMPTY_BUFFER,
        ChannelBuffers::buffer(0));
    ASSERT_EQ(ChannelBuffers::EMPTY_BUFFER,
        ChannelBuffers::buffer(ByteOrder::BYTE_ORDER_LITTLE, 0));

    char tmpBytes[8];
    ASSERT_EQ(ChannelBuffers::EMPTY_BUFFER,
        ChannelBuffers::wrappedBuffer(Array()));
    ASSERT_EQ(ChannelBuffers::EMPTY_BUFFER,
        ChannelBuffers::wrappedBuffer(ByteOrder::BYTE_ORDER_LITTLE, Array()));
    ASSERT_EQ(ChannelBuffers::EMPTY_BUFFER,
        ChannelBuffers::wrappedBuffer(Array(tmpBytes, 8), 0, 0));
    ASSERT_EQ(ChannelBuffers::EMPTY_BUFFER,
        ChannelBuffers::wrappedBuffer(ByteOrder::BYTE_ORDER_LITTLE, Array(tmpBytes, 8), 0, 0));
    ASSERT_EQ(ChannelBuffers::EMPTY_BUFFER,
        ChannelBuffers::wrappedBuffer(Array(tmpBytes, 8), 8, 0));
    ASSERT_EQ(ChannelBuffers::EMPTY_BUFFER,
        ChannelBuffers::wrappedBuffer(ByteOrder::BYTE_ORDER_LITTLE, Array(tmpBytes, 8), 8, 0));
    ASSERT_EQ(ChannelBuffers::EMPTY_BUFFER,
        ChannelBuffers::wrappedBuffer(ChannelBuffers::EMPTY_BUFFER));
    ASSERT_EQ(ChannelBuffers::EMPTY_BUFFER,
        ChannelBuffers::wrappedBuffer(Array(), Array()));
    ASSERT_EQ(ChannelBuffers::EMPTY_BUFFER,
        ChannelBuffers::wrappedBuffer(std::vector<Array>()));

    ASSERT_EQ(ChannelBuffers::EMPTY_BUFFER,
        ChannelBuffers::wrappedBuffer(std::vector<ChannelBufferPtr>()));
    ASSERT_EQ(ChannelBuffers::EMPTY_BUFFER,
        ChannelBuffers::wrappedBuffer(ChannelBuffers::buffer(0), ChannelBufferPtr()));
    ASSERT_EQ(ChannelBuffers::EMPTY_BUFFER,
        ChannelBuffers::wrappedBuffer(ChannelBuffers::buffer(0), ChannelBuffers::buffer(0), ChannelBufferPtr()));

    ASSERT_EQ(ChannelBuffers::EMPTY_BUFFER,
        ChannelBuffers::copiedBuffer(Array()));
    ASSERT_EQ(ChannelBuffers::EMPTY_BUFFER,
        ChannelBuffers::copiedBuffer(ByteOrder::BYTE_ORDER_LITTLE, Array()));
    ASSERT_EQ(ChannelBuffers::EMPTY_BUFFER,
        ChannelBuffers::copiedBuffer(Array(tmpBytes, 8), 0, 0));
    ASSERT_EQ(ChannelBuffers::EMPTY_BUFFER,
        ChannelBuffers::copiedBuffer(ByteOrder::BYTE_ORDER_LITTLE, Array(tmpBytes, 8), 0, 0));
    ASSERT_EQ(ChannelBuffers::EMPTY_BUFFER,
        ChannelBuffers::copiedBuffer(Array(tmpBytes, 8), 8, 0));
    ASSERT_EQ(ChannelBuffers::EMPTY_BUFFER,
        ChannelBuffers::copiedBuffer(ByteOrder::BYTE_ORDER_LITTLE, Array(tmpBytes, 8), 8, 0));
    ASSERT_EQ(ChannelBuffers::EMPTY_BUFFER,
        ChannelBuffers::copiedBuffer(ChannelBuffers::EMPTY_BUFFER));
    ASSERT_EQ(ChannelBuffers::EMPTY_BUFFER,
        ChannelBuffers::copiedBuffer(std::vector<Array>()));
    ASSERT_EQ(ChannelBuffers::EMPTY_BUFFER,
        ChannelBuffers::copiedBuffer(Array(), Array()));
    ASSERT_EQ(ChannelBuffers::EMPTY_BUFFER,
        ChannelBuffers::copiedBuffer(std::vector<ChannelBufferPtr>()));
    ASSERT_EQ(ChannelBuffers::EMPTY_BUFFER,
        ChannelBuffers::copiedBuffer(ChannelBuffers::buffer(0), ChannelBufferPtr()));
    ASSERT_EQ(ChannelBuffers::EMPTY_BUFFER,
        ChannelBuffers::copiedBuffer(ChannelBuffers::buffer(0), ChannelBuffers::buffer(0), ChannelBufferPtr()));
}

TEST(ChannelBuffersTest, shouldAllowEmptyBufferToCreateCompositeBuffer) {
    ChannelBufferPtr buf = ChannelBuffers::wrappedBuffer(
            ChannelBuffers::EMPTY_BUFFER,
            ChannelBuffers::wrappedBuffer(ByteOrder::BYTE_ORDER_LITTLE, Array(new char[16], 16)),
            ChannelBuffers::EMPTY_BUFFER, ChannelBufferPtr());
    ASSERT_EQ(16, buf->capacity());
}

TEST(ChannelBuffersTest, testWrappedBuffer) {
    //ASSERT_EQ(16, ChannelBuffers::wrappedBuffer(ByteBuffer.allocateDirect(16)).capacity());

    ASSERT_TRUE(ChannelBuffers::equals(
            ChannelBuffers::wrappedBuffer(ArrayUtil::create(3, 1, 2, 3)),
            ChannelBuffers::wrappedBuffer(ArrayUtil::create(3, 1, 2, 3), Array())));

    ASSERT_TRUE(ChannelBuffers::equals(
            ChannelBuffers::wrappedBuffer(ArrayUtil::create(3, 1, 2, 3)),
            ChannelBuffers::wrappedBuffer(
                    ArrayUtil::create(1, 1),
                    ArrayUtil::create(1, 2),
                    ArrayUtil::create(1, 3))));

    ASSERT_TRUE(ChannelBuffers::equals(
            ChannelBuffers::wrappedBuffer(ArrayUtil::create(3, 1, 2, 3)),
            ChannelBuffers::wrappedBuffer(
                    ChannelBuffers::wrappedBuffer(ArrayUtil::create(3, 1, 2, 3)), ChannelBufferPtr())));

    ASSERT_TRUE(ChannelBuffers::equals(
            ChannelBuffers::wrappedBuffer(ArrayUtil::create(3, 1, 2, 3)),
            ChannelBuffers::wrappedBuffer(
                    ChannelBuffers::wrappedBuffer(ArrayUtil::create(1, 1)),
                    ChannelBuffers::wrappedBuffer(ArrayUtil::create(1, 2)),
                    ChannelBuffers::wrappedBuffer(ArrayUtil::create(1, 3)))));

//     ASSERT_EQ(
//             ChannelBuffers::wrappedBuffer(ArrayUtil::create( 1, 2, 3 }),
//             ChannelBuffers::wrappedBuffer(new ByteBuffer[] {
//                     ByteBuffer.wrap(ArrayUtil::create( 1, 2, 3 })
//             }));
// 
//     ASSERT_EQ(
//             ChannelBuffers::wrappedBuffer(ArrayUtil::create( 1, 2, 3 }),
//             ChannelBuffers::wrappedBuffer(
//                     ByteBuffer.wrap(ArrayUtil::create( 1 }),
//                     ByteBuffer.wrap(ArrayUtil::create( 2 }),
//                     ByteBuffer.wrap(ArrayUtil::create( 3 })));
}


TEST(ChannelBuffersTest, testCopiedBuffer) {
    //ASSERT_EQ(16, ChannelBuffers::copiedBuffer(ByteBuffer.allocateDirect(16)).capacity());

    ASSERT_TRUE(ChannelBuffers::equals(
            ChannelBuffers::wrappedBuffer(ArrayUtil::create(3, 1, 2, 3)),
            ChannelBuffers::copiedBuffer(ArrayUtil::create(3, 1, 2, 3), Array())));

    ASSERT_TRUE(ChannelBuffers::equals(
            ChannelBuffers::wrappedBuffer(ArrayUtil::create(3, 1, 2, 3)),
            ChannelBuffers::copiedBuffer(
                    ArrayUtil::create(1, 1),
                    ArrayUtil::create(1, 2),
                    ArrayUtil::create(1, 3), Array())));

    ASSERT_TRUE(ChannelBuffers::equals(
            ChannelBuffers::wrappedBuffer(ArrayUtil::create(3, 1, 2, 3)),
            ChannelBuffers::copiedBuffer(
                    ChannelBuffers::wrappedBuffer(ArrayUtil::create(3, 1, 2, 3)), ChannelBufferPtr())));

    ASSERT_TRUE(ChannelBuffers::equals(
            ChannelBuffers::wrappedBuffer(ArrayUtil::create(3, 1, 2, 3)),
            ChannelBuffers::copiedBuffer(
                    ChannelBuffers::wrappedBuffer(ArrayUtil::create(1, 1)),
                    ChannelBuffers::wrappedBuffer(ArrayUtil::create(1, 2)),
                    ChannelBuffers::wrappedBuffer(ArrayUtil::create(1, 3)),
                    ChannelBufferPtr())));

//     ASSERT_EQ(
//             ChannelBuffers::wrappedBuffer(ArrayUtil::create( 1, 2, 3 }),
//             ChannelBuffers::copiedBuffer(new ByteBuffer[] {
//                     ByteBuffer.wrap(ArrayUtil::create( 1, 2, 3 })
//             }));
// 
//     ASSERT_EQ(
//             ChannelBuffers::wrappedBuffer(ArrayUtil::create( 1, 2, 3 }),
//             ChannelBuffers::copiedBuffer(
//                     ByteBuffer.wrap(ArrayUtil::create( 1 }),
//                     ByteBuffer.wrap(ArrayUtil::create( 2 }),
//                     ByteBuffer.wrap(ArrayUtil::create( 3 })));
}


TEST(ChannelBuffersTest, testHexDump) {
    ASSERT_STREQ("", ChannelBuffers::hexDump(*ChannelBuffers::EMPTY_BUFFER).c_str());

    ASSERT_STREQ("123456", ChannelBuffers::hexDump(
        *ChannelBuffers::wrappedBuffer(ArrayUtil::create(3, 0x12, 0x34, 0x56))).c_str());

    ASSERT_STREQ("1234567890abcdef", ChannelBuffers::hexDump(
        *ChannelBuffers::wrappedBuffer(ArrayUtil::create(8, 0x12, 0x34, 0x56, 0x78, 0x90, 0xAB, 0xCD, 0xEF))).c_str());
}


TEST(ChannelBuffersTest, testSwapMedium) {
    ASSERT_EQ(0x563412, ChannelBuffers::swapMedium(0x123456));
    ASSERT_EQ(0x80, ChannelBuffers::swapMedium(0x800000));
}


TEST(ChannelBuffersTest, testUnmodifiableBuffer) {
    ChannelBufferPtr buf = ChannelBuffers::unmodifiableBuffer(ChannelBuffers::buffer(16));
    
    ASSERT_THROW(buf->discardReadBytes(), ReadOnlyBufferException);
    ASSERT_THROW(buf->setByte(0, 0), ReadOnlyBufferException);
    ASSERT_THROW(buf->setBytes(0, *ChannelBuffers::EMPTY_BUFFER, 0, 0), ReadOnlyBufferException);
    ASSERT_THROW(buf->setBytes(0, Array(), 0, 0), ReadOnlyBufferException);
    //ASSERT_THROW(buf->setBytes(0, ByteBuffer.allocate(0)), ReadOnlyBufferException);
    ASSERT_THROW(buf->setShort(0, 0), ReadOnlyBufferException);
    ASSERT_THROW(buf->setMedium(0, 0), ReadOnlyBufferException);
    ASSERT_THROW(buf->setInt(0, 0), ReadOnlyBufferException);
    ASSERT_THROW(buf->setLong(0, 0), ReadOnlyBufferException);


//     try {
//         buf->setBytes(0, EasyMock.createMock(InputStream.class), 0);
//         FAIL();
//     } catch (const UnsupportedOperationException& e) {
//         // Expected
//     }
// 
//     try {
//         buf->setBytes(0, EasyMock.createMock(ScatteringByteChannel.class), 0);
//         FAIL();
//     } catch (const UnsupportedOperationException& e) {
//         // Expected
//     }

}