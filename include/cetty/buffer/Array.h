#if !defined(CETTY_BUFFER_ARRAY_H)
#define CETTY_BUFFER_ARRAY_H

/*
 * Copyright (c) 2010-2011 frankee zhou (frankee.zhou at gmail dot com)
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

#include <string>
#include <cetty/util/Exception.h>

namespace cetty {
namespace buffer {

using namespace cetty::util;

class ConstArray;

/**
 * Simple wrap of the memory buffer.
 * it do not take care of the life cycle of the memory buffer.
 */
class Array {
public:
    Array() : buf(0), bufSize(0) {}
    Array(char* data, int length) : buf(data), bufSize(length) {}
    Array(const Array& arry) : buf(arry.buf), bufSize(arry.bufSize) {}

    Array& operator=(const Array& arry) {
        buf = arry.buf;
        bufSize = arry.bufSize;
        return *this;
    }

    /**
     *  retrieve the reference of the special char.
     *
     *  @throws
     *      RangeException if the <tt>offset < 0</tt> or <tt>offset >= bufSize</tt>.
     */
    char& operator[](int index) const {
        if (index < 0 || index >= bufSize) { throw RangeException(""); }

        return buf[index];
    }

    inline bool empty() const { return bufSize == 0; }

    inline char* data() const { return buf; }

    /**
     *  @throws
     *      RangeException if the <tt>offset < 0</tt> or <tt>offset >= bufSize</tt>.
     */
    inline char* data(int offset) const {
        if (offset < 0 || (offset && offset >= bufSize)) {
            throw RangeException("out of range in array");
        }

        return (buf + offset);
    }

    /**
     *  return the Array's capacity.
     */
    inline int length() const { return bufSize; }

    /**
     *  reset the Array's data and the size.
     */
    inline void reset(char* d, int len) { buf = d; bufSize = len; }

    /**
     * just wrap the std::string to an Array, no copy happened.
     */
    static Array wrappedArray(std::string& string);

    /**
     * copy and new the exactly same array.
     * the caller must take care of the life cycle of the returned Array.
     */
    static Array clone(const Array& arry);

    /**
     * utility function copy the data from src Array to the dst Array.
     *
     * @throws
     *      RangeException if the dstIndex, srcIndex and the length out of the range.
     *      InvalidArgumentException if dst and src has not valid buffer pointer.
     */
    static void  copy(const Array& dst, int dstIndex, const ConstArray& src, int srcIndex, int length);

private:
    char* buf;
    int   bufSize;
};

/**
 *  Const version of Array, which data is unmodifiable.
 */
class ConstArray {
public:
    ConstArray() : buf(0), bufSize(0) {}
    ConstArray(const char* data, int length) : buf(data), bufSize(length) {}
    ConstArray(const Array& arry) : buf(arry.data()), bufSize(arry.length()) {}
    ConstArray(const ConstArray& arry) : buf(arry.buf), bufSize(arry.bufSize) {}

    ConstArray& operator=(const ConstArray& arry) {
        buf = arry.buf;
        bufSize = arry.bufSize;
        return *this;
    }

    /**
     *  retrieve the reference of the special char.
     *
     *  @throws
     *      RangeException if the <tt>offset < 0</tt> or <tt>offset >= bufSize</tt>.
     */
    const char& operator[](int index) const {
        if (index < 0 || index >= bufSize) { throw RangeException(""); }

        return buf[index];
    }

    bool empty() const { return bufSize == 0; }

    const char* data() const { return buf; }

    /**
     *  @throws
     *      RangeException if the <tt>offset < 0</tt> or <tt>offset >= bufSize</tt>.
     */
    const char* data(int offset) const {
        if (offset < 0 || (offset && offset >= bufSize)) { throw RangeException(); }

        return buf + offset;
    }

    /**
     *  return the Array's capacity.
     */
    int length() const { return bufSize; }

    /**
     * just wrap the std::string to an Array, no copy happened.
     */
    static ConstArray fromString(const std::string& string) {
        return ConstArray(string.data(), static_cast<int>(string.size()));
    }

    /**
     * copy and new the exactly same array.
     * the caller must take care of the life cycle of the returned Array.
     */
    static Array clone(const ConstArray& arry);

private:
    const char* buf;
    int         bufSize;
};

}
}

#endif //#if !defined(CETTY_BUFFER_ARRAY_H)

// Local Variables:
// mode: c++
// End:
