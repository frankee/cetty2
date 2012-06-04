#if !defined(CETTY_BUFFER_ABSTRACTCHANNELBUFFERTEST_H)
#define CETTY_BUFFER_ABSTRACTCHANNELBUFFERTEST_H

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
#include "boost/assert.hpp"
#include "boost/thread.hpp"

#include "gtest/gtest.h"
#include "cetty/util/Random.h"
#include "cetty/util/Exception.h"
#include "cetty/buffer/ChannelBuffer.h"
#include "cetty/buffer/ChannelBuffers.h"

#include <algorithm>
#include <stdio.h>
#include <stdlib.h>

namespace cetty { namespace buffer { 

/**
 *
 * 
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 *
 * @version $Rev: 2309 $, $Date: 2010-06-21 16:00:03 +0900 (Mon, 21 Jun 2010) $
 */
class AbstractChannelBufferTest : public testing::Test {
public:
    static const int CAPACITY = 4096; // Must be even
    static const int BLOCK_SIZE = 128;

public:
    AbstractChannelBufferTest() {}
    virtual ~AbstractChannelBufferTest() {}

protected:
    virtual void newBuffer(int capacity) = 0;
    virtual std::vector<ChannelBufferPtr>& components() = 0;

    virtual bool discardReadBytesDoesNotMoveWritableBytes() {
        return true;
    }

public:
    void SetUp() {
        boost::system_time t = boost::get_system_time();
        seed = t.time_of_day().total_nanoseconds();
        random.setSeed(seed);

        newBuffer(CAPACITY);
    }

    void TearDown() {
    }

    void initialState() {
        ASSERT_EQ(CAPACITY, buffer->capacity());
        ASSERT_EQ(0, buffer->readerIndex());
    }

    void readerIndexBoundaryCheck1() {
        try {
            buffer->writerIndex(0);
        }
        catch (const RangeException& e) {
            printf(e.what());
            FAIL();
        }
        EXPECT_THROW(buffer->readerIndex(-1), RangeException);
    }

    void readerIndexBoundaryCheck2() {
        try {
            buffer->writerIndex(buffer->capacity());
        }
        catch (const RangeException& e) {
            printf(e.what());
            FAIL();
        }
        EXPECT_THROW(buffer->readerIndex(buffer->capacity() + 1), RangeException);
    }

    void readerIndexBoundaryCheck3() {
        try {
            buffer->writerIndex(CAPACITY / 2);
        }
        catch (const RangeException& e) {
            printf(e.what());
            FAIL();
        }
        EXPECT_THROW(buffer->readerIndex(CAPACITY * 3 / 2), RangeException);
    }

    void readerIndexBoundaryCheck4() {
        EXPECT_NO_THROW(buffer->writerIndex(0));
        EXPECT_NO_THROW(buffer->readerIndex(0));
        EXPECT_NO_THROW(buffer->writerIndex(buffer->capacity()));
        EXPECT_NO_THROW(buffer->readerIndex(buffer->capacity()));
    }

    void writerIndexBoundaryCheck1() {
        EXPECT_THROW(buffer->writerIndex(-1), RangeException);
    }

    void writerIndexBoundaryCheck2() {
        try {
            buffer->writerIndex(CAPACITY);
            buffer->readerIndex(CAPACITY);
        }
        catch (const RangeException& e) {
            printf(e.what());
            FAIL();
        }
        EXPECT_THROW(buffer->writerIndex(buffer->capacity() + 1), RangeException);
    }

    void writerIndexBoundaryCheck3() {
        try {
            buffer->writerIndex(CAPACITY);
            buffer->readerIndex(CAPACITY / 2);
        }
        catch (const RangeException& e) {
            printf(e.what());
            FAIL();
        }
        EXPECT_THROW(buffer->writerIndex(CAPACITY / 4), RangeException);
    }

    void writerIndexBoundaryCheck4() {
        EXPECT_NO_THROW(buffer->writerIndex(0));
        EXPECT_NO_THROW(buffer->readerIndex(0));
        EXPECT_NO_THROW(buffer->writerIndex(CAPACITY));
    }

    void getByteBoundaryCheck1() {
        EXPECT_THROW(buffer->getByte(-1), RangeException);
    }

    void getByteBoundaryCheck2() {
        EXPECT_THROW(buffer->getByte(buffer->capacity()), RangeException);
    }

    void getShortBoundaryCheck1() {
        EXPECT_THROW(buffer->getShort(-1), RangeException);
    }

    void getShortBoundaryCheck2() {
        EXPECT_THROW(buffer->getShort(buffer->capacity() - 1), RangeException);
    }

    void getMediumBoundaryCheck1() {
        EXPECT_THROW(buffer->getMedium(-1), RangeException);
    }

    void getMediumBoundaryCheck2() {
        EXPECT_THROW(buffer->getMedium(buffer->capacity() - 2), RangeException);
    }

    void getIntBoundaryCheck1() {
        EXPECT_THROW(buffer->getInt(-1), RangeException);
    }

    void getIntBoundaryCheck2() {
        EXPECT_THROW(buffer->getInt(buffer->capacity() - 3), RangeException);
    }

    void getLongBoundaryCheck1() {
        EXPECT_THROW(buffer->getLong(-1), RangeException);
    }

    void getLongBoundaryCheck2() {
        EXPECT_THROW(buffer->getLong(buffer->capacity() - 7), RangeException);
    }

    void getByteArrayBoundaryCheck1() {
        EXPECT_THROW(buffer->getBytes(-1, Array()), RangeException);
    }

    void getByteArrayBoundaryCheck2() {
        EXPECT_THROW(buffer->getBytes(-1, Array(), 0, 0), RangeException);
    }

    void getByteArrayBoundaryCheck3() {
        char dst[4] = {0};
        buffer->setInt(0, 0x01020304);
        try {
            buffer->getBytes(0, Array(dst, 4), -1, 4);
            FAIL();
        }
        catch (const RangeException& e) {
            printf(e.what());
            // Success
        }

        // No partial copy is expected.
        ASSERT_EQ(0, dst[0]);
        ASSERT_EQ(0, dst[1]);
        ASSERT_EQ(0, dst[2]);
        ASSERT_EQ(0, dst[3]);
    }

    //
    void getByteArrayBoundaryCheck4() {
        char dst[4] = {0};
        buffer->setInt(0, 0x01020304);
        try {
            buffer->getBytes(0, Array(dst, 4), 1, 4);
            FAIL();
        }
        catch (const RangeException& e) {
            printf(e.what());
            // Success
        }

        // No partial copy is expected.
        ASSERT_EQ(0, dst[0]);
        ASSERT_EQ(0, dst[1]);
        ASSERT_EQ(0, dst[2]);
        ASSERT_EQ(0, dst[3]);
    }

    void getByteBufferBoundaryCheck() {
        //EXPECT_THROW(buffer->getBytes(-1, ByteBuffer.allocate(0)), RangeException);
    }

    void copyBoundaryCheck1() {
        EXPECT_THROW(buffer->copy(-1, 0), RangeException);
    }

    void copyBoundaryCheck2() {
        EXPECT_THROW(buffer->copy(0, buffer->capacity() + 1), RangeException);
    }

    void copyBoundaryCheck3() {
        EXPECT_THROW(buffer->copy(buffer->capacity() + 1, 0), RangeException);
    }

    void copyBoundaryCheck4() {
        EXPECT_THROW(buffer->copy(buffer->capacity(), 1), RangeException);
    }

    void setIndexBoundaryCheck1() {
        EXPECT_THROW(buffer->setIndex(-1, CAPACITY), RangeException);
    }

    void setIndexBoundaryCheck2() {
        EXPECT_THROW(buffer->setIndex(CAPACITY / 2, CAPACITY / 4), RangeException);
    }

    void setIndexBoundaryCheck3() {
        EXPECT_THROW(buffer->setIndex(0, CAPACITY + 1), RangeException);
    }

    
    void getByteBufferState() {
        /*
        ByteBuffer dst = ByteBuffer.allocate(4);
        dst.position(1);
        dst.limit(3);

        buffer->setByte(0, (byte) 1);
        buffer->setByte(1, (byte) 2);
        buffer->setByte(2, (byte) 3);
        buffer->setByte(3, (byte) 4);
        buffer->getBytes(1, dst);

        ASSERT_EQ(3, dst.position());
        ASSERT_EQ(3, dst.limit());

        dst.clear();
        ASSERT_EQ(0, dst.get(0));
        ASSERT_EQ(2, dst.get(1));
        ASSERT_EQ(3, dst.get(2));
        ASSERT_EQ(0, dst.get(3));*/
    }

    void getDirectByteBufferBoundaryCheck() {
        //EXPECT_THROW(buffer->getBytes(-1, ByteBuffer.allocateDirect(0)), RangeException);
    }

    
    void getDirectByteBufferState() {/*
        ByteBuffer dst = ByteBuffer.allocateDirect(4);
        dst.position(1);
        dst.limit(3);

        buffer->setByte(0, (byte) 1);
        buffer->setByte(1, (byte) 2);
        buffer->setByte(2, (byte) 3);
        buffer->setByte(3, (byte) 4);
        buffer->getBytes(1, dst);

        ASSERT_EQ(3, dst.position());
        ASSERT_EQ(3, dst.limit());

        dst.clear();
        ASSERT_EQ(0, dst.get(0));
        ASSERT_EQ(2, dst.get(1));
        ASSERT_EQ(3, dst.get(2));
        ASSERT_EQ(0, dst.get(3));*/
    }

    
    void testRandomByteAccess() {
        for (int i = 0; i < buffer->capacity(); i ++) {
            int value = random.nextInt(256);
            buffer->setByte(i, value);
        }

        random.setSeed(seed);
        for (int i = 0; i < buffer->capacity(); i ++) {
            boost::int8_t value = (boost::int8_t) random.nextInt(256);
            ASSERT_EQ(value, buffer->getByte(i));
        }
    }

    
    void testRandomUnsignedByteAccess() {
        for (int i = 0; i < buffer->capacity(); i ++) {
            int value = (int) random.nextInt();
            buffer->setByte(i, value);
        }

        random.setSeed(seed);
        for (int i = 0; i < buffer->capacity(); i ++) {
            int value = random.nextInt() & 0xFF;
            ASSERT_EQ(value, buffer->getUnsignedByte(i));
        }
    }

    
    void testRandomShortAccess() {
        for (int i = 0; i < buffer->capacity() - 1; i += 2) {
            short value = (short) random.nextInt();
            buffer->setShort(i, value);
        }

        random.setSeed(seed);
        for (int i = 0; i < buffer->capacity() - 1; i += 2) {
            short value = (short) random.nextInt();
            ASSERT_EQ(value, buffer->getShort(i));
        }
    }

    
    void testRandomUnsignedShortAccess() {
        for (int i = 0; i < buffer->capacity() - 1; i += 2) {
            short value = (short) random.nextInt();
            buffer->setShort(i, value);
        }

        random.setSeed(seed);
        for (int i = 0; i < buffer->capacity() - 1; i += 2) {
            int value = random.nextInt() & 0xFFFF;
            ASSERT_EQ(value, buffer->getUnsignedShort(i));
        }
    }

    
    void testRandomMediumAccess() {
        for (int i = 0; i < buffer->capacity() - 2; i += 3) {
            int value = random.nextInt();
            buffer->setMedium(i, value);
        }

        random.setSeed(seed);
        for (int i = 0; i < buffer->capacity() - 2; i += 3) {
            int value = random.nextInt() << 8 >> 8;
            ASSERT_EQ(value, buffer->getMedium(i));
        }
    }

    
    void testRandomUnsignedMediumAccess() {
        for (int i = 0; i < buffer->capacity() - 2; i += 3) {
            int value = random.nextInt();
            buffer->setMedium(i, value);
        }

        random.setSeed(seed);
        for (int i = 0; i < buffer->capacity() - 2; i += 3) {
            int value = random.nextInt() & 0x00FFFFFF;
            ASSERT_EQ(value, buffer->getUnsignedMedium(i));
        }
    }

    
    void testRandomIntAccess() {
        for (int i = 0; i < buffer->capacity() - 3; i += 4) {
            int value = random.nextInt();
            buffer->setInt(i, value);
        }

        random.setSeed(seed);
        for (int i = 0; i < buffer->capacity() - 3; i += 4) {
            int value = random.nextInt();
            ASSERT_EQ(value, buffer->getInt(i));
        }
    }

    
    void testRandomUnsignedIntAccess() {
        for (int i = 0; i < buffer->capacity() - 3; i += 4) {
            int value = random.nextInt();
            buffer->setInt(i, value);
        }

        random.setSeed(seed);
        for (int i = 0; i < buffer->capacity() - 3; i += 4) {
            long value = random.nextInt() & 0xFFFFFFFFL;
            ASSERT_EQ(value, buffer->getUnsignedInt(i));
        }
    }

    
    void testRandomLongAccess() {
        for (int i = 0; i < buffer->capacity() - 7; i += 8) {
            boost::int64_t value = random.nextLong();
            buffer->setLong(i, value);
        }

        random.setSeed(seed);
        for (int i = 0; i < buffer->capacity() - 7; i += 8) {
            boost::int64_t value = random.nextLong();
            ASSERT_EQ(value, buffer->getLong(i));
        }
    }

    
    void testSetZero() {
        buffer->clear();
        while (buffer->writable()) {
            buffer->writeByte(0xFF);
        }

        for (int i = 0; i < buffer->capacity();) {
            int randomLen = random.nextInt(32);
            int length = randomLen < buffer->capacity() - i ? randomLen : buffer->capacity() - i;
            buffer->setZero(i, length);
            i += length;
        }

        for (int i = 0; i < buffer->capacity(); i++) {
            ASSERT_EQ(0, buffer->getByte(i));
        }
    }

    
    void testSequentialByteAccess() {
        buffer->writerIndex(0);
        for (int i = 0; i < buffer->capacity(); i ++) {
            int value = random.nextInt();
            ASSERT_EQ(i, buffer->writerIndex());
            ASSERT_TRUE(buffer->writable());
            buffer->writeByte(value);
        }

        ASSERT_EQ(0, buffer->readerIndex());
        ASSERT_EQ(buffer->capacity(), buffer->writerIndex());
        ASSERT_FALSE(buffer->writable());

        random.setSeed(seed);
        for (int i = 0; i < buffer->capacity(); i ++) {
            boost::int8_t value = (boost::int8_t)random.nextInt();
            ASSERT_EQ(i, buffer->readerIndex());
            ASSERT_TRUE(buffer->readable());
            ASSERT_EQ(value, buffer->readByte());
        }

        ASSERT_EQ(buffer->capacity(), buffer->readerIndex());
        ASSERT_EQ(buffer->capacity(), buffer->writerIndex());
        ASSERT_FALSE(buffer->readable());
        ASSERT_FALSE(buffer->writable());
    }

    
    void testSequentialUnsignedByteAccess() {
        buffer->writerIndex(0);
        for (int i = 0; i < buffer->capacity(); i ++) {
            int value = random.nextInt();
            ASSERT_EQ(i, buffer->writerIndex());
            ASSERT_TRUE(buffer->writable());
            buffer->writeByte(value);
        }

        //std::string tmp = ChannelBuffers::hexDump(*buffer);

        ASSERT_EQ(0, buffer->readerIndex());
        ASSERT_EQ(buffer->capacity(), buffer->writerIndex());
        ASSERT_FALSE(buffer->writable());

        random.setSeed(seed);
        for (int i = 0; i < buffer->capacity(); i ++) {
            boost::uint8_t value = (boost::uint8_t)(random.nextInt() & 0xFF);
            ASSERT_EQ(i, buffer->readerIndex());
            ASSERT_TRUE(buffer->readable());
            ASSERT_EQ(value, buffer->readUnsignedByte());
        }

        ASSERT_EQ(buffer->capacity(), buffer->readerIndex());
        ASSERT_EQ(buffer->capacity(), buffer->writerIndex());
        ASSERT_FALSE(buffer->readable());
        ASSERT_FALSE(buffer->writable());
    }

    
    void testSequentialShortAccess() {
        buffer->writerIndex(0);
        for (int i = 0; i < buffer->capacity(); i += 2) {
            short value = (short) random.nextInt();
            ASSERT_EQ(i, buffer->writerIndex());
            ASSERT_TRUE(buffer->writable());
            buffer->writeShort(value);
        }

        ASSERT_EQ(0, buffer->readerIndex());
        ASSERT_EQ(buffer->capacity(), buffer->writerIndex());
        ASSERT_FALSE(buffer->writable());

        random.setSeed(seed);
        for (int i = 0; i < buffer->capacity(); i += 2) {
            short value = (short) random.nextInt();
            ASSERT_EQ(i, buffer->readerIndex());
            ASSERT_TRUE(buffer->readable());
            ASSERT_EQ(value, buffer->readShort());
        }

        ASSERT_EQ(buffer->capacity(), buffer->readerIndex());
        ASSERT_EQ(buffer->capacity(), buffer->writerIndex());
        ASSERT_FALSE(buffer->readable());
        ASSERT_FALSE(buffer->writable());
    }

    
    void testSequentialUnsignedShortAccess() {
        buffer->writerIndex(0);
        for (int i = 0; i < buffer->capacity(); i += 2) {
            short value = (short) random.nextInt();
            ASSERT_EQ(i, buffer->writerIndex());
            ASSERT_TRUE(buffer->writable());
            buffer->writeShort(value);
        }

        ASSERT_EQ(0, buffer->readerIndex());
        ASSERT_EQ(buffer->capacity(), buffer->writerIndex());
        ASSERT_FALSE(buffer->writable());

        random.setSeed(seed);
        for (int i = 0; i < buffer->capacity(); i += 2) {
            int value = random.nextInt() & 0xFFFF;
            ASSERT_EQ(i, buffer->readerIndex());
            ASSERT_TRUE(buffer->readable());
            ASSERT_EQ(value, buffer->readUnsignedShort());
        }

        ASSERT_EQ(buffer->capacity(), buffer->readerIndex());
        ASSERT_EQ(buffer->capacity(), buffer->writerIndex());
        ASSERT_FALSE(buffer->readable());
        ASSERT_FALSE(buffer->writable());
    }

    
    void testSequentialMediumAccess() {
        buffer->writerIndex(0);
        for (int i = 0; i < buffer->capacity() / 3 * 3; i += 3) {
            int value = random.nextInt();
            ASSERT_EQ(i, buffer->writerIndex());
            ASSERT_TRUE(buffer->writable());
            buffer->writeMedium(value);
        }

        ASSERT_EQ(0, buffer->readerIndex());
        ASSERT_EQ(buffer->capacity() / 3 * 3, buffer->writerIndex());
        ASSERT_EQ(buffer->capacity() % 3, buffer->writableBytes());

        random.setSeed(seed);
        for (int i = 0; i < buffer->capacity() / 3 * 3; i += 3) {
            int value = random.nextInt() << 8 >> 8;
            ASSERT_EQ(i, buffer->readerIndex());
            ASSERT_TRUE(buffer->readable());
            ASSERT_EQ(value, buffer->readMedium());
        }

        ASSERT_EQ(buffer->capacity() / 3 * 3, buffer->readerIndex());
        ASSERT_EQ(buffer->capacity() / 3 * 3, buffer->writerIndex());
        ASSERT_EQ(0, buffer->readableBytes());
        ASSERT_EQ(buffer->capacity() % 3, buffer->writableBytes());
    }

    
    void testSequentialUnsignedMediumAccess() {
        buffer->writerIndex(0);
        for (int i = 0; i < buffer->capacity() / 3 * 3; i += 3) {
            int value = random.nextInt() & 0x00FFFFFF;
            ASSERT_EQ(i, buffer->writerIndex());
            ASSERT_TRUE(buffer->writable());
            buffer->writeMedium(value);
        }

        ASSERT_EQ(0, buffer->readerIndex());
        ASSERT_EQ(buffer->capacity() / 3 * 3, buffer->writerIndex());
        ASSERT_EQ(buffer->capacity() % 3, buffer->writableBytes());

        random.setSeed(seed);
        for (int i = 0; i < buffer->capacity() / 3 * 3; i += 3) {
            int value = random.nextInt() & 0x00FFFFFF;
            ASSERT_EQ(i, buffer->readerIndex());
            ASSERT_TRUE(buffer->readable());
            ASSERT_EQ(value, buffer->readUnsignedMedium());
        }

        ASSERT_EQ(buffer->capacity() / 3 * 3, buffer->readerIndex());
        ASSERT_EQ(buffer->capacity() / 3 * 3, buffer->writerIndex());
        ASSERT_EQ(0, buffer->readableBytes());
        ASSERT_EQ(buffer->capacity() % 3, buffer->writableBytes());
    }

    
    void testSequentialIntAccess() {
        buffer->writerIndex(0);
        for (int i = 0; i < buffer->capacity(); i += 4) {
            int value = random.nextInt();
            ASSERT_EQ(i, buffer->writerIndex());
            ASSERT_TRUE(buffer->writable());
            buffer->writeInt(value);
        }

        ASSERT_EQ(0, buffer->readerIndex());
        ASSERT_EQ(buffer->capacity(), buffer->writerIndex());
        ASSERT_FALSE(buffer->writable());

        random.setSeed(seed);
        for (int i = 0; i < buffer->capacity(); i += 4) {
            int value = random.nextInt();
            ASSERT_EQ(i, buffer->readerIndex());
            ASSERT_TRUE(buffer->readable());
            ASSERT_EQ(value, buffer->readInt());
        }

        ASSERT_EQ(buffer->capacity(), buffer->readerIndex());
        ASSERT_EQ(buffer->capacity(), buffer->writerIndex());
        ASSERT_FALSE(buffer->readable());
        ASSERT_FALSE(buffer->writable());
    }

    
    void testSequentialUnsignedIntAccess() {
        buffer->writerIndex(0);
        for (int i = 0; i < buffer->capacity(); i += 4) {
            int value = random.nextInt();
            ASSERT_EQ(i, buffer->writerIndex());
            ASSERT_TRUE(buffer->writable());
            buffer->writeInt(value);
        }

        ASSERT_EQ(0, buffer->readerIndex());
        ASSERT_EQ(buffer->capacity(), buffer->writerIndex());
        ASSERT_FALSE(buffer->writable());

        random.setSeed(seed);
        for (int i = 0; i < buffer->capacity(); i += 4) {
            long value = random.nextInt() & 0xFFFFFFFFL;
            ASSERT_EQ(i, buffer->readerIndex());
            ASSERT_TRUE(buffer->readable());
            ASSERT_EQ(value, buffer->readUnsignedInt());
        }

        ASSERT_EQ(buffer->capacity(), buffer->readerIndex());
        ASSERT_EQ(buffer->capacity(), buffer->writerIndex());
        ASSERT_FALSE(buffer->readable());
        ASSERT_FALSE(buffer->writable());
    }

    
    void testSequentialLongAccess() {
        buffer->writerIndex(0);
        for (int i = 0; i < buffer->capacity(); i += 8) {
            boost::int64_t value = random.nextLong();
            ASSERT_EQ(i, buffer->writerIndex());
            ASSERT_TRUE(buffer->writable());
            buffer->writeLong(value);
        }

        ASSERT_EQ(0, buffer->readerIndex());
        ASSERT_EQ(buffer->capacity(), buffer->writerIndex());
        ASSERT_FALSE(buffer->writable());

        random.setSeed(seed);
        for (int i = 0; i < buffer->capacity(); i += 8) {
            boost::int64_t value = random.nextLong();
            ASSERT_EQ(i, buffer->readerIndex());
            ASSERT_TRUE(buffer->readable());
            ASSERT_EQ(value, buffer->readLong());
        }

        ASSERT_EQ(buffer->capacity(), buffer->readerIndex());
        ASSERT_EQ(buffer->capacity(), buffer->writerIndex());
        ASSERT_FALSE(buffer->readable());
        ASSERT_FALSE(buffer->writable());
    }

    
    void testByteArrayTransfer() {
        char value[BLOCK_SIZE * 2];
        for (int i = 0; i < buffer->capacity() - BLOCK_SIZE + 1; i += BLOCK_SIZE) {
            random.nextBytes(value, BLOCK_SIZE * 2);
            buffer->setBytes(i, Array(value, BLOCK_SIZE * 2), random.nextInt(BLOCK_SIZE), BLOCK_SIZE);
        }

        random.setSeed(seed);
        char expectedValue[BLOCK_SIZE * 2];
        for (int i = 0; i < buffer->capacity() - BLOCK_SIZE + 1; i += BLOCK_SIZE) {
            random.nextBytes(expectedValue, BLOCK_SIZE * 2);
            int valueOffset = random.nextInt(BLOCK_SIZE);
            buffer->getBytes(i, Array(value, BLOCK_SIZE * 2), valueOffset, BLOCK_SIZE);
            for (int j = valueOffset; j < valueOffset + BLOCK_SIZE; j ++) {
                ASSERT_EQ(expectedValue[j], value[j]);
            }
        }
    }
    
    void testRandomByteArrayTransfer1() {
        char bytes[BLOCK_SIZE];
        Array value(bytes, BLOCK_SIZE);
        for (int i = 0; i < buffer->capacity() - BLOCK_SIZE + 1; i += BLOCK_SIZE) {
            random.nextBytes(bytes, BLOCK_SIZE);
            buffer->setBytes(i, value);
        }

        random.setSeed(seed);
        char expectedValueContent[BLOCK_SIZE];
        ChannelBufferPtr expectedValue = ChannelBuffers::wrappedBuffer(Array(expectedValueContent, BLOCK_SIZE));
        for (int i = 0; i < buffer->capacity() - BLOCK_SIZE + 1; i += BLOCK_SIZE) {
            random.nextBytes(expectedValueContent, BLOCK_SIZE);
            buffer->getBytes(i, value);
            for (int j = 0; j < BLOCK_SIZE; j ++) {
                ASSERT_EQ(expectedValue->getByte(j), value[j]);
            }
        }
    }
    
    void testRandomByteArrayTransfer2() {
        char value[BLOCK_SIZE * 2];
        for (int i = 0; i < buffer->capacity() - BLOCK_SIZE + 1; i += BLOCK_SIZE) {
            random.nextBytes(value, BLOCK_SIZE * 2);
            buffer->setBytes(i, Array(value, BLOCK_SIZE * 2), random.nextInt(BLOCK_SIZE), BLOCK_SIZE);
        }

        random.setSeed(seed);
        char expectedValueContent[BLOCK_SIZE * 2];
        ChannelBufferPtr expectedValue = ChannelBuffers::wrappedBuffer(Array(expectedValueContent, BLOCK_SIZE * 2));
        for (int i = 0; i < buffer->capacity() - BLOCK_SIZE + 1; i += BLOCK_SIZE) {
            random.nextBytes(expectedValueContent, BLOCK_SIZE * 2);
            int valueOffset = random.nextInt(BLOCK_SIZE);
            buffer->getBytes(i, Array(value, BLOCK_SIZE * 2), valueOffset, BLOCK_SIZE);
            for (int j = valueOffset; j < valueOffset + BLOCK_SIZE; j ++) {
                ASSERT_EQ(expectedValue->getByte(j), value[j]);
            }
        }
    }

    
    void testRandomHeapBufferTransfer1() {
        char valueContent[BLOCK_SIZE];
        ChannelBufferPtr value = ChannelBuffers::wrappedBuffer(Array(valueContent, BLOCK_SIZE));
        for (int i = 0; i < buffer->capacity() - BLOCK_SIZE + 1; i += BLOCK_SIZE) {
            random.nextBytes(valueContent, BLOCK_SIZE);
            value->setIndex(0, BLOCK_SIZE);
            buffer->setBytes(i, *value);
            ASSERT_EQ(BLOCK_SIZE, value->readerIndex());
            ASSERT_EQ(BLOCK_SIZE, value->writerIndex());
        }

        random.setSeed(seed);
        char expectedValueContent[BLOCK_SIZE];
        ChannelBufferPtr expectedValue = ChannelBuffers::wrappedBuffer(Array(expectedValueContent, BLOCK_SIZE));
        for (int i = 0; i < buffer->capacity() - BLOCK_SIZE + 1; i += BLOCK_SIZE) {
            random.nextBytes(expectedValueContent, BLOCK_SIZE);
            value->clear();
            buffer->getBytes(i, *value);
            ASSERT_EQ(0, value->readerIndex());
            ASSERT_EQ(BLOCK_SIZE, value->writerIndex());
            for (int j = 0; j < BLOCK_SIZE; j ++) {
                ASSERT_EQ(expectedValue->getByte(j), value->getByte(j));
            }
        }
    }

    void testRandomHeapBufferTransfer2() {
        char valueContent[BLOCK_SIZE * 2];
        ChannelBufferPtr value = ChannelBuffers::wrappedBuffer(Array(valueContent, BLOCK_SIZE * 2));
        for (int i = 0; i < buffer->capacity() - BLOCK_SIZE + 1; i += BLOCK_SIZE) {
            random.nextBytes(valueContent, BLOCK_SIZE * 2);
            buffer->setBytes(i, *value, random.nextInt(BLOCK_SIZE), BLOCK_SIZE);
        }

        random.setSeed(seed);
        char expectedValueContent[BLOCK_SIZE * 2];
        ChannelBufferPtr expectedValue = ChannelBuffers::wrappedBuffer(Array(expectedValueContent, BLOCK_SIZE * 2));
        for (int i = 0; i < buffer->capacity() - BLOCK_SIZE + 1; i += BLOCK_SIZE) {
            random.nextBytes(expectedValueContent, BLOCK_SIZE * 2);
            int valueOffset = random.nextInt(BLOCK_SIZE);
            buffer->getBytes(i, *value, valueOffset, BLOCK_SIZE);
            for (int j = valueOffset; j < valueOffset + BLOCK_SIZE; j ++) {
                ASSERT_EQ(expectedValue->getByte(j), value->getByte(j));
            }
        }
    }

    void testRandomDirectBufferTransfer() {/*
        byte[] tmp = new byte[BLOCK_SIZE * 2];
        ChannelBuffer value = directBuffer(BLOCK_SIZE * 2);
        for (int i = 0; i < buffer->capacity() - BLOCK_SIZE + 1; i += BLOCK_SIZE) {
            random.nextBytes(tmp);
            value.setBytes(0, tmp, 0, value.capacity());
            buffer->setBytes(i, value, random.nextInt(BLOCK_SIZE), BLOCK_SIZE);
        }

        random.setSeed(seed);
        ChannelBuffer expectedValue = directBuffer(BLOCK_SIZE * 2);
        for (int i = 0; i < buffer->capacity() - BLOCK_SIZE + 1; i += BLOCK_SIZE) {
            random.nextBytes(tmp);
            expectedValue.setBytes(0, tmp, 0, expectedValue.capacity());
            int valueOffset = random.nextInt(BLOCK_SIZE);
            buffer->getBytes(i, value, valueOffset, BLOCK_SIZE);
            for (int j = valueOffset; j < valueOffset + BLOCK_SIZE; j ++) {
                ASSERT_EQ(expectedValue.getByte(j), value.getByte(j));
            }
        }*/
    }

    
    void testRandomByteBufferTransfer() { /*
        ByteBuffer value = ByteBuffer.allocate(BLOCK_SIZE * 2);
        for (int i = 0; i < buffer->capacity() - BLOCK_SIZE + 1; i += BLOCK_SIZE) {
            random.nextBytes(value.array());
            value.clear().position(random.nextInt(BLOCK_SIZE));
            value.limit(value.position() + BLOCK_SIZE);
            buffer->setBytes(i, value);
        }

        random.setSeed(seed);
        ByteBuffer expectedValue = ByteBuffer.allocate(BLOCK_SIZE * 2);
        for (int i = 0; i < buffer->capacity() - BLOCK_SIZE + 1; i += BLOCK_SIZE) {
            random.nextBytes(expectedValue.array());
            int valueOffset = random.nextInt(BLOCK_SIZE);
            value.clear().position(valueOffset).limit(valueOffset + BLOCK_SIZE);
            buffer->getBytes(i, value);
            ASSERT_EQ(valueOffset + BLOCK_SIZE, value.position());
            for (int j = valueOffset; j < valueOffset + BLOCK_SIZE; j ++) {
                ASSERT_EQ(expectedValue.get(j), value.get(j));
            }
        }*/
    }

    
    void testSequentialByteArrayTransfer1() {
        char value[BLOCK_SIZE];
        buffer->writerIndex(0);
        for (int i = 0; i < buffer->capacity() - BLOCK_SIZE + 1; i += BLOCK_SIZE) {
            random.nextBytes(value, BLOCK_SIZE);
            ASSERT_EQ(0, buffer->readerIndex());
            ASSERT_EQ(i, buffer->writerIndex());
            buffer->writeBytes(Array(value, BLOCK_SIZE));
        }

        random.setSeed(seed);
        char expectedValue[BLOCK_SIZE];
        for (int i = 0; i < buffer->capacity() - BLOCK_SIZE + 1; i += BLOCK_SIZE) {
            random.nextBytes(expectedValue, BLOCK_SIZE);
            
            ASSERT_EQ(i, buffer->readerIndex());
            ASSERT_EQ(CAPACITY, buffer->writerIndex());
            buffer->readBytes(Array(value, BLOCK_SIZE));
            for (int j = 0; j < BLOCK_SIZE; j ++) {
                ASSERT_EQ(expectedValue[j], value[j]);
            }
        }
    }

    void testSequentialByteArrayTransfer2() {
        char value[BLOCK_SIZE * 2];
        buffer->writerIndex(0);
        for (int i = 0; i < buffer->capacity() - BLOCK_SIZE + 1; i += BLOCK_SIZE) {
            random.nextBytes(value, BLOCK_SIZE * 2);
            
            ASSERT_EQ(0, buffer->readerIndex());
            ASSERT_EQ(i, buffer->writerIndex());
            int readerIndex = random.nextInt(BLOCK_SIZE);
            buffer->writeBytes(Array(value, BLOCK_SIZE * 2), readerIndex, BLOCK_SIZE);
        }

        random.setSeed(seed);
        char expectedValue[BLOCK_SIZE * 2];
        for (int i = 0; i < buffer->capacity() - BLOCK_SIZE + 1; i += BLOCK_SIZE) {
            random.nextBytes(expectedValue, BLOCK_SIZE * 2);
            int valueOffset = random.nextInt(BLOCK_SIZE);

            ASSERT_EQ(i, buffer->readerIndex());
            ASSERT_EQ(CAPACITY, buffer->writerIndex());
            buffer->readBytes(Array(value, BLOCK_SIZE * 2), valueOffset, BLOCK_SIZE);
            for (int j = valueOffset; j < valueOffset + BLOCK_SIZE; j ++) {
                ASSERT_EQ(expectedValue[j], value[j]);
            }
        }
    }

    void testSequentialHeapBufferTransfer1() {
        char valueContent[BLOCK_SIZE * 2];
        ChannelBufferPtr value = ChannelBuffers::wrappedBuffer(Array(valueContent, BLOCK_SIZE * 2));
        buffer->writerIndex(0);
        for (int i = 0; i < buffer->capacity() - BLOCK_SIZE + 1; i += BLOCK_SIZE) {
            random.nextBytes(valueContent, BLOCK_SIZE * 2);
            ASSERT_EQ(0, buffer->readerIndex());
            ASSERT_EQ(i, buffer->writerIndex());
            buffer->writeBytes(*value, random.nextInt(BLOCK_SIZE), BLOCK_SIZE);
            ASSERT_EQ(0, value->readerIndex());
            ASSERT_EQ(BLOCK_SIZE * 2, value->writerIndex());
        }

        random.setSeed(seed);
        char expectedValueContent[BLOCK_SIZE * 2];
        ChannelBufferPtr expectedValue = ChannelBuffers::wrappedBuffer(Array(expectedValueContent, BLOCK_SIZE * 2));
        for (int i = 0; i < buffer->capacity() - BLOCK_SIZE + 1; i += BLOCK_SIZE) {
            random.nextBytes(expectedValueContent, BLOCK_SIZE * 2);
            int valueOffset = random.nextInt(BLOCK_SIZE);

            ASSERT_EQ(i, buffer->readerIndex());
            ASSERT_EQ(CAPACITY, buffer->writerIndex());
            buffer->readBytes(*value, valueOffset, BLOCK_SIZE);
            for (int j = valueOffset; j < valueOffset + BLOCK_SIZE; j ++) {
                ASSERT_EQ(expectedValue->getByte(j), value->getByte(j));
            }
            
            ASSERT_EQ(0, value->readerIndex());
            ASSERT_EQ(BLOCK_SIZE * 2, value->writerIndex());
        }
    }
    
    void testSequentialHeapBufferTransfer2() {
        char valueContent[BLOCK_SIZE * 2];
        ChannelBufferPtr value = ChannelBuffers::wrappedBuffer(Array(valueContent, BLOCK_SIZE * 2));
        BOOST_ASSERT(value);

        buffer->writerIndex(0);
        for (int i = 0; i < buffer->capacity() - BLOCK_SIZE + 1; i += BLOCK_SIZE) {
            random.nextBytes(valueContent, BLOCK_SIZE * 2);
            ASSERT_EQ(0, buffer->readerIndex());
            ASSERT_EQ(i, buffer->writerIndex());

            int readerIndex = random.nextInt(BLOCK_SIZE);
            value->setIndex(readerIndex, readerIndex + BLOCK_SIZE);
            //value->writerIndex(readerIndex + BLOCK_SIZE);
            //value->readerIndex(readerIndex);
            buffer->writeBytes(*value);

            ASSERT_EQ(readerIndex + BLOCK_SIZE, value->writerIndex());
            ASSERT_EQ(value->writerIndex(), value->readerIndex());
        }

        random.setSeed(seed);
        char expectedValueContent[BLOCK_SIZE * 2];
        ChannelBufferPtr expectedValue = ChannelBuffers::wrappedBuffer(Array(expectedValueContent, BLOCK_SIZE * 2));
        for (int i = 0; i < buffer->capacity() - BLOCK_SIZE + 1; i += BLOCK_SIZE) {
            random.nextBytes(expectedValueContent, BLOCK_SIZE * 2);
            int valueOffset = random.nextInt(BLOCK_SIZE);

            ASSERT_EQ(i, buffer->readerIndex());
            ASSERT_EQ(CAPACITY, buffer->writerIndex());
            value->readerIndex(valueOffset);
            value->writerIndex(valueOffset);
            buffer->readBytes(*value, BLOCK_SIZE);

            for (int j = valueOffset; j < valueOffset + BLOCK_SIZE; j ++) {
                ASSERT_EQ(expectedValue->getByte(j), value->getByte(j));
            }
            ASSERT_EQ(valueOffset, value->readerIndex());
            ASSERT_EQ(valueOffset + BLOCK_SIZE, value->writerIndex());
        }
    }

    
    void testSequentialDirectBufferTransfer1() {/*
        char valueContent[BLOCK_SIZE * 2];
        ChannelBufferPtr value = ChannelBuffers::directBuffer(BLOCK_SIZE * 2);
        buffer->writerIndex(0);
        for (int i = 0; i < buffer->capacity() - BLOCK_SIZE + 1; i += BLOCK_SIZE) {
            random.nextBytes(valueContent, BLOCK_SIZE * 2);
            value->setBytes(0, Array(valueContent, BLOCK_SIZE * 2));
            ASSERT_EQ(0, buffer->readerIndex());
            ASSERT_EQ(i, buffer->writerIndex());
            
            buffer->writeBytes(*value, random.nextInt(BLOCK_SIZE), BLOCK_SIZE);
            ASSERT_EQ(0, value->readerIndex());
            ASSERT_EQ(0, value->writerIndex());
        }

        random.setSeed(seed);
        char expectedValueContent[BLOCK_SIZE * 2];
        ChannelBufferPtr expectedValue = ChannelBuffers::wrappedBuffer(Array(expectedValueContent, BLOCK_SIZE * 2));
        for (int i = 0; i < buffer->capacity() - BLOCK_SIZE + 1; i += BLOCK_SIZE) {
            random.nextBytes(expectedValueContent, BLOCK_SIZE * 2);
            int valueOffset = random.nextInt(BLOCK_SIZE);
            value->setBytes(0, Array(valueContent, BLOCK_SIZE * 2));
            ASSERT_EQ(i, buffer->readerIndex());
            ASSERT_EQ(CAPACITY, buffer->writerIndex());
            
            buffer->readBytes(*value, valueOffset, BLOCK_SIZE);
            for (int j = valueOffset; j < valueOffset + BLOCK_SIZE; j ++) {
                ASSERT_EQ(expectedValue->getByte(j), value->getByte(j));
            }
            ASSERT_EQ(0, value->readerIndex());
            ASSERT_EQ(0, value->writerIndex());
        }*/
    }

    void testSequentialDirectBufferTransfer2() {/*
        byte[] valueContent = new byte[BLOCK_SIZE * 2];
        ChannelBuffer value = directBuffer(BLOCK_SIZE * 2);
        buffer->writerIndex(0);
        for (int i = 0; i < buffer->capacity() - BLOCK_SIZE + 1; i += BLOCK_SIZE) {
            random.nextBytes(valueContent);
            value.setBytes(0, valueContent);
            ASSERT_EQ(0, buffer->readerIndex());
            ASSERT_EQ(i, buffer->writerIndex());
            int readerIndex = random.nextInt(BLOCK_SIZE);
            value.readerIndex(0);
            value.writerIndex(readerIndex + BLOCK_SIZE);
            value.readerIndex(readerIndex);
            buffer->writeBytes(value);
            ASSERT_EQ(readerIndex + BLOCK_SIZE, value.writerIndex());
            ASSERT_EQ(value.writerIndex(), value.readerIndex());
        }

        random.setSeed(seed);
        byte[] expectedValueContent = new byte[BLOCK_SIZE * 2];
        ChannelBuffer expectedValue = wrappedBuffer(expectedValueContent);
        for (int i = 0; i < buffer->capacity() - BLOCK_SIZE + 1; i += BLOCK_SIZE) {
            random.nextBytes(expectedValueContent);
            value.setBytes(0, valueContent);
            int valueOffset = random.nextInt(BLOCK_SIZE);
            ASSERT_EQ(i, buffer->readerIndex());
            ASSERT_EQ(CAPACITY, buffer->writerIndex());
            value.readerIndex(valueOffset);
            value.writerIndex(valueOffset);
            buffer->readBytes(value, BLOCK_SIZE);
            for (int j = valueOffset; j < valueOffset + BLOCK_SIZE; j ++) {
                ASSERT_EQ(expectedValue.getByte(j), value.getByte(j));
            }
            ASSERT_EQ(valueOffset, value.readerIndex());
            ASSERT_EQ(valueOffset + BLOCK_SIZE, value.writerIndex());
        }*/
    }

    void testSequentialByteBufferBackedHeapBufferTransfer1() {/*
        byte[] valueContent = new byte[BLOCK_SIZE * 2];
        ChannelBuffer value = wrappedBuffer(ByteBuffer.allocate(BLOCK_SIZE * 2));
        value.writerIndex(0);
        buffer->writerIndex(0);
        for (int i = 0; i < buffer->capacity() - BLOCK_SIZE + 1; i += BLOCK_SIZE) {
            random.nextBytes(valueContent);
            value.setBytes(0, valueContent);
            ASSERT_EQ(0, buffer->readerIndex());
            ASSERT_EQ(i, buffer->writerIndex());
            buffer->writeBytes(value, random.nextInt(BLOCK_SIZE), BLOCK_SIZE);
            ASSERT_EQ(0, value.readerIndex());
            ASSERT_EQ(0, value.writerIndex());
        }

        random.setSeed(seed);
        byte[] expectedValueContent = new byte[BLOCK_SIZE * 2];
        ChannelBuffer expectedValue = wrappedBuffer(expectedValueContent);
        for (int i = 0; i < buffer->capacity() - BLOCK_SIZE + 1; i += BLOCK_SIZE) {
            random.nextBytes(expectedValueContent);
            int valueOffset = random.nextInt(BLOCK_SIZE);
            value.setBytes(0, valueContent);
            ASSERT_EQ(i, buffer->readerIndex());
            ASSERT_EQ(CAPACITY, buffer->writerIndex());
            buffer->readBytes(value, valueOffset, BLOCK_SIZE);
            for (int j = valueOffset; j < valueOffset + BLOCK_SIZE; j ++) {
                ASSERT_EQ(expectedValue.getByte(j), value.getByte(j));
            }
            ASSERT_EQ(0, value.readerIndex());
            ASSERT_EQ(0, value.writerIndex());
        }*/
    }

    
    void testSequentialByteBufferBackedHeapBufferTransfer2() {/*
        byte[] valueContent = new byte[BLOCK_SIZE * 2];
        ChannelBuffer value = wrappedBuffer(ByteBuffer.allocate(BLOCK_SIZE * 2));
        value.writerIndex(0);
        buffer->writerIndex(0);
        for (int i = 0; i < buffer->capacity() - BLOCK_SIZE + 1; i += BLOCK_SIZE) {
            random.nextBytes(valueContent);
            value.setBytes(0, valueContent);
            ASSERT_EQ(0, buffer->readerIndex());
            ASSERT_EQ(i, buffer->writerIndex());
            int readerIndex = random.nextInt(BLOCK_SIZE);
            value.readerIndex(0);
            value.writerIndex(readerIndex + BLOCK_SIZE);
            value.readerIndex(readerIndex);
            buffer->writeBytes(value);
            ASSERT_EQ(readerIndex + BLOCK_SIZE, value.writerIndex());
            ASSERT_EQ(value.writerIndex(), value.readerIndex());
        }

        random.setSeed(seed);
        byte[] expectedValueContent = new byte[BLOCK_SIZE * 2];
        ChannelBuffer expectedValue = wrappedBuffer(expectedValueContent);
        for (int i = 0; i < buffer->capacity() - BLOCK_SIZE + 1; i += BLOCK_SIZE) {
            random.nextBytes(expectedValueContent);
            value.setBytes(0, valueContent);
            int valueOffset = random.nextInt(BLOCK_SIZE);
            ASSERT_EQ(i, buffer->readerIndex());
            ASSERT_EQ(CAPACITY, buffer->writerIndex());
            value.readerIndex(valueOffset);
            value.writerIndex(valueOffset);
            buffer->readBytes(value, BLOCK_SIZE);
            for (int j = valueOffset; j < valueOffset + BLOCK_SIZE; j ++) {
                ASSERT_EQ(expectedValue.getByte(j), value.getByte(j));
            }
            ASSERT_EQ(valueOffset, value.readerIndex());
            ASSERT_EQ(valueOffset + BLOCK_SIZE, value.writerIndex());
        }*/
    }
    
    void testSequentialByteBufferTransfer() {/*
        buffer->writerIndex(0);
        ByteBuffer value = ByteBuffer.allocate(BLOCK_SIZE * 2);
        for (int i = 0; i < buffer->capacity() - BLOCK_SIZE + 1; i += BLOCK_SIZE) {
            random.nextBytes(value.array());
            value.clear().position(random.nextInt(BLOCK_SIZE));
            value.limit(value.position() + BLOCK_SIZE);
            buffer->writeBytes(value);
        }

        random.setSeed(seed);
        ByteBuffer expectedValue = ByteBuffer.allocate(BLOCK_SIZE * 2);
        for (int i = 0; i < buffer->capacity() - BLOCK_SIZE + 1; i += BLOCK_SIZE) {
            random.nextBytes(expectedValue.array());
            int valueOffset = random.nextInt(BLOCK_SIZE);
            value.clear().position(valueOffset).limit(valueOffset + BLOCK_SIZE);
            buffer->readBytes(value);
            ASSERT_EQ(valueOffset + BLOCK_SIZE, value.position());
            for (int j = valueOffset; j < valueOffset + BLOCK_SIZE; j ++) {
                ASSERT_EQ(expectedValue.get(j), value.get(j));
            }
        }*/
    }

    
    void testSequentialCopiedBufferTransfer1() {
        buffer->writerIndex(0);
        for (int i = 0; i < buffer->capacity() - BLOCK_SIZE + 1; i += BLOCK_SIZE) {
            char value[BLOCK_SIZE];
            random.nextBytes(value, BLOCK_SIZE);

            ASSERT_EQ(0, buffer->readerIndex());
            ASSERT_EQ(i, buffer->writerIndex());
            buffer->writeBytes(Array(value, BLOCK_SIZE));
        }

        random.setSeed(seed);
        char expectedValue[BLOCK_SIZE];
        for (int i = 0; i < buffer->capacity() - BLOCK_SIZE + 1; i += BLOCK_SIZE) {
            random.nextBytes(expectedValue, BLOCK_SIZE);
            ASSERT_EQ(i, buffer->readerIndex());
            ASSERT_EQ(CAPACITY, buffer->writerIndex());
            
            ChannelBufferPtr actualValue = buffer->readBytes(BLOCK_SIZE);
            ChannelBufferPtr expectedBuffer = ChannelBuffers::wrappedBuffer(Array(expectedValue, BLOCK_SIZE));
            ASSERT_TRUE(ChannelBuffers::equals(expectedBuffer, actualValue));

            // Make sure if it is a copied buffer->
            actualValue->setByte(0, (actualValue->getByte(0) + 1));
            ASSERT_FALSE(buffer->getByte(i) == actualValue->getByte(0));
        }
    }

    /*
    @SuppressWarnings("deprecation")
    void testSequentialCopiedBufferTransfer2() {
        buffer->clear();
        buffer->writeZero(buffer->capacity());
        try {
            buffer->readBytes(ChannelBufferIndexFinder.CR);
            fail();
        } catch (NoSuchElementException e) {
            // Expected
        }

        assertSame(EMPTY_BUFFER, buffer->readBytes(ChannelBufferIndexFinder.NUL));

        buffer->clear();
        buffer->writeBytes(ArrayUtil::create(3, 1, 2, 3, 4, 0));

        ChannelBuffer copy = buffer->readBytes(ChannelBufferIndexFinder.NUL);
        ASSERT_EQ(wrappedBuffer(ArrayUtil::create(3, 1, 2, 3, 4)), copy);

        // Make sure if it is a copied buffer->
        copy.setByte(0, (byte) (copy.getByte(0) + 1));
        ASSERT_FALSE(buffer->getByte(0) == copy.getByte(0));
    }
*/
    
    void testSequentialSlice1() {
        buffer->writerIndex(0);
        for (int i = 0; i < buffer->capacity() - BLOCK_SIZE + 1; i += BLOCK_SIZE) {
            char value[BLOCK_SIZE];
            random.nextBytes(value, BLOCK_SIZE);
            ASSERT_EQ(0, buffer->readerIndex());
            ASSERT_EQ(i, buffer->writerIndex());
            buffer->writeBytes(Array(value, BLOCK_SIZE));
        }

        random.setSeed(seed);
        char expectedValue[BLOCK_SIZE];
        for (int i = 0; i < buffer->capacity() - BLOCK_SIZE + 1; i += BLOCK_SIZE) {
            random.nextBytes(expectedValue, BLOCK_SIZE);
            ASSERT_EQ(i, buffer->readerIndex());
            ASSERT_EQ(CAPACITY, buffer->writerIndex());
            
            ChannelBufferPtr actualValue = buffer->readSlice(BLOCK_SIZE);
            ASSERT_TRUE(ChannelBuffers::equals(
                *ChannelBuffers::wrappedBuffer(Array(expectedValue, BLOCK_SIZE)),
                *actualValue));

            // Make sure if it is a sliced buffer->
            actualValue->setByte(0, (actualValue->getByte(0) + 1));
            ASSERT_EQ(buffer->getByte(i), actualValue->getByte(0));
        }
    }

    /*
    @SuppressWarnings("deprecation")
    void testSequentialSlice2() {
        buffer->clear();
        buffer->writeZero(buffer->capacity());
        try {
            buffer->readSlice(ChannelBufferIndexFinder.CR);
            fail();
        } catch (NoSuchElementException e) {
            // Expected
        }

        assertSame(EMPTY_BUFFER, buffer->readSlice(ChannelBufferIndexFinder.NUL));

        buffer->clear();
        buffer->writeBytes(ArrayUtil::create(3, 1, 2, 3, 4, 0));

        ChannelBuffer slice = buffer->readSlice(ChannelBufferIndexFinder.NUL);
        ASSERT_EQ(wrappedBuffer(ArrayUtil::create(3, 1, 2, 3, 4)), slice);

        // Make sure if it is a sliced buffer->
        slice.setByte(0, (byte) (slice.getByte(0) + 1));
        ASSERT_TRUE(buffer->getByte(0) == slice.getByte(0));
    }
    */
    void testWriteZero() {
        try {
            buffer->writeZero(-1);
            FAIL();
        }
        catch (const InvalidArgumentException& e) {
            printf(e.what());
            // Expected
        }

        buffer->clear();
        while (buffer->writable()) {
            buffer->writeByte(0xFF);
        }

        buffer->clear();
        for (int i = 0; i < buffer->capacity();) {
            int randomLen = random.nextInt(32);
            int length = randomLen < buffer->capacity() - i ? randomLen : buffer->capacity() - i;
            buffer->writeZero(length);
            i += length;
        }

        ASSERT_EQ(0, buffer->readerIndex());
        ASSERT_EQ(buffer->capacity(), buffer->writerIndex());

        for (int i = 0; i < buffer->capacity(); i ++) {
            ASSERT_EQ(0, buffer->getByte(i));
        }
    }

    void testDiscardReadBytes() {
        buffer->writerIndex(0);
        for (int i = 0; i < buffer->capacity(); i += 4) {
            buffer->writeInt(i);
        }
        ChannelBufferPtr copy = ChannelBuffers::copiedBuffer(buffer);

        // Make sure there's no effect if called when readerIndex is 0.
        buffer->readerIndex(CAPACITY / 4);
        buffer->markReaderIndex();
        buffer->writerIndex(CAPACITY / 3);
        buffer->markWriterIndex();
        buffer->readerIndex(0);
        buffer->writerIndex(CAPACITY / 2);
        buffer->discardReadBytes();

        ASSERT_EQ(0, buffer->readerIndex());
        ASSERT_EQ(CAPACITY / 2, buffer->writerIndex());
        ASSERT_TRUE(ChannelBuffers::equals(
            copy->slice(0, CAPACITY / 2),
            buffer->slice(0, CAPACITY / 2)));
        
        buffer->resetReaderIndex();
        ASSERT_EQ(CAPACITY / 4, buffer->readerIndex());
        buffer->resetWriterIndex();
        ASSERT_EQ(CAPACITY / 3, buffer->writerIndex());

        // Make sure bytes after writerIndex is not copied.
        buffer->readerIndex(1);
        buffer->writerIndex(CAPACITY / 2);
        buffer->discardReadBytes();

        ASSERT_EQ(0, buffer->readerIndex());
        ASSERT_EQ(CAPACITY / 2 - 1, buffer->writerIndex());
        ASSERT_TRUE(ChannelBuffers::equals(
            copy->slice(1, CAPACITY / 2 - 1),
            buffer->slice(0, CAPACITY / 2 - 1)));

        if (discardReadBytesDoesNotMoveWritableBytes()) {
            // If writable bytes were copied, the test should fail to avoid unnecessary memory bandwidth consumption.
            ASSERT_FALSE(copy->slice(CAPACITY / 2, CAPACITY / 2)->equals(*(buffer->slice(CAPACITY / 2 - 1, CAPACITY / 2))));
        }
        else {
            ASSERT_TRUE(ChannelBuffers::equals(
                copy->slice(CAPACITY / 2, CAPACITY / 2),
                buffer->slice(CAPACITY / 2 - 1, CAPACITY / 2)));
        }

        // Marks also should be relocated.
        buffer->resetReaderIndex();
        ASSERT_EQ(CAPACITY / 4 - 1, buffer->readerIndex());
        buffer->resetWriterIndex();
        ASSERT_EQ(CAPACITY / 3 - 1, buffer->writerIndex());
    }

    /**
     * The similar test case with {@link #testDiscardReadBytes()} but this one
     * discards a large chunk at once.
     */
    void testDiscardReadBytes2() {
        buffer->writerIndex(0);
        for (int i = 0; i < buffer->capacity(); i ++) {
            buffer->writeByte(i);
        }
        ChannelBufferPtr copy = ChannelBuffers::copiedBuffer(buffer);

        // Discard the first (CAPACITY / 2 - 1) bytes.
        buffer->setIndex(CAPACITY / 2 - 1, CAPACITY - 1);
        buffer->discardReadBytes();
        ASSERT_EQ(0, buffer->readerIndex());
        ASSERT_EQ(CAPACITY / 2, buffer->writerIndex());

        for (int i = 0; i < CAPACITY / 2; i ++) {
            ASSERT_TRUE(ChannelBuffers::equals(
                copy->slice(CAPACITY / 2 - 1 + i, CAPACITY / 2 - i),
                buffer->slice(i, CAPACITY / 2 - i)));
        }
    }

    
    void testStreamTransfer1() {/*
        char* expected = new char[buffer->capacity()];
        random.nextBytes(expected);

        for (int i = 0; i < buffer->capacity() - BLOCK_SIZE + 1; i += BLOCK_SIZE) {
            ByteArrayInputStream in = new ByteArrayInputStream(expected, i, BLOCK_SIZE);
            ASSERT_EQ(BLOCK_SIZE, buffer->setBytes(i, in, BLOCK_SIZE));
            ASSERT_EQ(-1, buffer->setBytes(i, in, 0));
        }

        ByteArrayOutputStream out = new ByteArrayOutputStream();
        for (int i = 0; i < buffer->capacity() - BLOCK_SIZE + 1; i += BLOCK_SIZE) {
            buffer->getBytes(i, out, BLOCK_SIZE);
        }

        ASSERT_TRUE(Arrays.equals(expected, out.toByteArray()));*/
    }

    
    void testStreamTransfer2() {/*
        byte[] expected = new byte[buffer->capacity()];
        random.nextBytes(expected);
        buffer->clear();

        for (int i = 0; i < buffer->capacity() - BLOCK_SIZE + 1; i += BLOCK_SIZE) {
            ByteArrayInputStream in = new ByteArrayInputStream(expected, i, BLOCK_SIZE);
            ASSERT_EQ(i, buffer->writerIndex());
            buffer->writeBytes(in, BLOCK_SIZE);
            ASSERT_EQ(i + BLOCK_SIZE, buffer->writerIndex());
        }

        ByteArrayOutputStream out = new ByteArrayOutputStream();
        for (int i = 0; i < buffer->capacity() - BLOCK_SIZE + 1; i += BLOCK_SIZE) {
            ASSERT_EQ(i, buffer->readerIndex());
            buffer->readBytes(out, BLOCK_SIZE);
            ASSERT_EQ(i + BLOCK_SIZE, buffer->readerIndex());
        }

        ASSERT_TRUE(Arrays.equals(expected, out.toByteArray()));*/
    }

    
    void testCopy() {
        for (int i = 0; i < buffer->capacity(); i ++) {
            int value = random.nextInt();
            buffer->setByte(i, value);
        }

        int readerIndex = CAPACITY / 3;
        int writerIndex = CAPACITY * 2 / 3;
        buffer->setIndex(readerIndex, writerIndex);

        // Make sure all properties are copied.
        ChannelBufferPtr copy = buffer->copy();
        ASSERT_EQ(0, copy->readerIndex());
        ASSERT_EQ(buffer->readableBytes(), copy->writerIndex());
        ASSERT_EQ(buffer->readableBytes(), copy->capacity());
        ASSERT_TRUE(buffer->order() == copy->order());
        for (int i = 0; i < copy->capacity(); i ++) {
            ASSERT_EQ(buffer->getByte(i + readerIndex), copy->getByte(i));
        }

        // Make sure the buffer content is independent from each other.
        buffer->setByte(readerIndex, (buffer->getByte(readerIndex) + 1));
        ASSERT_TRUE(buffer->getByte(readerIndex) != copy->getByte(0));
        
        copy->setByte(1, (copy->getByte(1) + 1));
        ASSERT_TRUE(buffer->getByte(readerIndex + 1) != copy->getByte(1));
    }
    
    void testDuplicate() {
        for (int i = 0; i < buffer->capacity(); i ++) {
            int value = random.nextInt();
            buffer->setByte(i, value);
        }

        int readerIndex = CAPACITY / 3;
        int writerIndex = CAPACITY * 2 / 3;
        buffer->setIndex(readerIndex, writerIndex);

        // Make sure all properties are copied.
        ChannelBufferPtr duplicate = buffer->duplicate();
        ASSERT_EQ(buffer->readerIndex(), duplicate->readerIndex());
        ASSERT_EQ(buffer->writerIndex(), duplicate->writerIndex());
        ASSERT_EQ(buffer->capacity(), duplicate->capacity());
        ASSERT_TRUE(buffer->order() == duplicate->order());
        for (int i = 0; i < duplicate->capacity(); i ++) {
            ASSERT_EQ(buffer->getByte(i), duplicate->getByte(i));
        }

        // Make sure the buffer content is shared.
        buffer->setByte(readerIndex, (buffer->getByte(readerIndex) + 1));
        ASSERT_TRUE(buffer->getByte(readerIndex) == duplicate->getByte(readerIndex));
        
        duplicate->setByte(1, (duplicate->getByte(1) + 1));
        ASSERT_TRUE(buffer->getByte(1) == duplicate->getByte(1));
    }
    
    void testSliceEndianness() {
        ASSERT_TRUE(buffer->order() == buffer->slice(0, buffer->capacity())->order());
        ASSERT_TRUE(buffer->order() == buffer->slice(0, buffer->capacity() - 1)->order());
        ASSERT_TRUE(buffer->order() == buffer->slice(1, buffer->capacity() - 1)->order());
        ASSERT_TRUE(buffer->order() == buffer->slice(1, buffer->capacity() - 2)->order());
    }
    
    void testSliceIndex() {
        ASSERT_EQ(0, buffer->slice(0, buffer->capacity())->readerIndex());
        ASSERT_EQ(0, buffer->slice(0, buffer->capacity() - 1)->readerIndex());
        ASSERT_EQ(0, buffer->slice(1, buffer->capacity() - 1)->readerIndex());
        ASSERT_EQ(0, buffer->slice(1, buffer->capacity() - 2)->readerIndex());

        ASSERT_EQ(buffer->capacity(), buffer->slice(0, buffer->capacity())->writerIndex());
        ASSERT_EQ(buffer->capacity() - 1, buffer->slice(0, buffer->capacity() - 1)->writerIndex());
        ASSERT_EQ(buffer->capacity() - 1, buffer->slice(1, buffer->capacity() - 1)->writerIndex());
        ASSERT_EQ(buffer->capacity() - 2, buffer->slice(1, buffer->capacity() - 2)->writerIndex());
    }
    
    void testEquals() {
        char bytes[32];
        Array value(bytes, 32);
        buffer->setIndex(0, value.length());
        random.nextBytes(bytes, 32);
        buffer->setBytes(0, value);

        ASSERT_TRUE(ChannelBuffers::equals(buffer,
            ChannelBuffers::wrappedBuffer(ByteOrder::BYTE_ORDER_BIG, value)));

        ASSERT_TRUE(ChannelBuffers::equals(buffer,
            ChannelBuffers::wrappedBuffer(ByteOrder::BYTE_ORDER_LITTLE, value)));

        value[0] ++;
        ASSERT_FALSE(buffer->equals(*ChannelBuffers::wrappedBuffer(ByteOrder::BYTE_ORDER_BIG, value)));
        ASSERT_FALSE(buffer->equals(*ChannelBuffers::wrappedBuffer(ByteOrder::BYTE_ORDER_LITTLE, value)));
    }
    
    void testCompareTo() {
        // Fill the random stuff
        char bytes[32];
        Array value(bytes, 32);

        random.nextBytes(bytes, 32);
        
        // Prevent overflow / underflow
        if (value[0] == 0) {
            value[0] ++;
        }
        else if (value[0] == -1) {
            value[0] --;
        }

        buffer->setIndex(0, value.length());
        buffer->setBytes(0, value);

        ASSERT_EQ(0, buffer->compareTo(*ChannelBuffers::wrappedBuffer(ByteOrder::BYTE_ORDER_BIG, value)));
        ASSERT_EQ(0, buffer->compareTo(*ChannelBuffers::wrappedBuffer(ByteOrder::BYTE_ORDER_LITTLE, value)));

        value[0] ++;
        ASSERT_TRUE(buffer->compareTo(*ChannelBuffers::wrappedBuffer(ByteOrder::BYTE_ORDER_BIG, value)) < 0);
        ASSERT_TRUE(buffer->compareTo(*ChannelBuffers::wrappedBuffer(ByteOrder::BYTE_ORDER_LITTLE, value)) < 0);
        value[0] -= 2;
        ASSERT_TRUE(buffer->compareTo(*ChannelBuffers::wrappedBuffer(ByteOrder::BYTE_ORDER_BIG, value)) > 0);
        ASSERT_TRUE(buffer->compareTo(*ChannelBuffers::wrappedBuffer(ByteOrder::BYTE_ORDER_LITTLE, value)) > 0);
        value[0] ++;

        ASSERT_TRUE(buffer->compareTo(*ChannelBuffers::wrappedBuffer(ByteOrder::BYTE_ORDER_BIG, value, 0, 31)) > 0);
        ASSERT_TRUE(buffer->compareTo(*ChannelBuffers::wrappedBuffer(ByteOrder::BYTE_ORDER_LITTLE, value, 0, 31)) > 0);
        ASSERT_TRUE(buffer->slice(0, 31)->compareTo(*ChannelBuffers::wrappedBuffer(ByteOrder::BYTE_ORDER_BIG, value)) < 0);
        ASSERT_TRUE(buffer->slice(0, 31)->compareTo(*ChannelBuffers::wrappedBuffer(ByteOrder::BYTE_ORDER_LITTLE, value)) < 0);
    }
    
    void testToString() {/*
        buffer->clear();
        buffer->writeBytes(ChannelBuffers::copiedBuffer("Hello, World!", CharsetUtil.ISO_8859_1));
        ASSERT_EQ("Hello, World!", buffer->toString(CharsetUtil.ISO_8859_1));*/
    }

    void testIndexOf() {
        buffer->clear();
        buffer->writeByte(1);
        buffer->writeByte(2);
        buffer->writeByte(3);
        buffer->writeByte(2);
        buffer->writeByte(1);

        ASSERT_EQ(-1, buffer->indexOf(1, 4, (boost::int8_t) 1));
        ASSERT_EQ(-1, buffer->indexOf(4, 1, (boost::int8_t) 1));
        ASSERT_EQ(1, buffer->indexOf(1, 4, (boost::int8_t) 2));
        ASSERT_EQ(3, buffer->indexOf(4, 1, (boost::int8_t) 2));
    }
    
    void testToByteBuffer1() {/*
        byte[] value = new byte[buffer->capacity()];
        random.nextBytes(value);
        buffer->clear();
        buffer->writeBytes(value);

        ASSERT_EQ(ByteBuffer.wrap(value), buffer->toByteBuffer());*/
    }

    
    void testToByteBuffer2() {/*
        byte[] value = new byte[buffer->capacity()];
        random.nextBytes(value);
        buffer->clear();
        buffer->writeBytes(value);

        for (int i = 0; i < buffer->capacity() - BLOCK_SIZE + 1; i += BLOCK_SIZE) {
            ASSERT_EQ(ByteBuffer.wrap(value, i, BLOCK_SIZE), buffer->toByteBuffer(i, BLOCK_SIZE));
        }*/
    }

    
    void testToByteBuffer3() {
        //ASSERT_TRUE(buffer->order() == buffer->toByteBuffer().order());
    }

    
    void testToByteBuffers1() {/*
        byte[] value = new byte[buffer->capacity()];
        random.nextBytes(value);
        buffer->clear();
        buffer->writeBytes(value);

        ByteBuffer[] nioBuffers = buffer->toByteBuffers();
        int length = 0;
        for (ByteBuffer b: nioBuffers) {
            length += b.remaining();
        }

        ByteBuffer nioBuffer = ByteBuffer.allocate(length);
        for (ByteBuffer b: nioBuffers) {
            nioBuffer.put(b);
        }
        nioBuffer.flip();

        ASSERT_EQ(ByteBuffer.wrap(value), nioBuffer);*/
    }

    
    void testToByteBuffers2() {/*
        byte[] value = new byte[buffer->capacity()];
        random.nextBytes(value);
        buffer->clear();
        buffer->writeBytes(value);

        for (int i = 0; i < buffer->capacity() - BLOCK_SIZE + 1; i += BLOCK_SIZE) {
            ByteBuffer[] nioBuffers = buffer->toByteBuffers(i, BLOCK_SIZE);
            ByteBuffer nioBuffer = ByteBuffer.allocate(BLOCK_SIZE);
            for (ByteBuffer b: nioBuffers) {
                nioBuffer.put(b);
            }
            nioBuffer.flip();

            ASSERT_EQ(ByteBuffer.wrap(value, i, BLOCK_SIZE), nioBuffer);
        }*/
    }

    
    void testSkipBytes1() {
        buffer->setIndex(CAPACITY / 4, CAPACITY / 2);
        buffer->skipBytes(CAPACITY / 4);
        ASSERT_EQ(CAPACITY / 4 * 2, buffer->readerIndex());

        try {
            buffer->skipBytes(CAPACITY / 4 + 1);
            FAIL();
        }
        catch (const RangeException& e) {
            printf(e.what());
            // Expected
        }

        // Should remain unchanged.
        ASSERT_EQ(CAPACITY / 4 * 2, buffer->readerIndex());
    }
    
//     @SuppressWarnings("deprecation")
//     void testSkipBytes2() {
//         buffer->clear();
//         buffer->writeZero(buffer->capacity());
// 
//         try {
//             buffer->skipBytes(ChannelBufferIndexFinder.LF);
//             fail();
//         } catch (NoSuchElementException e) {
//             // Expected
//         }
// 
//         buffer->skipBytes(ChannelBufferIndexFinder.NUL);
//         ASSERT_EQ(0, buffer->readerIndex());
// 
//         buffer->clear();
//         buffer->writeBytes(ArrayUtil::create(3, 1, 2, 3, 4, 0));
//         buffer->skipBytes(ChannelBufferIndexFinder.NUL);
//         ASSERT_EQ(4, buffer->readerIndex());
//     }

    
    void testHashCode() {/*
        ChannelBufferPtr elemA = ChannelBuffers::buffer(15);
        ChannelBufferPtr elemB = ChannelBuffers::directBuffer(15);
        elemA->writeBytes(ArrayUtil::create(3, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 1, 2, 3, 4, 5));
        elemB->writeBytes(ArrayUtil::create(3, 6, 7, 8, 9, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 9));

        Set<ChannelBuffer> set = new HashSet<ChannelBuffer>();
        set.push_back(elemA);
        set.add(elemB);

        ASSERT_EQ(2, set.size());
        ASSERT_TRUE(set.contains(elemA.copy()));
        ASSERT_TRUE(set.contains(elemB.copy()));

        buffer->clear();
        buffer->writeBytes(elemA.duplicate());

        ASSERT_TRUE(set.remove(buffer));
        ASSERT_FALSE(set.contains(elemA));
        ASSERT_EQ(1, set.size());

        buffer->clear();
        buffer->writeBytes(elemB.duplicate());
        ASSERT_TRUE(set.remove(buffer));
        ASSERT_FALSE(set.contains(elemB));
        ASSERT_EQ(0, set.size());*/
    }

protected:
    ChannelBufferPtr buffer;

private:
    boost::int64_t seed;
    cetty::util::Random random;
};

}}

#endif //#if !defined(CETTY_BUFFER_ABSTRACTCHANNELBUFFERTEST_H)
