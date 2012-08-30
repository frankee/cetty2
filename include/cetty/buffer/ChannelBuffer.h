#if !defined(CETTY_BUFFER_CHANNELBUFFER_H)
#define CETTY_BUFFER_CHANNELBUFFER_H

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

#include <string>
#include <boost/cstdint.hpp>

#include <cetty/buffer/Array.h>
#include <cetty/buffer/ByteOrder.h>
#include <cetty/buffer/ChannelBufferPtr.h>
#include <cetty/buffer/ChannelBufferIndexFinder.h>
#include <cetty/util/ReferenceCounter.h>

namespace cetty {
namespace util {
class Charset;
class InputStream;
class OutputStream;
}
}

namespace cetty {
namespace buffer {

using namespace cetty::util;

class GatheringBuffer;
class ChannelBufferFactory;

/**
 * A random and sequential accessible sequence of zero or more bytes (octets).
 * This interface provides an abstract view for one or more primitive byte
 * arrays (<tt>Array</tt>) and <tt>std::string</tt>.
 *
 * <h3>Creation of a buffer</h3>
 *
 * It is recommended to create a new buffer using the helper methods in
 * {@link ChannelBuffers} rather than calling an individual implementation's
 * constructor.
 *
 * <h3>Random Access Indexing</h3>
 *
 * Just like an ordinary primitive byte array, {@link ChannelBufferPtr} uses
 * <a href="http://en.wikipedia.org/wiki/Index_(information_technology)#Array_element_identifier">zero-based indexing</a>.
 * It means the index of the first byte is always <tt>0</tt> and the index of
 * the last byte is always {@link #capacity() capacity - 1}.  For example, to
 * iterate all bytes of a buffer, you can do the following, regardless of
 * its internal implementation:
 *
 * <pre>
 * {@link ChannelBufferPtr} buffer = ...;
 * for (int i = 0; i &lt; buffer.capacity(); i ++) {
 *     byte b = buffer.getByte(i);
 *     System.out.println((char) b);
 * }
 * </pre>
 *
 * <h3>Sequential Access Indexing</h3>
 *
 * {@link ChannelBuffer} provides two pointer variables to support sequential
 * read and write operations - {@link #readerIndex() readerIndex} for a read
 * operation and {@link #writerIndex() writerIndex} for a write operation
 * respectively.  The following diagram shows how a buffer is segmented into
 * three areas by the two pointers:
 *
 * <pre>
 *      +-------------------+------------------+------------------+
 *      | discardable bytes |  readable bytes  |  writable bytes  |
 *      |                   |     (CONTENT)    |                  |
 *      +-------------------+------------------+------------------+
 *      |                   |                  |                  |
 *      0      <=      readerIndex   <=   writerIndex    <=    capacity
 * </pre>
 *
 * <h4>Readable bytes (the actual content)</h4>
 *
 * This segment is where the actual data is stored.  Any operation whose name
 * starts with <tt>read</tt> or <tt>skip</tt> will get or skip the data at the
 * current {@link #readerIndex() readerIndex} and increase it by the number of
 * read bytes.  If the argument of the read operation is also a
 * {@link ChannelBuffer} and no destination index is specified, the specified
 * buffer's {@link #readerIndex() readerIndex} is increased together.
 * <p>
 * If there's not enough content left, {@link RangeException} is
 * raised.  The default value of newly allocated, wrapped or copied buffer's
 * {@link #readerIndex() readerIndex} is <tt>0</tt>.
 *
 * <pre>
 * // Iterates the readable bytes of a buffer.
 * {@link ChannelBuffer} buffer = ...;
 * while (buffer.readable()) {
 *     System.out.println(buffer.readByte());
 * }
 * </pre>
 *
 * <h4>Writable bytes</h4>
 *
 * This segment is a undefined space which needs to be filled.  Any operation
 * whose name ends with <tt>write</tt> will write the data at the current
 * {@link #writerIndex() writerIndex} and increase it by the number of written
 * bytes.  If the argument of the write operation is also a {@link ChannelBuffer},
 * and no source index is specified, the specified buffer's
 * {@link #readerIndex() readerIndex} is increased together.
 * <p>
 * If there's not enough writable bytes left, {@link RangeException}
 * is raised.  The default value of newly allocated buffer's
 * {@link #writerIndex() writerIndex} is <tt>0</tt>.  The default value of
 * wrapped or copied buffer's {@link #writerIndex() writerIndex} is the
 * {@link #capacity() capacity} of the buffer.
 *
 * <pre>
 * // Fills the writable bytes of a buffer with random integers.
 * {@link ChannelBuffer} buffer = ...;
 * while (buffer.writableBytes() >= 4) {
 *     buffer.writeInt(random.nextInt());
 * }
 * </pre>
 *
 * <h4>Discardable bytes</h4>
 *
 * This segment contains the bytes which were read already by a read operation.
 * Initially, the size of this segment is <tt>0</tt>, but its size increases up
 * to the {@link #writerIndex() writerIndex} as read operations are executed.
 * The read bytes can be discarded by calling {@link #discardReadBytes()} to
 * reclaim unused area as depicted by the following diagram:
 *
 * <pre>
 *  BEFORE discardReadBytes()
 *
 *      +-------------------+------------------+------------------+
 *      | discardable bytes |  readable bytes  |  writable bytes  |
 *      +-------------------+------------------+------------------+
 *      |                   |                  |                  |
 *      0      <=      readerIndex   <=   writerIndex    <=    capacity
 *
 *
 *  AFTER discardReadBytes()
 *
 *      +------------------+--------------------------------------+
 *      |  readable bytes  |    writable bytes (got more space)   |
 *      +------------------+--------------------------------------+
 *      |                  |                                      |
 * readerIndex (0) <= writerIndex (decreased)        <=        capacity
 * </pre>
 *
 * Please note that there is no guarantee about the content of writable bytes
 * after calling {@link #discardReadBytes()}.  The writable bytes will not be
 * moved in most cases and could even be filled with completely different data
 * depending on the underlying buffer implementation.
 *
 * <h4>Clearing the buffer indexes</h4>
 *
 * You can set both {@link #readerIndex() readerIndex} and
 * {@link #writerIndex() writerIndex} to <tt>0</tt> by calling {@link #clear()}.
 * It does not clear the buffer content (e.g. filling with <tt>0</tt>) but just
 * clears the two pointers.  Please also note that the semantic of this
 * operation is different from {@link ByteBuffer#clear()}.
 *
 * <pre>
 *  BEFORE clear()
 *
 *      +-------------------+------------------+------------------+
 *      | discardable bytes |  readable bytes  |  writable bytes  |
 *      +-------------------+------------------+------------------+
 *      |                   |                  |                  |
 *      0      <=      readerIndex   <=   writerIndex    <=    capacity
 *
 *
 *  AFTER clear()
 *
 *      +---------------------------------------------------------+
 *      |             writable bytes (got more space)             |
 *      +---------------------------------------------------------+
 *      |                                                         |
 *      0 = readerIndex = writerIndex            <=            capacity
 * </pre>
 *
 * <h3>Search operations</h3>
 *
 * Various {@link #indexOf(int, int, byte)} methods help you locate an index of
 * a value which meets a certain criteria.  Complicated dynamic sequential
 * search can be done with {@link ChannelBufferIndexFinder} as well as simple
 * static single byte search.
 * <p>
 * If you are decoding variable length data such as NUL-terminated string, you
 * will find {@link #bytesBefore(byte)} also useful.
 *
 * <h3>Mark and reset</h3>
 *
 * There are two marker indexes in every buffer. One is for storing
 * {@link #readerIndex() readerIndex} and the other is for storing
 * {@link #writerIndex() writerIndex}.  You can always reposition one of the
 * two indexes by calling a reset method.  It works in a similar fashion to
 * the mark and reset methods in {@link InputStream} except that there's no
 * <tt>readlimit</tt>.
 *
 * <h3>Derived buffers</h3>
 *
 * You can create a view of an existing buffer by calling either
 * {@link #duplicate()}, {@link #slice()} or {@link #slice(int, int)}.
 * A derived buffer will have an independent {@link #readerIndex() readerIndex},
 * {@link #writerIndex() writerIndex} and marker indexes, while it shares
 * other internal data representation, just like a NIO buffer does.
 * <p>
 * In case a completely fresh copy of an existing buffer is required, please
 * call {@link #copy()} method instead.
 *
 * <h3>Conversion to existing JDK types</h3>
 *
 * <h4>Byte array</h4>
 *
 * If a {@link ChannelBuffer} is backed by a byte array (i.e. <tt>byte[]</tt>),
 * you can access it directly via the {@link #array()} method.  To determine
 * if a buffer is backed by a byte array, {@link #hasArray()} should be used.
 *
 * <h4>NIO Buffers</h4>
 *
 * Various {@link #toByteBuffer()} and {@link #toByteBuffers()} methods convert
 * a {@link ChannelBuffer} into one or more NIO buffers.  These methods avoid
 * buffer allocation and memory copy whenever possible, but there's no
 * guarantee that memory copy will not be involved.
 *
 * <h4>Strings</h4>
 *
 * Various {@link #toString(std::string)} methods convert a {@link ChannelBuffer}
 * into a {@link std::string}.  Please note that {@link #toString()} is not a
 * conversion method.
 *
 * <h4>I/O Streams</h4>
 *
 * Please refer to {@link ChannelBufferInputStream} and
 * {@link ChannelBufferOutputStream}.
 *
 *
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 *
 * @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
 *
 * @apiviz.landmark
 */

class ChannelBuffer : public cetty::util::ReferenceCounter<ChannelBuffer> {
public:
    virtual ~ChannelBuffer() {}

    /**
     * Returns the factory which creates a {@link ChannelBuffer} whose
     * type and default {@link ByteOrder} are same with this buffer.
     */
    virtual ChannelBufferFactory& factory() const = 0;

    /**
     * Returns the number of bytes (octets) this buffer can contain.
     */
    virtual int capacity() const = 0;

    /**
     * Returns the <a href="http://en.wikipedia.org/wiki/Endianness">endianness</a>
     * of this buffer.
     */
    virtual ByteOrder order() const = 0;

    /**
     * Returns the <tt>readerIndex</tt> of this buffer.
     */
    int readerIndex() const;

    /**
     * Sets the <tt>readerIndex</tt> of this buffer.
     *
     * @throws RangeException
     *         if the specified <tt>readerIndex</tt> is
     *            less than <tt>0</tt> or
     *            greater than <tt>this.writerIndex</tt>
     */
    void readerIndex(int readerIdx);

    /**
     * Offset the <tt>readerIndex</tt> of this buffer with the offset.
     * This method same with
     * <code>readerIndex(readerIndex() + offset);</code>
     *
     * @throws RangeException
     *         if the specified <tt>offset + readerIndex()</tt> is
     *            less than <tt>0</tt> or
     *            greater than <tt>writerIndex()</tt>
     */
    void offsetReaderIndex(int offset);

    /**
     * Returns the <tt>writerIndex</tt> of this buffer.
     */
    int writerIndex() const;

    /**
     * Sets the <tt>writerIndex</tt> of this buffer.
     *
     * @throws RangeException
     *         if the specified <tt>writerIdx</tt> is
     *            less than <tt>this.readerIndex</tt> or
     *            greater than <tt>this.capacity</tt>
     */
    void writerIndex(int writerIdx);

    /**
     * Offset the <tt>writerIndex</tt> of this buffer with the offset.
     * This method same with
     * <code>writerIndex(writerIndex() + offset);</code>
     *
     * @throws RangeException
     *         if the specified <tt>offset + writerIndex()</tt> is
     *            less than <tt>readerIndex()</tt> or
     *            greater than <tt>capacity()</tt>
     */
    void offsetWriterIndex(int offset);

    /**
     * Sets the <em>readerIndex</em> and <em>writerIndex</em> of this buffer
     * in one shot.  This method is useful when you have to worry about the
     * invocation order of {@link #readerIndex(int)} and {@link #writerIndex(int)}
     * methods.  For example, the following code will fail:
     *
     * <pre>
     * // Create a buffer whose readerIndex, writerIndex and capacity are
     * // 0, 0 and 8 respectively.
     * {@link ChannelBufferPtr} buf = {@link ChannelBuffers}::buffer(8);
     *
     * // RangeException is thrown because the specified
     * // readerIndex (2) cannot be greater than the current writerIndex (0).
     * buf->readerIndex(2);
     * buf->writerIndex(4);
     * </pre>
     *
     * The following code will also fail:
     *
     * <pre>
     * // Create a buffer whose readerIndex, writerIndex and capacity are
     * // 0, 8 and 8 respectively.
     * {@link ChannelBufferPtr} buf = {@link ChannelBuffers}::wrappedBuffer(new byte[8]);
     *
     * // readerIndex becomes 8.
     * buf->readLong();
     *
     * // RangeException is thrown because the specified
     * // writerIndex (4) cannot be less than the current readerIndex (8).
     * buf->writerIndex(4);
     * buf->readerIndex(2);
     * </pre>
     *
     * By contrast, {@link #setIndex(int, int)} guarantees that it never
     * throws an {@link RangeException} as long as the specified
     * indexes meet basic constraints, regardless what the current index
     * values of the buffer are:
     *
     * <pre>
     * // No matter what the current state of the buffer is, the following
     * // call always succeeds as long as the capacity of the buffer is not
     * // less than 4.
     * buf->setIndex(2, 4);
     * </pre>
     *
     * @throws RangeException
     *         if the specified <tt>readerIndex</tt> is less than 0,
     *         if the specified <tt>writerIndex</tt> is less than the specified
     *         <tt>readerIndex</tt> or if the specified <tt>writerIndex</tt> is
     *         greater than <tt>this.capacity</tt>
     */
    void setIndex(int readerIndex, int writerIndex);

    /**
     * Returns the number of readable bytes which is equal to
     * <tt>(this.writerIndex - this.readerIndex)</tt>.
     */
    int  readableBytes() const;


    virtual void readableBytes(Array* arry) = 0;

    /**
     * Returns the number of writable bytes which is equal to
     * <tt>(this.capacity - this.writerIndex)</tt>.
     */
    int writableBytes() const;

    /**
     * Returns the number of writable bytes which is equal to
     * <tt>(this.capacity - this.writerIndex)</tt>.
     */
    int aheadWritableBytes() const;

    virtual void writableBytes(Array* arry) = 0;

    virtual void aheadWritableBytes(Array* arry) = 0;

    /**
     * Returns <tt>true</tt> if and only if this buffer has a backing byte array.
     * If this method returns true, you can safely call {@link #array()} and
     * {@link #arrayOffset()}.
     */
    virtual bool hasArray() const = 0;

    /**
     * Returns <tt>true</tt>
     * if and only if <tt>(this.writerIndex - this.readerIndex)</tt> is greater
     * than <tt>0</tt>.
     */
    bool readable() const;

    /**
     * Returns <tt>true</tt>
     * if and only if <tt>(this.capacity - this.writerIndex)</tt> is greater
     * than <tt>0</tt>.
     */
    bool writable() const;

    /**
     * Sets the <tt>readerIndex</tt> and <tt>writerIndex</tt> of this buffer to
     * <tt>0</tt>.
     * This method is identical to {@link #setIndex(int, int) setIndex(0, 0)}.
     */
    void clear();

    /**
     * Marks the current <tt>readerIndex</tt> in this buffer.  You can
     * reposition the current <tt>readerIndex</tt> to the marked
     * <tt>readerIndex</tt> by calling {@link #resetReaderIndex()}.
     * The initial value of the marked <tt>readerIndex</tt> is <tt>0</tt>.
     */
    void markReaderIndex();

    /**
     * Repositions the current <tt>readerIndex</tt> to the marked
     * <tt>readerIndex</tt> in this buffer.
     *
     * @throws RangeException
     *         if the current <tt>writerIndex</tt> is less than the marked
     *         <tt>readerIndex</tt>
     */
    void resetReaderIndex();

    /**
     * Marks the current <tt>writerIndex</tt> in this buffer.  You can
     * reposition the current <tt>writerIndex</tt> to the marked
     * <tt>writerIndex</tt> by calling {@link #resetWriterIndex()}.
     * The initial value of the marked <tt>writerIndex</tt> is <tt>0</tt>.
     */
    void markWriterIndex();

    /**
     * Repositions the current <tt>writerIndex</tt> to the marked
     * <tt>writerIndex</tt> in this buffer.
     *
     * @throws RangeException
     *         if the current <tt>readerIndex</tt> is greater than the marked
     *         <tt>writerIndex</tt>
     */
    void resetWriterIndex();

    /**
     * Discards the bytes between the 0th index and <tt>readerIndex</tt>.
     * It moves the bytes between <tt>readerIndex</tt> and <tt>writerIndex</tt>
     * to the 0th index, and sets <tt>readerIndex</tt> and <tt>writerIndex</tt>
     * to <tt>0</tt> and <tt>oldWriterIndex - oldReaderIndex</tt> respectively.
     * <p>
     * Please refer to the class documentation for more detailed explanation.
     */
    virtual void discardReadBytes();

    /**
     * Makes sure the number of {@link #writableBytes() the writable bytes}
     * is equal to or greater than the specified value.  If there is enough
     * writable bytes in this buffer, this method returns with no side effect.
     * Otherwise:
     * <ul>
     * <li>a non-dynamic buffer will throw an {@link RangeException}.</li>
     * <li>a dynamic buffer will expand its capacity so that the number of the
     *     {@link #writableBytes() writable bytes} becomes equal to or greater
     *     than the specified value. The expansion involves the reallocation of
     *     the internal buffer and consequently memory copy.</li>
     * </ul>
     *
     * @param writableBytes
     *        the expected minimum number of writable bytes
     * @throws RangeException
     *         if @link #writableBytes() the writable bytes@endlink of this
     *         buffer is less than the specified value and if this buffer is
     *         not a dynamic buffer
     */
    virtual void ensureWritableBytes(int writableBytes);

    /**
     * Gets a byte at the specified absolute <tt>index</tt> in this buffer.
     * This method does not modify <tt>readerIndex</tt> or <tt>writerIndex</tt> of
     * this buffer.
     *
     * @throws RangeException
     *         if the specified <tt>index</tt> is less than <tt>0</tt> or
     *         <tt>index + 1</tt> is greater than <tt>this.capacity</tt>
     */
    virtual boost::int8_t getByte(int index) const = 0;

    /**
     * Gets an unsigned byte at the specified absolute <tt>index</tt> in this
     * buffer.  This method does not modify <tt>readerIndex</tt> or
     * <tt>writerIndex</tt> of this buffer.
     *
     * @throws RangeException
     *         if the specified <tt>index</tt> is less than <tt>0</tt> or
     *         <tt>index + 1</tt> is greater than <tt>this.capacity</tt>
     */
    boost::uint8_t getUnsignedByte(int index) const;

    /**
     * Gets a 16-bit short integer at the specified absolute <tt>index</tt> in
     * this buffer.  This method does not modify <tt>readerIndex</tt> or
     * <tt>writerIndex</tt> of this buffer.
     *
     * @throws RangeException
     *         if the specified <tt>index</tt> is less than <tt>0</tt> or
     *         <tt>index + 2</tt> is greater than <tt>this.capacity</tt>
     */
    virtual boost::int16_t getShort(int index) const = 0;

    /**
     * Gets an unsigned 16-bit short integer at the specified absolute
     * <tt>index</tt> in this buffer.  This method does not modify
     * <tt>readerIndex</tt> or <tt>writerIndex</tt> of this buffer.
     *
     * @throws RangeException
     *         if the specified <tt>index</tt> is less than <tt>0</tt> or
     *         <tt>index + 2</tt> is greater than <tt>this.capacity</tt>
     */
    boost::uint16_t getUnsignedShort(int index) const;

    /**
     * Gets a 32-bit integer at the specified absolute <tt>index</tt> in
     * this buffer.  This method does not modify <tt>readerIndex</tt> or
     * <tt>writerIndex</tt> of this buffer.
     *
     * @throws RangeException
     *         if the specified <tt>index</tt> is less than <tt>0</tt> or
     *         <tt>index + 4</tt> is greater than <tt>this.capacity</tt>
     */
    virtual boost::int32_t  getInt(int index) const = 0;

    /**
     * Gets an unsigned 32-bit integer at the specified absolute <tt>index</tt>
     * in this buffer.  This method does not modify <tt>readerIndex</tt> or
     * <tt>writerIndex</tt> of this buffer.
     *
     * @throws RangeException
     *         if the specified <tt>index</tt> is less than <tt>0</tt> or
     *         <tt>index + 4</tt> is greater than <tt>this.capacity</tt>
     */
    boost::uint32_t getUnsignedInt(int index) const;

    /**
     * Gets a 64-bit long integer at the specified absolute <tt>index</tt> in
     * this buffer.  This method does not modify <tt>readerIndex</tt> or
     * <tt>writerIndex</tt> of this buffer.
     *
     * @throws RangeException
     *         if the specified <tt>index</tt> is less than <tt>0</tt> or
     *         <tt>index + 8</tt> is greater than <tt>this.capacity</tt>
     */
    virtual boost::int64_t getLong(int index) const = 0;

    /**
     * Gets a 32-bit floating point number at the specified absolute
     * <tt>index</tt> in this buffer.  This method does not modify
     * <tt>readerIndex</tt> or <tt>writerIndex</tt> of this buffer.
     *
     * @throws RangeException
     *         if the specified <tt>index</tt> is less than <tt>0</tt> or
     *         <tt>index + 4</tt> is greater than <tt>this.capacity</tt>
     */
    float getFloat(int index) const;

    /**
     * Gets a 64-bit floating point number at the specified absolute
     * <tt>index</tt> in this buffer.  This method does not modify
     * <tt>readerIndex</tt> or <tt>writerIndex</tt> of this buffer.
     *
     * @throws RangeException
     *         if the specified <tt>index</tt> is less than <tt>0</tt> or
     *         <tt>index + 8</tt> is greater than <tt>this.capacity</tt>
     */
    double getDouble(int index) const;

    /**
     * Transfers this buffer's data to the specified destination starting at
     * the specified absolute <tt>index</tt> until the destination becomes
     * non-writable.  This method is basically same with
     * {@link #getBytes(int, ChannelBuffer, int, int)}, except that this
     * method increases the <tt>writerIndex</tt> of the destination by the
     * number of the transferred bytes while
     * {@link #getBytes(int, ChannelBuffer, int, int)} does not.
     * This method does not modify <tt>readerIndex</tt> or <tt>writerIndex</tt> of
     * the source buffer (i.e. <tt>this</tt>).
     *
     * @throws RangeException
     *         if the specified <tt>index</tt> is less than <tt>0</tt> or
     *         if <tt>index + dst.writableBytes</tt> is greater than
     *            <tt>this.capacity</tt>
     */
    int getBytes(int index, const ChannelBufferPtr& dst) const;

    /**
     * Transfers this buffer's data to the specified destination starting at
     * the specified absolute <tt>index</tt>.  This method is basically same
     * with {@link #getBytes(int, ChannelBuffer, int, int)}, except that this
     * method increases the <tt>writerIndex</tt> of the destination by the
     * number of the transferred bytes while
     * {@link #getBytes(int, ChannelBuffer, int, int)} does not.
     * This method does not modify <tt>readerIndex</tt> or <tt>writerIndex</tt> of
     * the source buffer (i.e. <tt>this</tt>).
     *
     * @param length the number of bytes to transfer
     *
     * @throws RangeException
     *         if the specified <tt>index</tt> is less than <tt>0</tt>,
     *         if <tt>index + length</tt> is greater than
     *            <tt>this.capacity</tt>, or
     *         if <tt>length</tt> is greater than <tt>dst.writableBytes</tt>
     */
    int getBytes(int index, const ChannelBufferPtr& dst, int length) const;

    /**
     * Transfers this buffer's data to the specified destination starting at
     * the specified absolute <tt>index</tt>.
     * This method does not modify <tt>readerIndex</tt> or <tt>writerIndex</tt>
     * of both the source (i.e. <tt>this</tt>) and the destination.
     *
     * @param dstIndex the first index of the destination
     * @param length   the number of bytes to transfer
     *
     * @throws RangeException
     *         if the specified <tt>index</tt> is less than <tt>0</tt>,
     *         if the specified <tt>dstIndex</tt> is less than <tt>0</tt>,
     *         if <tt>index + length</tt> is greater than
     *            <tt>this.capacity</tt>, or
     *         if <tt>dstIndex + length</tt> is greater than
     *            <tt>dst.capacity</tt>
     */
    virtual int getBytes(int index, const ChannelBufferPtr& dst, int dstIndex, int length) const = 0;

    /**
     * Transfers this buffer's data to the specified destination starting at
     * the specified absolute <tt>index</tt>.
     * This method does not modify <tt>readerIndex</tt> or <tt>writerIndex</tt> of
     * this buffer
     *
     * @throws RangeException
     *         if the specified <tt>index</tt> is less than <tt>0</tt> or
     *         if <tt>index + dst.length()</tt> is greater than
     *            <tt>this.capacity</tt>
     */
    int getBytes(int index, Array* dst) const;

    /**
     * Transfers this buffer's data to the specified destination starting at
     * the specified absolute <tt>index</tt>.
     * This method does not modify <tt>readerIndex</tt> or <tt>writerIndex</tt>
     * of this buffer.
     *
     * @param dstIndex the first index of the destination
     * @param length   the number of bytes to transfer
     *
     * @throws RangeException
               如果dst的容量不足，必须抛出异常
     *         if the specified <tt>index</tt> is less than <tt>0</tt>,
     *         if the specified <tt>dstIndex</tt> is less than <tt>0</tt>,
     *         if <tt>index + length</tt> is greater than
     *            <tt>this.capacity</tt>, or
     *         if <tt>dstIndex + length</tt> is greater than
     *            <tt>dst.length()</tt>
     */
    virtual int getBytes(int index, Array* dst, int dstIndex, int length) const = 0;

    /**
     * Transfers this buffer's data to the specified destination starting at
     * the specified absolute <tt>index</tt>.
     * This method does not modify <tt>readerIndex</tt> or <tt>writerIndex</tt> of
     * this buffer
     *
     * @param length   the number of bytes to transfer
     *
     * @throws RangeException
     *         if the specified <tt>index</tt> is less than <tt>0</tt> or
     *         if <tt>index + length</tt> is greater than
     *            <tt>this->capacity</tt>
     */
    int getBytes(int index, std::string* dst, int length) const;

    /**
     * Transfers this buffer's data to the specified destination starting at
     * the specified absolute <tt>index</tt>.
     * This method does not modify <tt>readerIndex</tt> or <tt>writerIndex</tt>
     * of this buffer.
     *
     * @param dstIndex the first index of the destination
     * @param length   the number of bytes to transfer
     *
     * @throws RangeException
     *         if the specified <tt>index</tt> is less than <tt>0</tt>,
     *         if the specified <tt>dstIndex</tt> is less than <tt>0</tt>,
     *         if <tt>index + length</tt> is greater than
     *            <tt>this->capacity</tt>
     */
    int getBytes(int index, std::string* dst, int dstIndex, int length) const;

    /**
     * Transfers this buffer's data to the specified stream starting at the
     * specified absolute <tt>index</tt>.
     * This method does not modify <tt>readerIndex</tt> or <tt>writerIndex</tt> of
     * this buffer.
     *
     * @param length the number of bytes to transfer
     *
     * @throws RangeException
     *         if the specified <tt>index</tt> is less than <tt>0</tt> or
     *         if <tt>index + length</tt> is greater than
     *            <tt>this.capacity</tt>
     * @throws IOException
     *         if the specified stream threw an exception during I/O
     */
    virtual int getBytes(int index, OutputStream* out, int length) const = 0;

    /**
     * Returns a copy of this buffer's readable bytes.  Modifying the content
     * of the returned buffer or this buffer does not affect each other at all.
     * This method is identical to <tt>buf.copy(buf.readerIndex(), buf.readableBytes())</tt>.
     * This method does not modify <tt>readerIndex</tt> or <tt>writerIndex</tt> of
     * this buffer.
     */
    ChannelBufferPtr copy() const;

    /**
     * Returns a copy of this buffer's sub-region.  Modifying the content of
     * the returned buffer or this buffer does not affect each other at all.
     * This method does not modify <tt>readerIndex</tt> or <tt>writerIndex</tt> of
     * this buffer.
     */
    virtual ChannelBufferPtr copy(int index, int length) const = 0;

    /**
     * Returns a slice of this buffer's readable bytes. Modifying the content
     * of the returned buffer or this buffer affects each other's content
     * while they maintain separate indexes and marks.  This method is
     * identical to <tt>buf.slice(buf.readerIndex(), buf.readableBytes())</tt>.
     * This method does not modify <tt>readerIndex</tt> or <tt>writerIndex</tt> of
     * this buffer.
     */
    ChannelBufferPtr slice();

    /**
     * Returns a slice of this buffer's sub-region. Modifying the content of
     * the returned buffer or this buffer affects each other's content while
     * they maintain separate indexes and marks.
     * This method does not modify <tt>readerIndex</tt> or <tt>writerIndex</tt> of
     * this buffer.
     */
    virtual ChannelBufferPtr slice(int index, int length) = 0;


    int slice(GatheringBuffer* gatheringBuffer);

    /**
     * Returns a slice of this buffer's sub-region. Modifying the content of
     * the returned buffer or this buffer affects each other's content while
     * they maintain separate indexes and marks.
     * This method does not modify <tt>readerIndex</tt> or <tt>writerIndex</tt> of
     * this buffer.
     */
    virtual int slice(int index, int length, GatheringBuffer* gatheringBuffer) = 0;

    /**
     * Returns a buffer which shares the whole region of this buffer.
     * Modifying the content of the returned buffer or this buffer affects
     * each other's content while they maintain separate indexes and marks.
     * This method is identical to <tt>buf.slice(0, buf.capacity())</tt>.
     * This method does not modify <tt>readerIndex</tt> or <tt>writerIndex</tt> of
     * this buffer.
     */
    ChannelBufferPtr duplicate();

    /**
     * Sets the specified byte at the specified absolute <tt>index</tt> in this
     * buffer.  The 24 high-order bits of the specified value are ignored.
     * This method does not modify <tt>readerIndex</tt> or <tt>writerIndex</tt> of
     * this buffer.
     *
     * @throws RangeException
     *         if the specified <tt>index</tt> is less than <tt>0</tt> or
     *         <tt>index + 1</tt> is greater than <tt>this.capacity</tt>
     */
    virtual int setByte(int index, int value) = 0;

    /**
     * Sets the specified 16-bit short integer at the specified absolute
     * <tt>index</tt> in this buffer.  The 16 high-order bits of the specified
     * value are ignored.
     * This method does not modify <tt>readerIndex</tt> or <tt>writerIndex</tt> of
     * this buffer.
     *
     * @throws RangeException
     *         if the specified <tt>index</tt> is less than <tt>0</tt> or
     *         <tt>index + 2</tt> is greater than <tt>this.capacity</tt>
     */
    virtual int setShort(int index, int value) = 0;

    /**
     * Sets the specified 32-bit integer at the specified absolute
     * <tt>index</tt> in this buffer.
     * This method does not modify <tt>readerIndex</tt> or <tt>writerIndex</tt> of
     * this buffer.
     *
     * @throws RangeException
     *         if the specified <tt>index</tt> is less than <tt>0</tt> or
     *         <tt>index + 4</tt> is greater than <tt>this.capacity</tt>
     */
    virtual int setInt(int index, int value) = 0;

    /**
     * Sets the specified 64-bit long integer at the specified absolute
     * <tt>index</tt> in this buffer.
     * This method does not modify <tt>readerIndex</tt> or <tt>writerIndex</tt> of
     * this buffer.
     *
     * @throws RangeException
     *         if the specified <tt>index</tt> is less than <tt>0</tt> or
     *         <tt>index + 8</tt> is greater than <tt>this.capacity</tt>
     */
    virtual int setLong(int index, boost::int64_t value) = 0;

    /**
     * Sets the specified 32-bit floating-point number at the specified
     * absolute <tt>index</tt> in this buffer.
     * This method does not modify <tt>readerIndex</tt> or <tt>writerIndex</tt> of
     * this buffer.
     *
     * @throws RangeException
     *         if the specified <tt>index</tt> is less than <tt>0</tt> or
     *         <tt>index + 4</tt> is greater than <tt>this.capacity</tt>
     */
    int setFloat(int index, float value);

    /**
     * Sets the specified 64-bit floating-point number at the specified
     * absolute <tt>index</tt> in this buffer.
     * This method does not modify <tt>readerIndex</tt> or <tt>writerIndex</tt> of
     * this buffer.
     *
     * @throws RangeException
     *         if the specified <tt>index</tt> is less than <tt>0</tt> or
     *         <tt>index + 8</tt> is greater than <tt>this.capacity</tt>
     */
    int setDouble(int index, double value);

    /**
     * Transfers the specified source buffer's data to this buffer starting at
     * the specified absolute <tt>index</tt> until the source buffer becomes
     * unreadable.  This method is basically same with
     * {@link #setBytes(int, ChannelBuffer, int, int)}, except that this
     * method increases the <tt>readerIndex</tt> of the source buffer by
     * the number of the transferred bytes while
     * {@link #setBytes(int, ChannelBuffer, int, int)} does not.
     * This method does not modify <tt>readerIndex</tt> or <tt>writerIndex</tt> of
     * the source buffer (i.e. <tt>this</tt>).
     *
     * @throws RangeException
     *         if the specified <tt>index</tt> is less than <tt>0</tt> or
     *         if <tt>index + src.readableBytes</tt> is greater than
     *            <tt>this.capacity</tt>
     */
    int setBytes(int index, const ChannelBufferPtr& src);

    /**
     * Transfers the specified source buffer's data to this buffer starting at
     * the specified absolute <tt>index</tt>.  This method is basically same
     * with {@link #setBytes(int, ChannelBuffer, int, int)}, except that this
     * method increases the <tt>readerIndex</tt> of the source buffer by
     * the number of the transferred bytes while
     * {@link #setBytes(int, ChannelBuffer, int, int)} does not.
     * This method does not modify <tt>readerIndex</tt> or <tt>writerIndex</tt> of
     * the source buffer (i.e. <tt>this</tt>).
     *
     * @param length the number of bytes to transfer
     *
     * @throws RangeException
     *         if the specified <tt>index</tt> is less than <tt>0</tt>,
     *         if <tt>index + length</tt> is greater than
     *            <tt>this.capacity</tt>, or
     *         if <tt>length</tt> is greater than <tt>src.readableBytes</tt>
     */
    int setBytes(int index, const ChannelBufferPtr& src, int length);

    /**
     * Transfers the specified source buffer's data to this buffer starting at
     * the specified absolute <tt>index</tt>.
     * This method does not modify <tt>readerIndex</tt> or <tt>writerIndex</tt>
     * of both the source (i.e. <tt>this</tt>) and the destination.
     *
     * @param srcIndex the first index of the source
     * @param length   the number of bytes to transfer
     *
     * @throws RangeException
     *         if the specified <tt>index</tt> is less than <tt>0</tt>,
     *         if the specified <tt>srcIndex</tt> is less than <tt>0</tt>,
     *         if <tt>index + length</tt> is greater than
     *            <tt>this.capacity</tt>, or
     *         if <tt>srcIndex + length</tt> is greater than
     *            <tt>src.capacity</tt>
     */
    virtual int setBytes(int index, const ConstChannelBufferPtr& src, int srcIndex, int length) = 0;

    /**
     * Transfers the specified source array's data to this buffer starting at
     * the specified absolute <tt>index</tt>.
     * This method does not modify <tt>readerIndex</tt> or <tt>writerIndex</tt> of
     * this buffer.
     *
     * @throws RangeException
     *         if the specified <tt>index</tt> is less than <tt>0</tt> or
     *         if <tt>index + src.length()</tt> is greater than
     *            <tt>this.capacity</tt>
     */
    int setBytes(int index, const ConstArray& src);

    /**
     * Transfers the specified source array's data to this buffer starting at
     * the specified absolute <tt>index</tt>.
     * This method does not modify <tt>readerIndex</tt> or <tt>writerIndex</tt> of
     * this buffer.
     *
     * @throws RangeException
     *         if the specified <tt>index</tt> is less than <tt>0</tt>,
     *         if the specified <tt>srcIndex</tt> is less than <tt>0</tt>,
     *         if <tt>index + length</tt> is greater than
     *            <tt>this.capacity</tt>, or
     *         if <tt>srcIndex + length</tt> is greater than <tt>src.length()</tt>
     */
    virtual int setBytes(int index, const ConstArray& src, int srcIndex, int length) = 0;

    /**
     * Transfers the specified source array's data to this buffer starting at
     * the specified absolute <tt>index</tt>.
     * This method does not modify <tt>readerIndex</tt> or <tt>writerIndex</tt> of
     * this buffer.
     *
     * @throws RangeException
     *         if the specified <tt>index</tt> is less than <tt>0</tt> or
     *         if <tt>index + src.length()()</tt> is greater than
     *            <tt>this.capacity</tt>
     */
    int setBytes(int index, const std::string& src);

    /**
     * Transfers the specified source array's data to this buffer starting at
     * the specified absolute <tt>index</tt>.
     * This method does not modify <tt>readerIndex</tt> or <tt>writerIndex</tt> of
     * this buffer.
     *
     * @throws RangeException
     *         if the specified <tt>index</tt> is less than <tt>0</tt>,
     *         if the specified <tt>srcIndex</tt> is less than <tt>0</tt>,
     *         if <tt>index + length</tt> is greater than
     *            <tt>this.capacity</tt>, or
     *         if <tt>srcIndex + length</tt> is greater than <tt>src.length()</tt>
     */
    int setBytes(int index, const std::string& src, int srcIndex, int length);

    /**
     * Transfers the content of the specified source stream to this buffer
     * starting at the specified absolute <tt>index</tt>.
     * This method does not modify <tt>readerIndex</tt> or <tt>writerIndex</tt> of
     * this buffer.
     *
     * @param length the number of bytes to transfer
     *
     * @return the actual number of bytes read in from the specified channel.
     *         <tt>-1</tt> if the specified channel is closed.
     *
     * @throws RangeException
     *         if the specified <tt>index</tt> is less than <tt>0</tt> or
     *         if <tt>index + length</tt> is greater than <tt>this.capacity</tt>
     * @throws IOException
     *         if the specified stream threw an exception during I/O
     */
    virtual int setBytes(int index, InputStream* in, int length) = 0;

    /**
     * Fills this buffer with <tt>NUL (0x00)</tt> starting at the specified
     * absolute <tt>index</tt>.
     * This method does not modify <tt>readerIndex</tt> or <tt>writerIndex</tt> of
     * this buffer.
     *
     * @param length the number of <tt>NUL</tt>s to write to the buffer
     *
     * @throws RangeException
     *         if the specified <tt>index</tt> is less than <tt>0</tt> or
     *         if <tt>index + length</tt> is greater than <tt>this.capacity</tt>
     */
    int setZero(int index, int length);

    /**
     * Gets a byte at the current <tt>readerIndex</tt> and increases
     * the <tt>readerIndex</tt> by <tt>1</tt> in this buffer.
     *
     * @throws RangeException
     *         if <tt>this.readableBytes</tt> is less than <tt>1</tt>
     */
    boost::int8_t readByte();

    /**
     * Gets an unsigned byte at the current <tt>readerIndex</tt> and increases
     * the <tt>readerIndex</tt> by <tt>1</tt> in this buffer.
     *
     * @throws RangeException
     *         if <tt>this.readableBytes</tt> is less than <tt>1</tt>
     */
    boost::uint8_t readUnsignedByte();

    /**
     * Gets a 16-bit short integer at the current <tt>readerIndex</tt>
     * and increases the <tt>readerIndex</tt> by <tt>2</tt> in this buffer.
     *
     * @throws RangeException
     *         if <tt>this.readableBytes</tt> is less than <tt>2</tt>
     */
    boost::int16_t readShort();

    /**
     * Gets an unsigned 16-bit short integer at the current <tt>readerIndex</tt>
     * and increases the <tt>readerIndex</tt> by <tt>2</tt> in this buffer.
     *
     * @throws RangeException
     *         if <tt>this.readableBytes</tt> is less than <tt>2</tt>
     */
    boost::uint16_t readUnsignedShort();

    /**
     * Gets a 32-bit integer at the current <tt>readerIndex</tt>
     * and increases the <tt>readerIndex</tt> by <tt>4</tt> in this buffer.
     *
     * @throws RangeException
     *         if <tt>this.readableBytes</tt> is less than <tt>4</tt>
     */
    boost::int32_t readInt();

    /**
     * Gets an unsigned 32-bit integer at the current <tt>readerIndex</tt>
     * and increases the <tt>readerIndex</tt> by <tt>4</tt> in this buffer.
     *
     * @throws RangeException
     *         if <tt>this.readableBytes</tt> is less than <tt>4</tt>
     */
    boost::uint32_t readUnsignedInt();

    /**
     * Gets a 64-bit integer at the current <tt>readerIndex</tt>
     * and increases the <tt>readerIndex</tt> by <tt>8</tt> in this buffer.
     *
     * @throws RangeException
     *         if <tt>this.readableBytes</tt> is less than <tt>8</tt>
     */
    boost::int64_t readLong();

    /**
     * Gets a 32-bit floating point number at the current <tt>readerIndex</tt>
     * and increases the <tt>readerIndex</tt> by <tt>4</tt> in this buffer.
     *
     * @throws RangeException
     *         if <tt>this.readableBytes</tt> is less than <tt>4</tt>
     */
    float readFloat();

    /**
     * Gets a 64-bit floating point number at the current <tt>readerIndex</tt>
     * and increases the <tt>readerIndex</tt> by <tt>8</tt> in this buffer.
     *
     * @throws RangeException
     *         if <tt>this.readableBytes</tt> is less than <tt>8</tt>
     */
    double readDouble();

    /**
     * Transfers this buffer's data to a newly created buffer starting at
     * the current <tt>readerIndex</tt> and increases the <tt>readerIndex</tt>
     * by the number of the transferred bytes (= <tt>length</tt>).
     * The returned buffer's <tt>readerIndex</tt> and <tt>writerIndex</tt> are
     * <tt>0</tt> and <tt>length</tt> respectively.
     *
     * @param length the number of bytes to transfer
     *
     * @return the newly created buffer which contains the transferred bytes
     *
     * @throws RangeException
     *         if <tt>length</tt> is greater than <tt>this.readableBytes</tt>
     */
    ChannelBufferPtr readBytes();

    /**
     * Transfers this buffer's data to a newly created buffer starting at
     * the current <tt>readerIndex</tt> and increases the <tt>readerIndex</tt>
     * by the number of the transferred bytes (= <tt>length</tt>).
     * The returned buffer's <tt>readerIndex</tt> and <tt>writerIndex</tt> are
     * <tt>0</tt> and <tt>length</tt> respectively.
     *
     * @param length the number of bytes to transfer
     *
     * @return the newly created buffer which contains the transferred bytes
     *
     * @throws RangeException
     *         if <tt>length</tt> is greater than <tt>this.readableBytes</tt>
     */
    ChannelBufferPtr readBytes(int length);

    /**
     * Transfers this buffer's data to the specified destination starting at
     * the current <tt>readerIndex</tt> until the destination becomes
     * non-writable, and increases the <tt>readerIndex</tt> by the number of the
     * transferred bytes.  This method is basically same with
     * {@link #readBytes(ChannelBuffer, int, int)}, except that this method
     * increases the <tt>writerIndex</tt> of the destination by the number of
     * the transferred bytes while {@link #readBytes(ChannelBuffer, int, int)}
     * does not.
     *
     * @throws RangeException
     *         if <tt>dst.writableBytes</tt> is greater than
     *            <tt>this.readableBytes</tt>
     */
    int readBytes(const ChannelBufferPtr& dst);

    /**
     * Transfers this buffer's data to the specified destination starting at
     * the current <tt>readerIndex</tt> and increases the <tt>readerIndex</tt>
     * by the number of the transferred bytes (= <tt>length</tt>).  This method
     * is basically same with {@link #readBytes(ChannelBuffer, int, int)},
     * except that this method increases the <tt>writerIndex</tt> of the
     * destination by the number of the transferred bytes (= <tt>length</tt>)
     * while {@link #readBytes(ChannelBuffer, int, int)} does not.
     *
     * @throws RangeException
     *         if <tt>length</tt> is greater than <tt>this.readableBytes</tt> or
     *         if <tt>length</tt> is greater than <tt>dst.writableBytes</tt>
     */
    int readBytes(const ChannelBufferPtr& dst, int length);

    /**
     * Transfers this buffer's data to the specified destination starting at
     * the current <tt>readerIndex</tt> and increases the <tt>readerIndex</tt>
     * by the number of the transferred bytes (= <tt>length</tt>).
     *
     * @param dstIndex the first index of the destination
     * @param length   the number of bytes to transfer
     *
     * @throws RangeException
     *         if the specified <tt>dstIndex</tt> is less than <tt>0</tt>,
     *         if <tt>length</tt> is greater than <tt>this.readableBytes</tt>, or
     *         if <tt>dstIndex + length</tt> is greater than
     *            <tt>dst.capacity</tt>
     */
    int readBytes(const ChannelBufferPtr& dst, int dstIndex, int length);

    /**
     * Transfers this buffer's data to the specified destination starting at
     * the current <tt>readerIndex</tt> and increases the <tt>readerIndex</tt>
     * by the number of the transferred bytes (= <tt>dst.length()</tt>).
     *
     * @throws RangeException
     *         if <tt>dst.length()</tt> is greater than <tt>this.readableBytes</tt>
     */
    int readBytes(Array* dst);

    /**
     * Transfers this buffer's data to the specified destination starting at
     * the current <tt>readerIndex</tt> and increases the <tt>readerIndex</tt>
     * by the number of the transferred bytes (= <tt>length</tt>).
     *
     * @param dstIndex the first index of the destination
     * @param length   the number of bytes to transfer
     *
     * @throws RangeException
     *         if the specified <tt>dstIndex</tt> is less than <tt>0</tt>,
     *         if <tt>length</tt> is greater than <tt>this.readableBytes</tt>, or
     *         if <tt>dstIndex + length</tt> is greater than <tt>dst.length()</tt>
     */
    int readBytes(Array* dst, int dstIndex, int length);

    /**
     * Transfers this buffer's data to the specified destination starting at
     * the current <tt>readerIndex</tt> and increases the <tt>readerIndex</tt>
     * by the number of the readable bytes (= <tt>readableBytes()</tt>).
     *
     */
    int readBytes(std::string* dst);

    /**
     * Transfers this buffer's data to the specified destination starting at
     * the current <tt>readerIndex</tt> and increases the <tt>readerIndex</tt>
     * by the number of the transferred bytes (= <tt>dst.length()</tt>).
     *
     * @param length   the number of bytes to transfer
     *
     * @throws RangeException
     *         if <tt>length</tt> is greater than <tt>this.readableBytes</tt>
     */
    int readBytes(std::string* dst, int length);

    /**
     * Transfers this buffer's data to the specified destination starting at
     * the current <tt>readerIndex</tt> and increases the <tt>readerIndex</tt>
     * by the number of the transferred bytes (= <tt>length</tt>).
     *
     * @param dstIndex the first index of the destination
     * @param length   the number of bytes to transfer
     *
     * @throws RangeException
     *         if the specified <tt>dstIndex</tt> is less than <tt>0</tt>,
     *         if <tt>length</tt> is greater than <tt>this.readableBytes</tt>, or
     *         if <tt>dstIndex + length</tt> is greater than <tt>dst.length()</tt>
     */
    int readBytes(std::string* dst, int dstIndex, int length);

    /**
     * Transfers this buffer's data to the specified stream starting at the
     * current <tt>readerIndex</tt>.
     *
     * @param length the number of bytes to transfer
     *
     * @throws RangeException
     *         if <tt>length</tt> is greater than <tt>this.readableBytes</tt>
     * @throws IOException
     *         if the specified stream threw an exception during I/O
     */
    int readBytes(OutputStream* out, int length);

    /**
     * Returns a new slice of this buffer's sub-region starting at the current
     * <tt>readerIndex</tt> and increases the <tt>readerIndex</tt> by the
     * {@link #readableBytes() readableBytes}. This method is basically same with
     * {@link #readSlice(int) readSlice(readableBytes())}.
     *
     * @return the newly created slice
     */
    ChannelBufferPtr readSlice();

    /**
     * Returns a new slice of this buffer's sub-region starting at the current
     * <tt>readerIndex</tt> and increases the <tt>readerIndex</tt> by the size
     * of the new slice (= <tt>length</tt>).
     *
     * @param length the size of the new slice
     *
     * @return the newly created slice
     *
     * @throws RangeException
     *         if <tt>length</tt> is greater than <tt>this.readableBytes</tt>
     */
    ChannelBufferPtr readSlice(int length);

    /**
     * Assign a new slice of this buffer's sub-region starting at the current
     * <tt>readerIndex</tt> and increases the <tt>readerIndex</tt> by the
     * {@link #readableBytes() readableBytes} to a GatheringBuffer.
     */
    int readSlice(GatheringBuffer* gatheringBuffer);


    int readSlice(GatheringBuffer* gatheringBuffer, int length);

    /**
     * Increases the current <tt>readerIndex</tt> by the specified
     * <tt>length</tt> in this buffer.
     *
     * @throws RangeException
     *         if <tt>length</tt> is greater than <tt>this.readableBytes</tt>
     */
    int skipBytes(int length);

    /**
     * Sets the specified byte at the current <tt>writerIndex</tt>
     * and increases the <tt>writerIndex</tt> by <tt>1</tt> in this buffer.
     * The 24 high-order bits of the specified value are ignored.
     *
     * @throws RangeException
     *         if <tt>this.writableBytes</tt> is less than <tt>1</tt>
     */
    int writeByte(int value);

    int writeByteAhead(int value);

    /**
     * Sets the specified 16-bit short integer at the current
     * <tt>writerIndex</tt> and increases the <tt>writerIndex</tt> by <tt>2</tt>
     * in this buffer.  The 16 high-order bits of the specified value are ignored.
     *
     * @throws RangeException
     *         if <tt>this.writableBytes</tt> is less than <tt>2</tt>
     */
    int writeShort(int value);

    int writeShortAhead(int value);

    /**
     * Sets the specified 32-bit integer at the current <tt>writerIndex</tt>
     * and increases the <tt>writerIndex</tt> by <tt>4</tt> in this buffer.
     *
     * @throws RangeException
     *         if <tt>this.writableBytes</tt> is less than <tt>4</tt>
     */
    int writeInt(int value);

    int writeIntAhead(int value);

    /**
     * Sets the specified 64-bit long integer at the current
     * <tt>writerIndex</tt> and increases the <tt>writerIndex</tt> by <tt>8</tt>
     * in this buffer.
     *
     * @throws RangeException
     *         if <tt>this.writableBytes</tt> is less than <tt>8</tt>
     */
    int writeLong(boost::int64_t value);

    int writeLongAhead(boost::int64_t value);

    /**
     * Sets the specified 32-bit floating point number at the current
     * <tt>writerIndex</tt> and increases the <tt>writerIndex</tt> by <tt>4</tt>
     * in this buffer.
     *
     * @throws RangeException
     *         if <tt>this.writableBytes</tt> is less than <tt>4</tt>
     */
    int writeFloat(float value);

    int writeFloatAhead(float value);

    /**
     * Sets the specified 64-bit floating point number at the current
     * <tt>writerIndex</tt> and increases the <tt>writerIndex</tt> by <tt>8</tt>
     * in this buffer.
     *
     * @throws RangeException
     *         if <tt>this.writableBytes</tt> is less than <tt>8</tt>
     */
    int writeDouble(double value);

    int writeDoubleAhead(double value);

    /**
     * Transfers the specified source buffer's data to this buffer starting at
     * the current <tt>writerIndex</tt> until the source buffer becomes
     * unreadable, and increases the <tt>writerIndex</tt> by the number of
     * the transferred bytes.  This method is basically same with
     * {@link #writeBytes(ChannelBuffer, int, int)}, except that this method
     * increases the <tt>readerIndex</tt> of the source buffer by the number of
     * the transferred bytes while {@link #writeBytes(ChannelBuffer, int, int)}
     * does not.
     *
     * @throws RangeException
     *         if <tt>src.readableBytes</tt> is greater than
     *            <tt>this.writableBytes</tt>
     *
     */
    int writeBytes(const ChannelBufferPtr& src);

    int writeBytesAhead(const ChannelBufferPtr& src);

    /**
     * Transfers the specified source buffer's data to this buffer starting at
     * the current <tt>writerIndex</tt> and increases the <tt>writerIndex</tt>
     * by the number of the transferred bytes (= <tt>length</tt>).  This method
     * is basically same with {@link #writeBytes(ChannelBuffer, int, int)},
     * except that this method increases the <tt>readerIndex</tt> of the source
     * buffer by the number of the transferred bytes (= <tt>length</tt>) while
     * {@link #writeBytes(ChannelBuffer, int, int)} does not.
     *
     * @param length the number of bytes to transfer
     *
     * @throws RangeException
     *         if <tt>length</tt> is greater than <tt>this.writableBytes</tt> or
     *         if <tt>length</tt> is greater then <tt>src.readableBytes</tt>
     */
    int writeBytes(const ChannelBufferPtr& src, int length);

    int writeBytesAhead(const ChannelBufferPtr& src, int length);

    /**
     * Transfers the specified source buffer's data to this buffer starting at
     * the current <tt>writerIndex</tt> and increases the <tt>writerIndex</tt>
     * by the number of the transferred bytes (= <tt>length</tt>).
     *
     * @param srcIndex the first index of the source
     * @param length   the number of bytes to transfer
     *
     * @throws RangeException
     *         if the specified <tt>srcIndex</tt> is less than <tt>0</tt>,
     *         if <tt>srcIndex + length</tt> is greater than
     *            <tt>src.capacity</tt>, or
     *         if <tt>length</tt> is greater than <tt>this.writableBytes</tt>
     */
    int writeBytes(const ConstChannelBufferPtr& src, int srcIndex, int length);

    int writeBytesAhead(const ConstChannelBufferPtr& src, int srcIndex, int length);

    /**
     * Transfers the specified source array's data to this buffer starting at
     * the current <tt>writerIndex</tt> and increases the <tt>writerIndex</tt>
     * by the number of the transferred bytes (= <tt>src.length()</tt>).
     *
     * @throws RangeException
     *         if <tt>src.length()</tt> is greater than <tt>this.writableBytes</tt>
     */
    int writeBytes(const ConstArray& src);

    int writeBytesAhead(const ConstArray& src);

    /**
     * Transfers the specified source array's data to this buffer starting at
     * the current <tt>writerIndex</tt> and increases the <tt>writerIndex</tt>
     * by the number of the transferred bytes (= <tt>length</tt>).
     *
     * @param srcIndex the first index of the source
     * @param length   the number of bytes to transfer
     *
     * @throws RangeException
     *         if the specified <tt>srcIndex</tt> is less than <tt>0</tt>,
     *         if <tt>srcIndex + length</tt> is greater than
     *            <tt>src.length()</tt>, or
     *         if <tt>length</tt> is greater than <tt>this.writableBytes</tt>
     */
    int writeBytes(const ConstArray& src, int srcIndex, int length);

    int writeBytesAhead(const ConstArray& src, int srcIndex, int length);

    /**
     * Transfers the specified source array's data to this buffer starting at
     * the current <tt>writerIndex</tt> and increases the <tt>writerIndex</tt>
     * by the number of the transferred bytes (= <tt>src.length()</tt>).
     *
     * @throws RangeException
     *         if <tt>src.length()</tt> is greater than <tt>this.writableBytes</tt>
     */
    int writeBytes(const std::string& src);

    int writeBytesAhead(const std::string& src);

    int writeBytes(const char* src, int length);
    int writeBytesAhead(const char* src, int length);

    /**
     * Transfers the specified source array's data to this buffer starting at
     * the current <tt>writerIndex</tt> and increases the <tt>writerIndex</tt>
     * by the number of the transferred bytes (= <tt>length</tt>).
     *
     * @param srcIndex the first index of the source
     * @param length   the number of bytes to transfer
     *
     * @throws RangeException
     *         if the specified <tt>srcIndex</tt> is less than <tt>0</tt>,
     *         if <tt>srcIndex + length</tt> is greater than
     *            <tt>src.length()</tt>, or
     *         if <tt>length</tt> is greater than <tt>this.writableBytes</tt>
     */
    int writeBytes(const std::string& src, int srcIndex, int length);

    int writeBytesAhead(const std::string& src, int srcIndex, int length);

    /**
     * Transfers the content of the specified stream to this buffer
     * starting at the current <tt>writerIndex</tt> and increases the
     * <tt>writerIndex</tt> by the number of the transferred bytes.
     *
     * @param length the number of bytes to transfer
     *
     * @return the actual number of bytes read in from the specified stream
     *
     * @throws RangeException
     *         if <tt>length</tt> is greater than <tt>this.writableBytes</tt>
     * @throws IOException
     *         if the specified stream threw an exception during I/O
     */
    int writeBytes(InputStream* in, int length);

    /**
     * Fills this buffer with <tt>NUL (0x00)</tt> starting at the current
     * <tt>writerIndex</tt> and increases the <tt>writerIndex</tt> by the
     * specified <tt>length</tt>.
     *
     * @param length the number of <tt>NUL</tt>s to write to the buffer
     *
     * @throws RangeException
     *         if <tt>length</tt> is greater than <tt>this.writableBytes</tt>
     */
    int writeZero(int length);

    int writeZeroAhead(int length);

    /**
     * Locates the first occurrence of the specified <tt>value</tt> in this
     * buffer.  The search takes place from the specified <tt>fromIndex</tt>
     * (inclusive)  to the specified <tt>toIndex</tt> (exclusive).
     * <p>
     * If <tt>fromIndex</tt> is greater than <tt>toIndex</tt>, the search is
     * performed in a reversed order.
     * <p>
     * This method does not modify <tt>readerIndex</tt> or <tt>writerIndex</tt> of
     * this buffer.
     *
     * @return the absolute index of the first occurrence if found.
     *         <tt>-1</tt> otherwise.
     */
    int indexOf(int fromIndex, int toIndex, boost::int8_t value) const;

    /**
     * Locates the first place where the specified <tt>indexFinder</tt>
     * returns <tt>true</tt>.  The search takes place from the specified
     * <tt>fromIndex</tt> (inclusive) to the specified <tt>toIndex</tt>
     * (exclusive).
     * <p>
     * If <tt>fromIndex</tt> is greater than <tt>toIndex</tt>, the search is
     * performed in a reversed order.
     * <p>
     * This method does not modify <tt>readerIndex</tt> or <tt>writerIndex</tt> of
     * this buffer.
     *
     * @return the absolute index where the specified <tt>indexFinder</tt>
     *         returned <tt>true</tt>.  <tt>-1</tt> if the <tt>indexFinder</tt>
     *         did not return <tt>true</tt> at all.
     */
    int indexOf(int fromIndex,
                int toIndex,
                const ChannelBufferIndexFinder::Functor& indexFinder) const;

    /**
     * Locates the first occurrence of the specified <tt>value</tt> in this
     * buffer.  The search takes place from the current <tt>readerIndex</tt>
     * (inclusive) to the current <tt>writerIndex</tt> (exclusive).
     * <p>
     * This method does not modify <tt>readerIndex</tt> or <tt>writerIndex</tt> of
     * this buffer.
     *
     * @return the number of bytes between the current <tt>readerIndex</tt>
     *         and the first occurrence if found. <tt>-1</tt> otherwise.
     */
    int bytesBefore(boost::int8_t value) const;

    /**
     * Locates the first place where the specified <tt>indexFinder</tt> returns
     * <tt>true</tt>.  The search takes place from the current <tt>readerIndex</tt>
     * (inclusive) to the current <tt>writerIndex</tt>.
     * <p>
     * This method does not modify <tt>readerIndex</tt> or <tt>writerIndex</tt> of
     * this buffer.
     *
     * @return the number of bytes between the current <tt>readerIndex</tt>
     *         and the first place where the <tt>indexFinder</tt> returned
     *         <tt>true</tt>.  <tt>-1</tt> if the <tt>indexFinder</tt> did not
     *         return <tt>true</tt> at all.
     */
    int bytesBefore(const ChannelBufferIndexFinder::Functor& indexFinder) const;

    /**
     * Locates the first occurrence of the specified <tt>value</tt> in this
     * buffer.  The search starts from the current <tt>readerIndex</tt>
     * (inclusive) and lasts for the specified <tt>length</tt>.
     * <p>
     * This method does not modify <tt>readerIndex</tt> or <tt>writerIndex</tt> of
     * this buffer.
     *
     * @return the number of bytes between the current <tt>readerIndex</tt>
     *         and the first occurrence if found. <tt>-1</tt> otherwise.
     *
     * @throws RangeException
     *         if <tt>length</tt> is greater than <tt>this.readableBytes</tt>
     */
    int bytesBefore(int length, boost::int8_t value) const;

    /**
     * Locates the first place where the specified <tt>indexFinder</tt> returns
     * <tt>true</tt>.  The search starts the current <tt>readerIndex</tt>
     * (inclusive) and lasts for the specified <tt>length</tt>.
     * <p>
     * This method does not modify <tt>readerIndex</tt> or <tt>writerIndex</tt> of
     * this buffer.
     *
     * @return the number of bytes between the current <tt>readerIndex</tt>
     *         and the first place where the <tt>indexFinder</tt> returned
     *         <tt>true</tt>.  <tt>-1</tt> if the <tt>indexFinder</tt> did not
     *         return <tt>true</tt> at all.
     *
     * @throws RangeException
     *         if <tt>length</tt> is greater than <tt>this.readableBytes</tt>
     */
    int bytesBefore(int length, const ChannelBufferIndexFinder::Functor& indexFinder) const;

    /**
     * Locates the first occurrence of the specified <tt>value</tt> in this
     * buffer.  The search starts from the specified <tt>index</tt> (inclusive)
     * and lasts for the specified <tt>length</tt>.
     * <p>
     * This method does not modify <tt>readerIndex</tt> or <tt>writerIndex</tt> of
     * this buffer.
     *
     * @return the number of bytes between the specified <tt>index</tt>
     *         and the first occurrence if found. <tt>-1</tt> otherwise.
     *
     * @remark 
     *         if <tt>index + length</tt> is greater than <tt>this.capacity</tt>,
     *         will only search to this.capacity.
     */
    int bytesBefore(int index, int length, boost::int8_t value) const;

    /**
     * Locates the first place where the specified <tt>indexFinder</tt> returns
     * <tt>true</tt>.  The search starts the specified <tt>index</tt> (inclusive)
     * and lasts for the specified <tt>length</tt>.
     * <p>
     * This method does not modify <tt>readerIndex</tt> or <tt>writerIndex</tt> of
     * this buffer.
     *
     * @return the number of bytes between the specified <tt>index</tt>
     *         and the first place where the <tt>indexFinder</tt> returned
     *         <tt>true</tt>.  <tt>-1</tt> if the <tt>indexFinder</tt> did not
     *         return <tt>true</tt> at all.
     *
     * @throws RangeException
     *         if <tt>index + length</tt> is greater than <tt>this.capacity</tt>
     */
    int bytesBefore(int index,
                    int length,
                    const ChannelBufferIndexFinder::Functor& indexFinder) const;

    /**
     * Compares the content of the specified buffer to the content of this
     * buffer.  Comparison is performed in the same manner with the string
     * comparison functions of various languages such as <tt>strcmp</tt>,
     * <tt>memcmp</tt> and {@link std::string#compareTo(std::string)}.
     */
    int compare(const ChannelBufferPtr& buffer) const;

    /**
     * Returns the string representation of this buffer.  This method does not
     * necessarily return the whole content of the buffer but returns
     * the values of the key properties such as {@link #readerIndex()},
     * {@link #writerIndex()} and {@link #capacity()}.
     */
    virtual std::string toString() const;

protected:
    /**
     * Throws an {@link RangeException} if the current
     * {@link #readableBytes() readable bytes} of this buffer is less
     * than the specified value.
     */
    virtual bool checkReadableBytes(int minReadableBytes) const;

protected:
    ChannelBuffer()
        : readerIdx(0),
        writerIdx(0),
        markedReaderIndex(0),
        markedWriterIndex(0) {}

    ChannelBuffer(int readerIdx, int writerIdx)
        : readerIdx(readerIdx),
          writerIdx(writerIdx),
          markedReaderIndex(0),
          markedWriterIndex(0) {}

protected:
    int readerIdx;
    int writerIdx;
    int markedReaderIndex;
    int markedWriterIndex;
};

}
}

#endif //#if !defined(CETTY_BUFFER_CHANNELBUFFER_H)

// Local Variables:
// mode: c++
// End:
