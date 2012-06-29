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
#include <vector>
#include <stdarg.h>
#include <boost/cstdint.hpp>
#include <cetty/util/Character.h>

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

    /**
     * Split a string with a delimiter.
     * @param str the string.
     * @param delim the delimiter.
     * @param elems a vector object into which the result elements are pushed.
     * @return the number of result elements.
     */
    static size_t strsplit(const std::string& str, char delim, std::vector<std::string>* elems);


    /**
     * Split a string with delimiters.
     * @param str the string.
     * @param delims the delimiters.
     * @param elems a vector object into which the result elements are pushed.
     * @return the number of result elements.
     */
    static size_t strsplit(const std::string& str, const std::string& delims,
                    std::vector<std::string>* elems);

    /**
     * Check whether a string begins with a key.
     * @param str the string.
     * @param key the forward matching key string.
     * @return true if the target string begins with the key, else, it is false.
     */
    static bool strfwm(const std::string& str, const std::string& key);


    /**
     * Check whether a string ends with a key.
     * @param str the string.
     * @param key the backward matching key string.
     * @return true if the target string ends with the key, else, it is false.
     */
    static bool strbwm(const std::string& str, const std::string& key);

    /**
     * Convert the letters of a string into upper case.
     * @param str the string to convert.
     * @return the string itself.
     */
    static std::string* strtoupper(std::string* str);

    /**
     * Convert the letters of a string into lower case.
     * @param str the string to convert.
     * @return the string itself.
     */
    static std::string* strtolower(std::string* str);

    /**
     *
     */
    static inline boost::int64_t atoi(const std::string& str) {
        return StringUtil::atoi(str.c_str());
    }

    /**
     * Convert a decimal string to an integer.
     * @param str the decimal string.
     * @return the integer.  If the string does not contain numeric expression, 0 is returned.
     */
    static boost::int64_t atoi(const char* str);

    /**
     *
     *
     */
    static inline double atof(const std::string& str) {
        return StringUtil::atof(str.c_str());
    }

    /**
     * Convert a decimal string to a real number.
     * @param str the decimal string.
     * @return the real number.  If the string does not contain numeric expression, 0.0 is returned.
     */
    static double atof(const char* str);

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

