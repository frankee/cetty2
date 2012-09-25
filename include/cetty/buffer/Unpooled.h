#if !defined(CETTY_BUFFER_UNPOOLED_H)
#define CETTY_BUFFER_UNPOOLED_H

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
#include <boost/smart_ptr/intrusive_ptr.hpp>
#include <cetty/Types.h>
#include <cetty/buffer/ChannelBuffer.h>

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
 * #include <cetty/buffer/Unpooled.h>
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

class Unpooled {
public:
    /**
     * A buffer whose capacity is <tt>0</tt>.
     */
    static ChannelBufferPtr EMPTY_BUFFER;

    static const int DEFAULT_AHEAD_BYTES = 8;

    /**
     * Creates a new big-endian Java heap buffer with reasonably small initial capacity, which
     * expands its capacity boundlessly on demand.
     */
    static ChannelBufferPtr buffer();

    static ChannelBufferPtr buffer(int initialCapacity);

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
    static ChannelBufferPtr buffer(int initialCapacity,
        int aheadBytes,
        int maxCapacity = MAX_INT32);

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
    static ChannelBufferPtr wrappedBuffer(char* bytes, int length);

    /**
     * Creates a new big-endian buffer which wraps the sub-region of the
     * specified <tt>array</tt>.  A modification on the specified array's
     * content will be visible to the returned buffer.
     */
    static ChannelBufferPtr wrappedBuffer(char* bytes, int offset, int length);

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

    static ChannelBufferPtr copiedBuffer(const char* bytes, int length);

    /**
     * Creates a new big-endian buffer whose content is a copy of the
     * specified <tt>array</tt>.  The new buffer's <tt>readerIndex</tt> and
     * <tt>writerIndex</tt> are <tt>0</tt> and <tt>array.length()</tt> respectively.
     */
    static ChannelBufferPtr copiedBuffer(const StringPiece& bytes);

    /**
     * Creates a new big-endian buffer whose content is a copy of the
     * specified <tt>array</tt>'s sub-region.  The new buffer's
     * <tt>readerIndex</tt> and <tt>writerIndex</tt> are <tt>0</tt> and
     * the specified <tt>length</tt> respectively.
     */
    static ChannelBufferPtr copiedBuffer(const StringPiece& arry, int offset, int length);

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


    static ChannelBufferPtr copiedBuffer(const std::vector<StringPiece>& bytes);

private:
    static ChannelBufferPtr compositeBuffer(const std::vector<ChannelBufferPtr>& components);

private:
    Unpooled() {}
};

}
}

#endif //#if !defined(CETTY_BUFFER_UNPOOLED_H)

// Local Variables:
// mode: c++
// End:
