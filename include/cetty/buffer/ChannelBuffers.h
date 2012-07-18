#if !defined(CETTY_BUFFER_CHANNELBUFFERS_H)
#define CETTY_BUFFER_CHANNELBUFFERS_H

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

#include <vector>
#include <boost/cstdint.hpp>
#include <boost/smart_ptr/intrusive_ptr.hpp>
#include <cetty/buffer/Array.h>
#include <cetty/buffer/ChannelBuffer.h>
#include <cetty/buffer/ChannelBufferIndexFinder.h>

namespace cetty {
namespace buffer {

class ByteOrder;
class ChannelBufferFactory;

/**
 * Creates a new {@link ChannelBuffer} by allocating new space or by wrapping
 * or copying existing byte arrays, byte buffers and a string.
 *
 * <h3>Use static import</h3>
 * This classes is intended to be used with Java 5 static import statement:
 *
 * <pre>
 * #include <cetty/buffer/ChannelBuffers.h>
 *
 * {@link ChannelBufferPtr} *heapBuffer    = ChannelBuffers::buffer(128);
 * {@link ChannelBufferPtr} *dynamicBuffer = ChannelBuffers::dynamicBuffer(512);
 * {@link ChannelBufferPtr} *wrappedBuffer = ChannelBuffers::wrappedBuffer(Array(new char[128], 128));
 * {@link ChannelBufferPtr} *copiedBuffer  = ChannelBuffers::copiedBuffer(arry);
 * </pre>
 *
 * <h3>Allocating a new buffer</h3>
 *
 * Three buffer types are provided out of the box.
 *
 * <ul>
 * <li>{@link #buffer(int)} allocates a new fixed-capacity heap buffer.</li>
 * <li>{@link #dynamicBuffer(int)} allocates a new dynamic-capacity heap
 *     buffer, whose capacity increases automatically as needed by a write
 *     operation.</li>
 * </ul>
 *
 * <h3>Creating a wrapped buffer</h3>
 *
 * Wrapped buffer is a buffer which is a view of one or more existing
 * byte arrays and byte buffers.  Any changes in the content of the original
 * array or buffer will be visible in the wrapped buffer.  Various wrapper
 * methods are provided and their name is all <tt>wrappedBuffer()</tt>.
 * You might want to take a look at the methods that accept varargs closely if
 * you want to create a buffer which is composed of more than one array to
 * reduce the number of memory copy.
 *
 * <h3>Creating a copied buffer</h3>
 *
 * Copied buffer is a deep copy of one or more existing byte arrays, byte
 * buffers or a string.  Unlike a wrapped buffer, there's no shared data
 * between the original data and the copied buffer.  Various copy methods are
 * provided and their name is all <tt>copiedBuffer()</tt>.  It is also convenient
 * to use this operation to merge multiple buffers into one buffer.
 *
 * <h3>Miscellaneous utility methods</h3>
 *
 * This class also provides various utility methods to help implementation
 * of a new buffer type, generation of hex dump and swapping an integer's
 * byte order.
 *
 *
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 *
 * @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
 *
 */

class ChannelBuffers {
public:
    /**
     * A buffer whose capacity is <tt>0</tt>.
     */
    static ChannelBufferPtr EMPTY_BUFFER;

    static const int DEFAULT_AHEAD_BYTES = 8;

    /**
     * Creates a new big-endian heap buffer with the specified
     * <tt>capacity + aheadBytes</tt>.  The new buffer's <tt>readerIndex</tt> and
     * <tt>writerIndex</tt> are <tt>aheadBytes</tt>.
     *
     *  @param  int capacity:
     *  @param  int aheadBytes, for some situation, you can prepend data without copy.
     *                          default value is DEFAULT_AHEAD_BYTES.
     *
     *  @return if capacity is <tt>0</tt>, return <tt>EMPTY_BUFFER</tt>
     */
    static ChannelBufferPtr buffer(int capacity, int aheadBytes = DEFAULT_AHEAD_BYTES);

    /**
     * Creates a new heap buffer with the specified <tt>endianness</tt>
     * and <tt>capacity</tt>.  The new buffer's <tt>readerIndex</tt> and
     * <tt>writerIndex</tt> are <tt>0</tt>.
     */
    static ChannelBufferPtr buffer(const ByteOrder& endianness,
                                   int capacity,
                                   int aheadBytes = DEFAULT_AHEAD_BYTES);

    /**
     * Creates a new big-endian dynamic buffer whose estimated data length is
     * <tt>256</tt> bytes.  The new buffer's <tt>readerIndex</tt> and
     * <tt>writerIndex</tt> are <tt>0</tt>.
     */
    static ChannelBufferPtr dynamicBuffer();

    static ChannelBufferPtr dynamicBuffer(ChannelBufferFactory& factory);

    /**
     * Creates a new big-endian dynamic buffer with the specified estimated
     * data length.  More accurate estimation yields less unexpected
     * reallocation overhead.  The new buffer's <tt>readerIndex</tt> and
     * <tt>writerIndex</tt> are <tt>0</tt>.
     */
    static ChannelBufferPtr dynamicBuffer(int estimatedLength);

    /**
     * Creates a new dynamic buffer with the specified endianness and
     * the specified estimated data length.  More accurate estimation yields
     * less unexpected reallocation overhead.  The new buffer's
     * <tt>readerIndex</tt> and <tt>writerIndex</tt> are <tt>0</tt>.
     */
    static ChannelBufferPtr dynamicBuffer(const ByteOrder& endianness, int estimatedLength);

    /**
     * Creates a new big-endian dynamic buffer with the specified estimated
     * data length using the specified factory.  More accurate estimation yields
     * less unexpected reallocation overhead.  The new buffer's <tt>readerIndex</tt>
     * and <tt>writerIndex</tt> are <tt>0</tt>.
     */
    static ChannelBufferPtr dynamicBuffer(int estimatedLength, ChannelBufferFactory& factory);

    /**
     * Creates a new dynamic buffer with the specified endianness and
     * the specified estimated data length using the specified factory.
     * More accurate estimation yields less unexpected reallocation overhead.
     * The new buffer's <tt>readerIndex</tt> and <tt>writerIndex</tt> are <tt>0</tt>.
     */
    static ChannelBufferPtr dynamicBuffer(const ByteOrder& endianness,
                                          int estimatedLength,
                                          ChannelBufferFactory& factory);

    /**
     * Creates a new big-endian buffer which wraps the specified <tt>string</tt>.
     * A modification on the specified array's content will be visible to the
     * returned buffer.
     */
    static ChannelBufferPtr wrappedBuffer(std::string& str);

    /**
     * Creates a new big-endian buffer which wraps the specified <tt>array</tt>.
     * A modification on the specified array's content will be visible to the
     * returned buffer.
     */
    static ChannelBufferPtr wrappedBuffer(const Array& arry);

    /**
     * Creates a new buffer which wraps the specified <tt>array</tt> with the
     * specified <tt>endianness</tt>.  A modification on the specified array's
     * content will be visible to the returned buffer.
     */
    static ChannelBufferPtr wrappedBuffer(const ByteOrder& endianness, const Array& arry);

    /**
     * Creates a new big-endian buffer which wraps the sub-region of the
     * specified <tt>array</tt>.  A modification on the specified array's
     * content will be visible to the returned buffer.
     */
    static ChannelBufferPtr wrappedBuffer(const Array& arry, int offset, int length);

    /**
     * Creates a new buffer which wraps the sub-region of the specified
     * <tt>array</tt> with the specified <tt>endianness</tt>.  A modification on
     * the specified array's content will be visible to the returned buffer.
     */
    static ChannelBufferPtr wrappedBuffer(const ByteOrder& endianness,
                                          const Array& arry,
                                          int offset,
                                          int length);

    /**
     * Creates a new big-endian composite buffer which wraps the specified
     * arrays without copying them.  A modification on the specified arrays'
     * content will be visible to the returned buffer.
     */
    static ChannelBufferPtr wrappedBuffer(const Array& array0, const Array& array1);

    /**
     * Creates a new big-endian composite buffer which wraps the specified
     * arrays without copying them.  A modification on the specified arrays'
     * content will be visible to the returned buffer.
     */
    static ChannelBufferPtr wrappedBuffer(const Array& array0,
                                          const Array& array1,
                                          const Array& array2);

    /**
     * Creates a new composite buffer which wraps the specified arrays without
     * copying them.  A modification on the specified arrays' content will be
     * visible to the returned buffer.
     *
     * @param endianness the endianness of the new buffer
     */
    static ChannelBufferPtr wrappedBuffer(const ByteOrder& endianness,
                                          const Array& array0,
                                          const Array& array1);

    /**
     * Creates a new composite buffer which wraps the specified arrays without
     * copying them.  A modification on the specified arrays' content will be
     * visible to the returned buffer.
     *
     * @param endianness the endianness of the new buffer
     */
    static ChannelBufferPtr wrappedBuffer(const ByteOrder& endianness,
                                          const Array& array0,
                                          const Array& array1,
                                          const Array& array2);

    /**
     * Creates a new composite buffer which wraps the specified arrays without
     * copying them.  A modification on the specified arrays' content will be
     * visible to the returned buffer.
     *
     * @param endianness the endianness of the new buffer
     */
    static ChannelBufferPtr wrappedBuffer(std::vector<Array>& arrays);

    /**
     * Creates a new composite buffer which wraps the specified arrays without
     * copying them.  A modification on the specified arrays' content will be
     * visible to the returned buffer.
     *
     * @param endianness the endianness of the new buffer
     */
    static ChannelBufferPtr wrappedBuffer(const ByteOrder& endianness, std::vector<Array>& arrays);

    /**
     * Creates a new buffer which wraps the specified buffer's readable bytes.
     * A modification on the specified buffer's content will be visible to the
     * returned buffer.
     */
    static ChannelBufferPtr wrappedBuffer(const ChannelBufferPtr& buffer);

    /**
     * Creates a new composite buffer which wraps the readable bytes of the
     * specified buffers without copying them.  A modification on the content
     * of the specified buffers will be visible to the returned buffer.
     *
     * @throws InvalidArgumentException
     *         if the specified buffers' endianness are different from each
     *         other
     */
    static ChannelBufferPtr wrappedBuffer(const ChannelBufferPtr& buffer0,
                                          const ChannelBufferPtr& buffer1);

    static ChannelBufferPtr wrappedBuffer(const ChannelBufferPtr& buffer0,
                                          const ChannelBufferPtr& buffer1,
                                          const ChannelBufferPtr& buffer2);

    static ChannelBufferPtr wrappedBuffer(const std::vector<ChannelBufferPtr>& buffers);

    /**
     * Creates a new big-endian buffer whose content is a copy of the
     * specified <tt>array</tt>.  The new buffer's <tt>readerIndex</tt> and
     * <tt>writerIndex</tt> are <tt>0</tt> and <tt>array.length()</tt> respectively.
     */
    static ChannelBufferPtr copiedBuffer(const ConstArray& arry);

    /**
     * Creates a new buffer with the specified <tt>endianness</tt> whose
     * content is a copy of the specified <tt>array</tt>.  The new buffer's
     * <tt>readerIndex</tt> and <tt>writerIndex</tt> are <tt>0</tt> and
     * <tt>array.length()</tt> respectively.
     */
    static ChannelBufferPtr copiedBuffer(const ByteOrder& endianness, const ConstArray& arry);

    /**
     * Creates a new big-endian buffer whose content is a copy of the
     * specified <tt>array</tt>'s sub-region.  The new buffer's
     * <tt>readerIndex</tt> and <tt>writerIndex</tt> are <tt>0</tt> and
     * the specified <tt>length</tt> respectively.
     */
    static ChannelBufferPtr copiedBuffer(const ConstArray& arry, int offset, int length);

    /**
     * Creates a new buffer with the specified <tt>endianness</tt> whose
     * content is a copy of the specified <tt>array</tt>'s sub-region.  The new
     * buffer's <tt>readerIndex</tt> and <tt>writerIndex</tt> are <tt>0</tt> and
     * the specified <tt>length</tt> respectively.
     */
    static ChannelBufferPtr copiedBuffer(const ByteOrder& endianness,
                                         const ConstArray& arry,
                                         int offset,
                                         int length);

    /**
     * Creates a new big-endian buffer whose content is a merged copy of
     * the specified <tt>arrays</tt>.  The new buffer's <tt>readerIndex</tt>
     * and <tt>writerIndex</tt> are <tt>0</tt> and the sum of all arrays'
     * <tt>length</tt> respectively.
     */
    static ChannelBufferPtr copiedBuffer(const Array& array0, const Array& array1);

    static ChannelBufferPtr copiedBuffer(const Array& array0,
                                         const Array& array1,
                                         const Array& array2);

    static ChannelBufferPtr copiedBuffer(const std::vector<Array>& arrays);

    /**
     * Creates a new buffer with the specified <tt>endianness</tt> whose
     * content is a merged copy of the specified <tt>arrays</tt>.  The new
     * buffer's <tt>readerIndex</tt> and <tt>writerIndex</tt> are <tt>0</tt>
     * and the sum of all arrays' <tt>length</tt> respectively.
     */
    static ChannelBufferPtr copiedBuffer(const ByteOrder& endianness,
                                         const Array& array0,
                                         const Array& array1);

    static ChannelBufferPtr copiedBuffer(const ByteOrder& endianness,
                                         const Array& array0,
                                         const Array& array1,
                                         const Array& array2);

    static ChannelBufferPtr copiedBuffer(const ByteOrder& endianness,
                                         const std::vector<Array>& arrays);

    /**
     * Creates a new buffer whose content is a copy of the specified
     * <tt>buffer</tt>'s readable bytes.  The new buffer's <tt>readerIndex</tt>
     * and <tt>writerIndex</tt> are <tt>0</tt> and <tt>buffer.readableBytes</tt>
     * respectively.
     */
    static ChannelBufferPtr copiedBuffer(const ChannelBufferPtr& buffer);

    /**
     * Creates a new buffer whose content is a merged copy of the specified
     * <tt>buffers</tt>' readable bytes.  The new buffer's <tt>readerIndex</tt>
     * and <tt>writerIndex</tt> are <tt>0</tt> and the sum of all buffers'
     * <tt>readableBytes</tt> respectively.
     *
     * @throws InvalidArgumentException
     *         if the specified buffers' endianness are different from each
     *         other
     */
    static ChannelBufferPtr copiedBuffer(const ChannelBufferPtr& buffer0,
                                         const ChannelBufferPtr& buffer1);

    static ChannelBufferPtr copiedBuffer(const ChannelBufferPtr& buffer0,
                                         const ChannelBufferPtr& buffer1,
                                         const ChannelBufferPtr& buffer2);

    static ChannelBufferPtr copiedBuffer(const std::vector<ChannelBufferPtr>& buffers);


    static ChannelBufferPtr copiedBuffer(const std::string& string);

    /**
     * Creates a new buffer with the specified <tt>endianness</tt> whose
     * content is the specified <tt>string</tt> encoded in the specified
     * <tt>charset</tt>.  The new buffer's <tt>readerIndex</tt> and
     * <tt>writerIndex</tt> are <tt>0</tt> and the length of the encoded string
     * respectively.
     */
    static ChannelBufferPtr copiedBuffer(const ByteOrder& endianness, const std::string& string);

    /**
     * Creates a new big-endian buffer whose content is a subregion of
     * the specified <tt>array</tt> encoded in the specified <tt>charset</tt>.
     * The new buffer's <tt>readerIndex</tt> and <tt>writerIndex</tt> are
     * <tt>0</tt> and the length of the encoded string respectively.
     */
    static ChannelBufferPtr copiedBuffer(const std::string& string, int offset, int length);

    /**
     * Creates a new buffer with the specified <tt>endianness</tt> whose
     * content is a subregion of the specified <tt>string</tt>.
     * The new buffer's <tt>readerIndex</tt> and <tt>writerIndex</tt> are
     * <tt>0</tt> and the length of the encoded string respectively.
     */
    static ChannelBufferPtr copiedBuffer(const ByteOrder& endianness,
                                         const std::string& string,
                                         int offset,
                                         int length);

    /**
     * Creates a read-only buffer which disallows any modification operations
     * on the specified <tt>buffer</tt>.  The new buffer has the same
     * <tt>readerIndex</tt> and <tt>writerIndex</tt> with the specified
     * <tt>buffer</tt>.
     */
    //static ChannelBufferPtr unmodifiableBuffer(const ChannelBufferPtr& buffer);

    static int hashCode(const ChannelBuffer& buffer);

    /**
     * Returns a <a href="http://en.wikipedia.org/wiki/Hex_dump">hex dump</a>
     * of the specified buffer's readable bytes.
     */
    static std::string hexDump(const ChannelBufferPtr& buffer);

    static void hexDump(const ChannelBufferPtr& buffer, std::string* dump);

    /**
     * Returns a <a href="http://en.wikipedia.org/wiki/Hex_dump">hex dump</a>
     * of the specified buffer's sub-region.
     */
    static std::string hexDump(const ChannelBufferPtr& buffer, int fromIndex, int length);

    static void hexDump(const ChannelBufferPtr& buffer, int fromIndex, int length, std::string* dump);

    /**
     * Returns <tt>true</tt> if and only if the two specified buffers are
     * identical to each other as described in <tt>ChannelBuffer#equals(Object)</tt>.
     * This method is useful when implementing a new buffer type.
     */
    static bool equals(const ConstChannelBufferPtr& lbuffer, const ConstChannelBufferPtr& rbuffer);

    /**
     * Compares the two specified buffers as described in {@link ChannelBuffer#compareTo(ChannelBuffer)}.
     * This method is useful when implementing a new buffer type.
     */
    static int compare(const ConstChannelBufferPtr& lBuffer,
                       const ConstChannelBufferPtr& rBuffer);

    /**
     * The default implementation of {@link ChannelBuffer#indexOf(int, int, byte)}.
     * This method is useful when implementing a new buffer type.
     */
    static int indexOf(const ConstChannelBufferPtr& buffer, int fromIndex, int toIndex, boost::int8_t value);

    /**
     * The default implementation of {@link ChannelBuffer#indexOf(int, int, ChannelBufferIndexFinder)}.
     * This method is useful when implementing a new buffer type.
     */
    static int indexOf(const ConstChannelBufferPtr& buffer, int fromIndex, int toIndex, const ChannelBufferIndexFinder::Functor& indexFinder);

    /**
     * Toggles the endianness of the specified 16-bit short integer.
     */
    static boost::int16_t swapShort(boost::int16_t value);

    /**
     * Toggles the endianness of the specified 24-bit medium integer.
     */
    static int swapMedium(int value);

    /**
     * Toggles the endianness of the specified 32-bit integer.
     */
    static int swapInt(int value);

    /**
     * Toggles the endianness of the specified 64-bit long integer.
     */
    static boost::int64_t swapLong(boost::int64_t value);

private:
    static ChannelBufferPtr compositeBuffer(const ByteOrder& endianness, const std::vector<ChannelBufferPtr>& components);

    static int firstIndexOf(const ConstChannelBufferPtr& buffer, int fromIndex, int toIndex, boost::int8_t value);
    static int lastIndexOf(const ConstChannelBufferPtr& buffer, int fromIndex, int toIndex, boost::int8_t value);

    static int firstIndexOf(const ConstChannelBufferPtr& buffer, int fromIndex, int toIndex, const ChannelBufferIndexFinder::Functor& indexFinder);
    static int lastIndexOf(const ConstChannelBufferPtr& buffer, int fromIndex, int toIndex, const ChannelBufferIndexFinder::Functor& indexFinder);

    ChannelBuffers() {}
};

}
}


#endif //#if !defined(CETTY_BUFFER_CHANNELBUFFERS_H)

// Local Variables:
// mode: c++
// End:
