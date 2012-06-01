#if !defined(CETTY_UTIL_SIMPLESTRING_H)
#define CETTY_UTIL_SIMPLESTRING_H

/**
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
#include <boost/cstdint.hpp>
#include <boost/assert.hpp>

namespace cetty {
namespace util {

class SimpleString {
public:
    char* data;
    int   size;

public:
    SimpleString() : data(0), size(0) {}
    SimpleString(char* data) : data(data), size(NULL == data ? 0 : strlen(data)) {}
    SimpleString(char* data, int size) : data(data), size(size) {}

    inline void clear() { data = NULL; size = 0; }
    inline bool empty()  const { return size == 0; }
    inline int  length() const { return size; }

    inline operator char* () { return data; }

    bool operator==(const char* str) const;
    bool operator==(const std::string& str) const;

    inline char* c_str() { return data; }
    inline const char* c_str() const { return data; }

    inline char operator[](int index) const {
        return data[index];
    }
    inline char& operator[](int index) {
        return data[index];
    }
    inline char at(int index) const {
        BOOST_ASSERT(index < size);
        return data[index];
    }

    SimpleString substr(int start, int end) const;

    SimpleString trim() const;
    bool iequals(const std::string& str) const;

    int toInt();
    boost::int64_t toInt64();
    double toDouble();
};

}
}

#endif //#if !defined(CETTY_UTIL_SIMPLESTRING_H)

// Local Variables:
// mode: c++
// End:

