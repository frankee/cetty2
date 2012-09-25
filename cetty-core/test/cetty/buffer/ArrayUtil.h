#if !defined(CETTY_BUFFER_ARRAYUTIL_H)
#define CETTY_BUFFER_ARRAYUTIL_H

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

#include <stdarg.h>
#include <string>

namespace cetty {
namespace buffer {

using namespace cetty::util;

class ArrayUtil {
public:
    static std::string* create(int cnt, ...) {
        if (cnt <= 0) { return NULL; }

        std::string* buf = new std::string();
        va_list argp;
        int argno = 0;
        int charect;
        va_start(argp, cnt);

        while (argno < cnt) {
            charect = va_arg(argp, int);

            if (charect > 255) {
                charect = 0;
            }

            if (charect < -128) {
                charect = 0;
            }

            buf->append(1, charect);
            ++argno;
        }

        va_end(argp);

        return buf;
    }
};

}
}

#endif //#if !defined(CETTY_BUFFER_ARRAYUTIL_H)
