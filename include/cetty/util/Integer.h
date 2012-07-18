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

#if !defined(CETTY_UTIL_INTEGER_H)
#define CETTY_UTIL_INTEGER_H

#include <string>
#include <limits.h>
#include <stdio.h>

#include <boost/assert.hpp>
#include <boost/cstdint.hpp>
#include <boost/limits.hpp>

namespace cetty {
namespace util {

class Integer {
public:
    static const int MAX_VALUE = INT_MAX;
    static const int MIN_VALUE = INT_MIN;

    Integer() : value(MAX_VALUE) {}
    Integer(int value) : value(value) {}
    Integer(const Integer& integer) : value(integer.value) {}
    Integer(const std::string& s) : value(MAX_VALUE) { if (!s.empty()) { value = atoi(s.c_str()); } }

    Integer& operator =(const Integer& integer) { this->value = integer.value; return *this; }
    bool operator ==(const Integer& integer) const { return this->value == integer.value; }
    bool operator !=(const Integer& integer) const { return this->value != integer.value; }
    bool operator < (const Integer& integer) const { return this->value <  integer.value; }

    Integer& operator +=(const Integer& integer) {
        this->value += integer.value; return *this;
    }
    Integer& operator +=(int value) { this->value += value; return *this; }

    char byteValue() { return char(value & 0xf); }
    int  intValue() { return value; }

    std::string toString() {
        return toString(value);
    }

    static Integer decode(const std::string& nm);

    // Returns a Integer instance representing the specified int value.
    static Integer valueOf(int i);
    static Integer valueOf(const std::string& s);
    static Integer valueOf(const std::string& s, int radix);

    static int parse(const std::string& s);
    static int parse(const std::string& s, int radix);
    static int parse(const char* s);
    static int parse(const char* s, int radix);

    static bool tryParse(const std::string& s, int& value);
    static bool tryParseHex(const std::string& s, unsigned int& value);
    static bool tryParseHex(const char* s, unsigned int& value);

    /**
     *  @brief 返回一个表示指定整数的 string 对象。
     *
     *  @param  [in]int value:
     *
     *  @return
     */
    static std::string toString(boost::int64_t value);
    static std::string toBinaryString(boost::int64_t value);
    static std::string toHexString(boost::int64_t value);
    static std::string toOctalString(boost::int64_t value);

    static std::string toString(boost::int64_t value, int width, bool zeroPadded = false);
    static std::string toBinaryString(boost::int64_t value, int width, bool zeroPadded = false);
    static std::string toOctalString(boost::int64_t value, int width, bool zeroPadded = false);
    static std::string toHexString(boost::int64_t value, int width, bool zeroPadded = false);

    static void appendString(boost::int64_t value, std::string* str);
    static void appendBinaryString(boost::int64_t value, std::string* str);
    static void appendOctalString(boost::int64_t value, std::string* str);
    static void appendHexString(boost::int64_t value, std::string* str);

    static void appendString(boost::int64_t value, int width, bool zeroPadded, std::string* str);
    static void appendBinaryString(boost::int64_t value, int width, bool zeroPadded, std::string* str);
    static void appendOctalString(boost::int64_t value, int width, bool zeroPadded, std::string* str);
    static void appendHexString(boost::int64_t value, int width, bool zeroPadded, std::string* str);

private:
    int value;
};

}
}

#endif //#if !defined(CETTY_UTIL_INTEGER_H)

// Local Variables:
// mode: c++
// End:
