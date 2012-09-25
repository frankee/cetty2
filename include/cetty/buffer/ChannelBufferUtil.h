#if !defined(CETTY_BUFFER_CHANNELBUFFERUTIL_H)
#define CETTY_BUFFER_CHANNELBUFFERUTIL_H

/*
 * Copyright (c) 2010-2012 frankee zhou (frankee.zhou at gmail dot com)
 *
 * Distributed under under the Apache License, version 2.0 (the "License").
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 */

#include <cetty/Types.h>
#include <cetty/buffer/ChannelBufferPtr.h>

namespace cetty {
namespace buffer {

class ChannelBufferUtil {
public:
    /**
     * Creates a read-only buffer which disallows any modification operations
     * on the specified <tt>buffer</tt>.  The new buffer has the same
     * <tt>readerIndex</tt> and <tt>writerIndex</tt> with the specified
     * <tt>buffer</tt>.
     */
    //static ChannelBufferPtr unmodifiableBuffer(const ChannelBufferPtr& buffer);

    static int hashCode(const ChannelBufferPtr& buffer);

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
    static std::string hexDump(const ChannelBufferPtr& buffer,
                               int fromIndex,
                               int length);

    static void hexDump(const ChannelBufferPtr& buffer,
                        int fromIndex,
                        int length,
                        std::string* dump);

    /**
     * Returns <tt>true</tt> if and only if the two specified buffers are
     * identical to each other as described in <tt>ChannelBuffer#equals(Object)</tt>.
     * This method is useful when implementing a new buffer type.
     */
    static bool equals(const ConstChannelBufferPtr& lbuffer,
                       const ConstChannelBufferPtr& rbuffer);

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
    static int indexOf(const ConstChannelBufferPtr& buffer,
                       int fromIndex,
                       int toIndex,
                       int8_t value);

    /**
     * Toggles the endianness of the specified 16-bit short integer.
     */
    static int16_t swapShort(int16_t value);

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
    static int64_t swapLong(int64_t value);

private:
    static int firstIndexOf(const ConstChannelBufferPtr& buffer,
                            int fromIndex,
                            int toIndex,
                            int8_t value);

    static int lastIndexOf(const ConstChannelBufferPtr& buffer,
                           int fromIndex,
                           int toIndex,
                           int8_t value);

private:
    ChannelBufferUtil();
};


}
}

#endif //#if !defined(CETTY_BUFFER_CHANNELBUFFERUTIL_H)

// Local Variables:
// mode: c++
// End:
