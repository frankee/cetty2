#if !defined(CETTY_BUFFER_ABSTRACTCOMPOSITECHANNELBUFFERTEST_H)
#define CETTY_BUFFER_ABSTRACTCOMPOSITECHANNELBUFFERTEST_H

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

#include "cetty/buffer/AbstractChannelBufferTest.h"
#include "cetty/buffer/ArrayUtil.h"

namespace cetty { namespace buffer { 

/**
 *
 * 
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 * @author Frederic Bregier (fredbregier@free.fr)
 * @version $Rev: 2080 $, $Date: 2010-01-26 18:04:19 +0900 (Tue, 26 Jan 2010) $
 */
class AbstractCompositeChannelBufferTest : public AbstractChannelBufferTest {
protected:
    AbstractCompositeChannelBufferTest(ByteOrder order) : order(order) {
    }

    virtual ~AbstractCompositeChannelBufferTest() {}
    
    virtual void newBuffer(int length) {
        for (int i = 0; i < length; i += 10) {
            buffers.push_back(ChannelBuffers::EMPTY_BUFFER);
            buffers.push_back(ChannelBuffers::wrappedBuffer(order, Array(new char[1], 1)));
            buffers.push_back(ChannelBuffers::EMPTY_BUFFER);
            buffers.push_back(ChannelBuffers::wrappedBuffer(order, Array(new char[2], 2)));
            buffers.push_back(ChannelBuffers::EMPTY_BUFFER);
            buffers.push_back(ChannelBuffers::wrappedBuffer(order, Array(new char[3], 3)));
            buffers.push_back(ChannelBuffers::EMPTY_BUFFER);
            buffers.push_back(ChannelBuffers::wrappedBuffer(order, Array(new char[4], 4)));
            buffers.push_back(ChannelBuffers::EMPTY_BUFFER);
            buffers.push_back(ChannelBuffers::wrappedBuffer(order, Array(new char[5], 5)));
            buffers.push_back(ChannelBuffers::EMPTY_BUFFER);
            buffers.push_back(ChannelBuffers::wrappedBuffer(order, Array(new char[6], 6)));
            buffers.push_back(ChannelBuffers::EMPTY_BUFFER);
            buffers.push_back(ChannelBuffers::wrappedBuffer(order, Array(new char[7], 7)));
            buffers.push_back(ChannelBuffers::EMPTY_BUFFER);
            buffers.push_back(ChannelBuffers::wrappedBuffer(order, Array(new char[8], 8)));
            buffers.push_back(ChannelBuffers::EMPTY_BUFFER);
            buffers.push_back(ChannelBuffers::wrappedBuffer(order, Array(new char[9], 9)));
            buffers.push_back(ChannelBuffers::EMPTY_BUFFER);
        }

        ChannelBufferPtr newbuf = ChannelBuffers::wrappedBuffer(buffers);
        newbuf->writerIndex(length);
        this->buffer = ChannelBuffers::wrappedBuffer(newbuf);

        BOOST_ASSERT(length == this->buffer->capacity());
        BOOST_ASSERT(length == this->buffer->readableBytes());
        BOOST_ASSERT(!this->buffer->writable());
        
        this->buffer->writerIndex(0);
    }

    std::vector<ChannelBufferPtr>& components() {
        return buffers;
    }

    // Composite buffer does not waste bandwidth on discardReadBytes, but
    // the test will fail in strict mode.
    bool discardReadBytesDoesNotMoveWritableBytes() {
        return false;
    }

public:
    //
    void testDiscardReadBytes3() {
        ChannelBufferPtr a;
        ChannelBufferPtr b;
        a = ChannelBuffers::wrappedBuffer(order, ArrayUtil::create(10, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10));
        b = ChannelBuffers::wrappedBuffer(
                ChannelBuffers::wrappedBuffer(order, ArrayUtil::create(10, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10), 0, 5),
                ChannelBuffers::wrappedBuffer(order, ArrayUtil::create(10, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10), 5, 5));
        
        a->skipBytes(6);
        a->markReaderIndex();
        b->skipBytes(6);
        b->markReaderIndex();
        ASSERT_EQ(a->readerIndex(), b->readerIndex());

        a->readerIndex(a->readerIndex()-1);
        b->readerIndex(b->readerIndex()-1);
        ASSERT_EQ(a->readerIndex(), b->readerIndex());
        
        a->writerIndex(a->writerIndex()-1);
        a->markWriterIndex();
        b->writerIndex(b->writerIndex()-1);
        b->markWriterIndex();
        ASSERT_EQ(a->writerIndex(), b->writerIndex());
        
        a->writerIndex(a->writerIndex()+1);
        b->writerIndex(b->writerIndex()+1);
        ASSERT_EQ(a->writerIndex(), b->writerIndex());
        ASSERT_TRUE(ChannelBuffers::equals(a, b));
        
        // now discard
        a->discardReadBytes();
        b->discardReadBytes();
        ASSERT_EQ(a->readerIndex(), b->readerIndex());
        ASSERT_EQ(a->writerIndex(), b->writerIndex());
        ASSERT_TRUE(ChannelBuffers::equals(a, b));
        
        a->resetReaderIndex();
        b->resetReaderIndex();
        ASSERT_EQ(a->readerIndex(), b->readerIndex());
        
        a->resetWriterIndex();
        b->resetWriterIndex();
        ASSERT_EQ(a->writerIndex(), b->writerIndex());
        ASSERT_TRUE(ChannelBuffers::equals(a, b));
    }

    void testCompositeWrappedBuffer() {
        ChannelBufferPtr header = ChannelBuffers::dynamicBuffer(order, 12);
        ChannelBufferPtr payload = ChannelBuffers::dynamicBuffer(order, 512);

        
        header->writeBytes(Array(new char[12], 12));
        payload->writeBytes(Array(new char[512], 512));

        ChannelBufferPtr buffer = ChannelBuffers::wrappedBuffer(header, payload, ChannelBufferPtr());

        ASSERT_TRUE(
        header->readableBytes() == 12);
        ASSERT_TRUE(payload->readableBytes() == 512);

        ASSERT_EQ(12 + 512, buffer->readableBytes());
        //ASSERT_EQ(12 + 512, buffer.toByteBuffer(0, 12 + 512).remaining());
    }

    void testSeveralBuffersEquals() {
        ChannelBufferPtr a;
        ChannelBufferPtr b;

        //XXX Same tests with several buffers in wrappedCheckedBuffer
        // Different length.
        a = ChannelBuffers::wrappedBuffer(order, ArrayUtil::create(1, 1));
        b = ChannelBuffers::wrappedBuffer(
                ChannelBuffers::wrappedBuffer(order, ArrayUtil::create(1, 1)),
                ChannelBuffers::wrappedBuffer(order, ArrayUtil::create(1, 2)),
                ChannelBufferPtr());
        ASSERT_FALSE(ChannelBuffers::equals(a, b));

        // Same content, same firstIndex, short length.
        a = ChannelBuffers::wrappedBuffer(order, ArrayUtil::create(3, 1, 2, 3));
        b = ChannelBuffers::wrappedBuffer(
                ChannelBuffers::wrappedBuffer(order, ArrayUtil::create(1, 1)),
                ChannelBuffers::wrappedBuffer(order, ArrayUtil::create(1, 2)),
                ChannelBuffers::wrappedBuffer(order, ArrayUtil::create(1, 3)),
                ChannelBufferPtr());
        ASSERT_TRUE(ChannelBuffers::equals(a, b));

        // Same content, different firstIndex, short length.
        a = ChannelBuffers::wrappedBuffer(order, ArrayUtil::create(3, 1, 2, 3));
        b = ChannelBuffers::wrappedBuffer(
                ChannelBuffers::wrappedBuffer(order, ArrayUtil::create(5, 0, 1, 2, 3, 4), 1, 2),
                ChannelBuffers::wrappedBuffer(order, ArrayUtil::create(5, 0, 1, 2, 3, 4), 3, 1),
                ChannelBufferPtr());
        ASSERT_TRUE(ChannelBuffers::equals(a, b));

        // Different content, same firstIndex, short length.
        a = ChannelBuffers::wrappedBuffer(order, ArrayUtil::create(3, 1, 2, 3));
        b = ChannelBuffers::wrappedBuffer(
                ChannelBuffers::wrappedBuffer(order, ArrayUtil::create(2, 1, 2)),
                ChannelBuffers::wrappedBuffer(order, ArrayUtil::create(1, 4)),
                ChannelBufferPtr());
        ASSERT_FALSE(ChannelBuffers::equals(a, b));

        // Different content, different firstIndex, short length.
        a = ChannelBuffers::wrappedBuffer(order, ArrayUtil::create(3, 1, 2, 3));
        b = ChannelBuffers::wrappedBuffer(
                ChannelBuffers::wrappedBuffer(order, ArrayUtil::create(5, 0, 1, 2, 4, 5), 1, 2),
                ChannelBuffers::wrappedBuffer(order, ArrayUtil::create(5, 0, 1, 2, 4, 5), 3, 1),
                ChannelBufferPtr());
        ASSERT_FALSE(ChannelBuffers::equals(a, b));

        // Same content, same firstIndex, long length.
        a = ChannelBuffers::wrappedBuffer(order, ArrayUtil::create(10, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10));
        b = ChannelBuffers::wrappedBuffer(
                ChannelBuffers::wrappedBuffer(order, ArrayUtil::create(3, 1, 2, 3)),
                ChannelBuffers::wrappedBuffer(order, ArrayUtil::create(3, 4, 5, 6)),
                ChannelBuffers::wrappedBuffer(order, ArrayUtil::create(4, 7, 8, 9, 10)),
                ChannelBufferPtr());
        ASSERT_TRUE(ChannelBuffers::equals(a, b));

        // Same content, different firstIndex, long length.
        a = ChannelBuffers::wrappedBuffer(order, ArrayUtil::create(10, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10));
        b = ChannelBuffers::wrappedBuffer(
                ChannelBuffers::wrappedBuffer(order, ArrayUtil::create(12, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11), 1, 5),
                ChannelBuffers::wrappedBuffer(order, ArrayUtil::create(12, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11), 6, 5), ChannelBufferPtr());
        ASSERT_TRUE(ChannelBuffers::equals(a, b));

        // Different content, same firstIndex, long length.
        a = ChannelBuffers::wrappedBuffer(order, ArrayUtil::create(10, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10));
        b = ChannelBuffers::wrappedBuffer(
                ChannelBuffers::wrappedBuffer(order, ArrayUtil::create(5, 1, 2, 3, 4, 6)),
                ChannelBuffers::wrappedBuffer(order, ArrayUtil::create(5, 7, 8, 5, 9, 10)), ChannelBufferPtr());
        ASSERT_FALSE(ChannelBuffers::equals(a, b));

        // Different content, different firstIndex, long length.
        a = ChannelBuffers::wrappedBuffer(order, ArrayUtil::create(10, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10));
        b = ChannelBuffers::wrappedBuffer(
                ChannelBuffers::wrappedBuffer(order, ArrayUtil::create(12, 0, 1, 2, 3, 4, 6, 7, 8, 5, 9, 10, 11), 1, 5),
                ChannelBuffers::wrappedBuffer(order, ArrayUtil::create(12, 0, 1, 2, 3, 4, 6, 7, 8, 5, 9, 10, 11), 6, 5), ChannelBufferPtr());
        ASSERT_FALSE(ChannelBuffers::equals(a, b));
    }
    
    void testWrappedBuffer() {
        ChannelBufferPtr a;
        ChannelBufferPtr b;
        //ASSERT_EQ(16, ChannelBuffers::wrappedBuffer(ChannelBuffers::wrappedBuffer(ByteBuffer.allocateDirect(16))).capacity());

        ASSERT_TRUE(ChannelBuffers::equals(
                ChannelBuffers::wrappedBuffer(ChannelBuffers::wrappedBuffer(order, ArrayUtil::create(3, 1, 2, 3))),
                ChannelBuffers::wrappedBuffer(ChannelBuffers::wrappedBuffer(order, ArrayUtil::create(3, 1, 2, 3), Array()))));

        ASSERT_TRUE(ChannelBuffers::equals(
                ChannelBuffers::wrappedBuffer(ChannelBuffers::wrappedBuffer(order, ArrayUtil::create(3, 1, 2, 3))),
                ChannelBuffers::wrappedBuffer(ChannelBuffers::wrappedBuffer(order,
                        ArrayUtil::create(1, 1),
                        ArrayUtil::create(1, 2),
                        ArrayUtil::create(1, 3),
                        Array()))));

        ASSERT_TRUE(ChannelBuffers::equals(
                ChannelBuffers::wrappedBuffer(ChannelBuffers::wrappedBuffer(order, ArrayUtil::create(3, 1, 2, 3))),
                ChannelBuffers::wrappedBuffer(ChannelBuffers::wrappedBuffer(order, ArrayUtil::create(3, 1, 2, 3)))));

        ASSERT_TRUE(ChannelBuffers::equals(
                ChannelBuffers::wrappedBuffer(ChannelBuffers::wrappedBuffer(order, ArrayUtil::create(3, 1, 2, 3))),
                ChannelBuffers::wrappedBuffer(
                        ChannelBuffers::wrappedBuffer(order, ArrayUtil::create(1, 1)),
                        ChannelBuffers::wrappedBuffer(order, ArrayUtil::create(1, 2)),
                        ChannelBuffers::wrappedBuffer(order, ArrayUtil::create(1, 3)),
                        ChannelBufferPtr())));

//         ASSERT_EQ(
//                 ChannelBuffers::wrappedBuffer(ChannelBuffers::wrappedBuffer(order, ArrayUtil::create(3, 1, 2, 3))),
//                 ChannelBuffers::wrappedBuffer(ChannelBuffers::wrappedBuffer(new ByteBuffer[] {
//                         ByteBuffer.wrap(ArrayUtil::create(3, 1, 2, 3))
//                ))));

//         ASSERT_EQ(
//                 ChannelBuffers::wrappedBuffer(ChannelBuffers::wrappedBuffer(order, ArrayUtil::create(3, 1, 2, 3))),
//                 ChannelBuffers::wrappedBuffer(ChannelBuffers::wrappedBuffer(
//                         ByteBuffer.wrap(ArrayUtil::create(3, 1)),
//                         ByteBuffer.wrap(ArrayUtil::create(3, 2)),
//                         ByteBuffer.wrap(ArrayUtil::create(3, 3)))));
    }

    void testWrittenBuffersEquals() {
        //XXX Same tests than testEquals with written AggregateChannelBuffers
        ChannelBufferPtr a, b;
        // Different length.
        a = ChannelBuffers::wrappedBuffer(order, ArrayUtil::create(1, 1 ));
        b = ChannelBuffers::wrappedBuffer(ChannelBuffers::wrappedBuffer(order, ArrayUtil::create(1, 1), ArrayUtil::create(1, 1), Array()));
        
        // to enable writeBytes
        b->writerIndex(b->writerIndex()-1);
        b->writeBytes(
                *ChannelBuffers::wrappedBuffer(order, ArrayUtil::create(1, 2)));
        
        ASSERT_FALSE(ChannelBuffers::equals(a, b));

        // Same content, same firstIndex, short length.
        a = ChannelBuffers::wrappedBuffer(order, ArrayUtil::create(1, 1, 2, 3));
        b = ChannelBuffers::wrappedBuffer(
                ChannelBuffers::wrappedBuffer(order, ArrayUtil::create(1, 1), ArrayUtil::create(1, 2), Array()));
        // to enable writeBytes
        b->writerIndex(b->writerIndex()-2);
        b->writeBytes(*ChannelBuffers::wrappedBuffer(order, ArrayUtil::create(1, 2)));
        b->writeBytes(*ChannelBuffers::wrappedBuffer(order, ArrayUtil::create(1, 3)));
        ASSERT_FALSE(ChannelBuffers::equals(a, b));

        // Same content, different firstIndex, short length.
        a = ChannelBuffers::wrappedBuffer(order, ArrayUtil::create(3, 1, 2, 3));
        b = ChannelBuffers::wrappedBuffer(ChannelBuffers::wrappedBuffer(order, ArrayUtil::create(5, 0, 1, 2, 3, 4), 1, 3));
        
        // to enable writeBytes
        b->writerIndex(b->writerIndex()-1);
        b->writeBytes(
                *ChannelBuffers::wrappedBuffer(order, ArrayUtil::create(5, 0, 1, 2, 3, 4), 3, 1));
        ASSERT_TRUE(ChannelBuffers::equals(a, b));

        // Different content, same firstIndex, short length.
        a = ChannelBuffers::wrappedBuffer(order, ArrayUtil::create(3, 1, 2, 3));
        b = ChannelBuffers::wrappedBuffer(ChannelBuffers::wrappedBuffer(order, ArrayUtil::create(2, 1, 2), ArrayUtil::create(1, 1), Array()));
        // to enable writeBytes
        b->writerIndex(b->writerIndex()-1);
        b->writeBytes(
                *ChannelBuffers::wrappedBuffer(order, ArrayUtil::create(1, 4)));
        ASSERT_FALSE(ChannelBuffers::equals(a, b));

        // Different content, different firstIndex, short length.
        a = ChannelBuffers::wrappedBuffer(order, ArrayUtil::create(3, 1, 2, 3));
        b = ChannelBuffers::wrappedBuffer(ChannelBuffers::wrappedBuffer(order, ArrayUtil::create(5, 0, 1, 2, 4, 5), 1, 3));
        // to enable writeBytes
        b->writerIndex(b->writerIndex()-1);
        b->writeBytes(
                *ChannelBuffers::wrappedBuffer(order, ArrayUtil::create(5, 0, 1, 2, 4, 5), 3, 1));
        ASSERT_FALSE(ChannelBuffers::equals(a, b));

        // Same content, same firstIndex, long length.
        a = ChannelBuffers::wrappedBuffer(order, ArrayUtil::create(10, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10));
        b = ChannelBuffers::wrappedBuffer(ChannelBuffers::wrappedBuffer(
            order, ArrayUtil::create(3, 1, 2, 3), ArrayUtil::create(7, 0, 0, 0, 0, 0, 0, 0), Array()));
        
        // to enable writeBytes
        b->writerIndex(b->writerIndex()-7);
        b->writeBytes(
                *ChannelBuffers::wrappedBuffer(order, ArrayUtil::create(3, 4, 5, 6)));
        b->writeBytes(
                *ChannelBuffers::wrappedBuffer(order, ArrayUtil::create(4, 7, 8, 9, 10)));
        ASSERT_TRUE(ChannelBuffers::equals(a, b));

        // Same content, different firstIndex, long length.
        a = ChannelBuffers::wrappedBuffer(order, ArrayUtil::create(10, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10));
        b = ChannelBuffers::wrappedBuffer(ChannelBuffers::wrappedBuffer(order, ArrayUtil::create(12, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11), 1, 10));
        // to enable writeBytes
        b->writerIndex(b->writerIndex()-5);
        b->writeBytes(
                *ChannelBuffers::wrappedBuffer(order, ArrayUtil::create(12, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11), 6, 5));
        ASSERT_TRUE(ChannelBuffers::equals(a, b));

        // Different content, same firstIndex, long length.
        a = ChannelBuffers::wrappedBuffer(order, ArrayUtil::create(10, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10));
        b = ChannelBuffers::wrappedBuffer(ChannelBuffers::wrappedBuffer(order, ArrayUtil::create(5, 1, 2, 3, 4, 6), ArrayUtil::create(5, 0, 0, 0, 0, 0), Array()));
        
        // to enable writeBytes
        b->writerIndex(b->writerIndex()-5);
        b->writeBytes(
                *ChannelBuffers::wrappedBuffer(order, ArrayUtil::create(5, 7, 8, 5, 9, 10)));
        ASSERT_FALSE(ChannelBuffers::equals(a, b));

        // Different content, different firstIndex, long length.
        a = ChannelBuffers::wrappedBuffer(order, ArrayUtil::create(10, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10));
        b = ChannelBuffers::wrappedBuffer(ChannelBuffers::wrappedBuffer(order, ArrayUtil::create(12, 0, 1, 2, 3, 4, 6, 7, 8, 5, 9, 10, 11), 1, 10));
        
        // to enable writeBytes
        b->writerIndex(b->writerIndex()-5);
        b->writeBytes(
                *ChannelBuffers::wrappedBuffer(order, ArrayUtil::create(12, 0, 1, 2, 3, 4, 6, 7, 8, 5, 9, 10, 11), 6, 5));
        ASSERT_FALSE(ChannelBuffers::equals(a, b));
    }

private:
    ByteOrder order;

    std::vector<ChannelBufferPtr> buffers;
    //ChannelBufferPtr buffer;
};

}}

#endif //#if !defined(CETTY_BUFFER_ABSTRACTCOMPOSITECHANNELBUFFERTEST_H)
