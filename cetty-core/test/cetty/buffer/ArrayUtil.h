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
#include "cetty/buffer/Array.h"

namespace cetty { namespace buffer { 

class ArrayUtil{
public:
    static Array create(int cnt, ...) {
        if (cnt <= 0) return Array();

        char* buf = new char[cnt];
        va_list argp;
        int argno = 0;
        int charect;
        va_start(argp, cnt);
        while(argno < cnt) {
            charect = va_arg(argp, int);
            if (charect > 255) {
                charect = 0;
            }
            if (charect < -128) {
                charect = 0;
            }
            buf[argno] = (char)charect;
            ++argno;
        }
        va_end(argp);

        return Array(buf, cnt);
    }
};

}}

#endif //#if !defined(CETTY_BUFFER_ARRAYUTIL_H)

