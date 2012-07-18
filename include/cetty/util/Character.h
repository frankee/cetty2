#if !defined(CETTY_UTIL_CHARACTER_H)
#define CETTY_UTIL_CHARACTER_H

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

#include <ctype.h>

namespace cetty {
namespace util {

class Character {
public:
    static bool isISOControl(int ch) {
        return iscntrl(ch) != 0;
    }
    static bool isWhitespace(int ch) {
        return isspace(ch) != 0;
    }
    static bool isDigit(int ch) {
        return isdigit(ch) != 0;
    }
    static bool isHex(int ch) {
        return (ch >= '0' && ch <= '9')
               || (ch >= 'a' && ch <= 'f')
               || (ch >= 'A' && ch <= 'F');
    }

};

}
}

#endif //#if !defined(CETTY_UTIL_CHARACTER_H)

// Local Variables:
// mode: c++
// End:
