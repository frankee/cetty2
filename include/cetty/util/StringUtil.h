#if !defined(CETTY_UTIL_STRINGUTIL_H)
#define CETTY_UTIL_STRINGUTIL_H

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
#include <stdarg.h>
#include "cetty/util/Character.h"

namespace cetty {
namespace util {

class StringUtil {
public:
    static const std::string NEWLINE;

    static int hashCode(const std::string& str);

    /**
     * Strip a String of it's ISO control characters.
     *
     * @param value
     *          The String that should be stripped.
     * @return {@code String}
     *          A new String instance with its hexadecimal control characters replaced
     *          by a space. Or the unmodified String if it does not contain any ISO
     *          control characters.
     */
    static std::string stripControlCharacters(const std::string& value);

    /**
     * Append a formatted string at the end of a string.
     * @param dest the destination string.
     * @param format the printf-like format string.  The conversion character `%' can be used with
     * such flag characters as `s', `d', `o', `u', `x', `X', `c', `e', `E', `f', `g', `G', and `%'.
     * @param ap used according to the format string.
     */
    static void vstrprintf(std::string* dest, const char* format, va_list ap);

    /**
     * Append a formatted string at the end of a string.
     * @param dest the destination string.
     * @param format the printf-like format string.  The conversion character `%' can be used with
     * such flag characters as `s', `d', `o', `u', `x', `X', `c', `e', `E', `f', `g', `G', and `%'.
     * @param ... used according to the format string.
     */
    static void strprintf(std::string* dest, const char* format, ...);


    /**
     * Generate a formatted string.
     * @param format the printf-like format string.  The conversion character `%' can be used with
     * such flag characters as `s', `d', `o', `u', `x', `X', `c', `e', `E', `f', `g', `G', and `%'.
     * @param ... used according to the format string.
     * @return the result string.
     */
    static std::string strprintf(const char* format, ...);

private:
    StringUtil() {}
    ~StringUtil() {}
};

}
}

#endif //#if !defined(CETTY_UTIL_STRINGUTIL_H)

// Local Variables:
// mode: c++
// End:

