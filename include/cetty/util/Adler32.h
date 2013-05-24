#if !defined(CETTY_UTIL_ADLER32_H)
#define CETTY_UTIL_ADLER32_H

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

#include <string>
#include <cetty/Types.h>

namespace cetty {
namespace util {

class Adler32 {
public:
    static int adler32(const std::string& str);
    static int adler32(const char* buf, int len);
    static int adler32(int adler, const char* buf, int len);
};

inline
int Adler32::adler32(const std::string& str) {
    return adler32(1, str.c_str(), static_cast<int>(str.size()));
}
inline
int Adler32::adler32(const char* buf, int len) {
    return adler32(1, buf, len);
}

}
}

#endif //#if !defined(CETTY_UTIL_ADLER32_H)

// Local Variables:
// mode: c++
// End:
