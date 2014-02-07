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

#include <cetty/util/StringUtil.h>

#include <stdarg.h>
#include <errno.h>
#include <float.h>    // FLT_DIG and DBL_DIG
#include <limits>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <boost/crc.hpp>
#include <boost/assert.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <cetty/util/StlUtil.h>
#include <cetty/util/StringPiece.h>

namespace cetty {
namespace util {

#ifdef _MSC_VER
#define strtoll  _strtoi64
#define strtoull _strtoui64
#define snprintf _snprintf
#elif defined(__DECCXX) && defined(__osf__)
// HP C++ on Tru64 does not have strtoll, but strtol is already 64-bit.
#define strtoll strtol
#define strtoull strtoul
#endif

const std::string StringUtil::NEWLINE("\r\n");

int StringUtil::hashCode(const std::string& str) {
    boost::crc_32_type crc32;
    crc32.process_bytes((void const*)(str.data()), str.size());
    return crc32.checksum();
}

std::string StringUtil::stripControlCharacters(const std::string& value) {
    if (value.empty()) {
        return value;
    }

    bool hasControlChars = false;

    for (int i = (int)(value.length()) - 1; i >= 0; --i) {
        if (isISOControl(value.at(i))) {
            hasControlChars = true;
            break;
        }
    }

    if (!hasControlChars) {
        return value;
    }

    std::string buf;
    buf.reserve(value.length());

    size_t i = 0;

    // Skip initial control characters (i.e. left trim)
    for (; i < value.length(); ++i) {
        if (!isISOControl(value.at(i))) {
            break;
        }
    }

    // Copy non control characters and substitute control characters with
    // a space.  The last control characters are trimmed.
    bool suppressingControlChars = false;

    for (; i < value.length(); ++i) {
        if (isISOControl(value.at(i))) {
            suppressingControlChars = true;
            continue;
        }
        else {
            if (suppressingControlChars) {
                suppressingControlChars = false;
                buf += ' ';
            }

            buf += value.at(i);
        }
    }

    return buf;
}

/**
 * Append a formatted string at the end of a string.
 * @param dest the destination string.
 * @param format the printf-like format string.  The conversion character `%' can be used with
 * such flag characters as `s', `d', `o', `u', `x', `X', `c', `e', `E', `f', `g', `G', and `%'.
 * @param ap used according to the format string.
*/
static void vstrprintf(std::string* dest, const char* format, va_list ap) {
    BOOST_ASSERT(dest && format);
    static const size_t NUMBUFSIZ = 32;

    while (*format != '\0') {
        if (*format == '%') {
            char cbuf[NUMBUFSIZ];
            cbuf[0] = '%';
            size_t cbsiz = 1;
            int32_t lnum = 0;
            format++;

            while (std::strchr("0123456789 .+-hlLz", *format) && *format != '\0' &&
                    cbsiz < NUMBUFSIZ - 1) {
                if (*format == 'l' || *format == 'L') { lnum++; }

                cbuf[cbsiz++] = *(format++);
            }

            cbuf[cbsiz++] = *format;
            cbuf[cbsiz] = '\0';

            switch (*format) {
            case 's': {
                const char* tmp = va_arg(ap, const char*);

                if (tmp) {
                    dest->append(tmp);
                }
                else {
                    dest->append("(null)");
                }

                break;
            }

            case 'd': {
                char tbuf[NUMBUFSIZ*4];
                size_t tsiz;

                if (lnum >= 2) {
                    tsiz = std::sprintf(tbuf, cbuf, va_arg(ap, long long));
                }
                else if (lnum >= 1) {
                    tsiz = std::sprintf(tbuf, cbuf, va_arg(ap, long));
                }
                else {
                    tsiz = std::sprintf(tbuf, cbuf, va_arg(ap, int));
                }

                dest->append(tbuf, tsiz);
                break;
            }

            case 'o': case 'u': case 'x': case 'X': case 'c': {
                char tbuf[NUMBUFSIZ*4];
                size_t tsiz;

                if (lnum >= 2) {
                    tsiz = std::sprintf(tbuf, cbuf, va_arg(ap, unsigned long long));
                }
                else if (lnum >= 1) {
                    tsiz = std::sprintf(tbuf, cbuf, va_arg(ap, unsigned long));
                }
                else {
                    tsiz = std::sprintf(tbuf, cbuf, va_arg(ap, unsigned int));
                }

                dest->append(tbuf, tsiz);
                break;
            }

            case 'e': case 'E': case 'f': case 'g': case 'G': {
                char tbuf[NUMBUFSIZ*4];
                size_t tsiz;

                if (lnum >= 1) {
#if defined(_MSC_VER)
                    tsiz = _snprintf(tbuf, sizeof(tbuf), cbuf, va_arg(ap, long double));
#else
                    tsiz = snprintf(tbuf, sizeof(tbuf), cbuf, va_arg(ap, long double));
#endif
                }
                else {
#if defined(_MSC_VER)
                    tsiz = _snprintf(tbuf, sizeof(tbuf), cbuf, va_arg(ap, double));
#else
                    tsiz = snprintf(tbuf, sizeof(tbuf), cbuf, va_arg(ap, double));
#endif
                }

                if (tsiz > sizeof(tbuf)) {
                    tbuf[sizeof(tbuf)-1] = '*';
                    tsiz = sizeof(tbuf);
                }

                dest->append(tbuf, tsiz);
                break;
            }

            case 'p': {
                char tbuf[NUMBUFSIZ*4];
                size_t tsiz = std::sprintf(tbuf, "%p", va_arg(ap, void*));
                dest->append(tbuf, tsiz);
                break;
            }

            case '%': {
                dest->append("%", 1);
                break;
            }
            }
        }
        else {
            dest->append(format, 1);
        }

        format++;
    }
}

void StringUtil::printf(std::string* dest, const char* format, ...) {
    BOOST_ASSERT(dest && format);
    va_list ap;
    va_start(ap, format);
    vstrprintf(dest, format, ap);
    va_end(ap);
}

std::string StringUtil::printf(const char* format, ...) {
    BOOST_ASSERT(format);
    std::string str;
    va_list ap;
    va_start(ap, format);
    vstrprintf(&str, format, ap);
    va_end(ap);
    return str;
}

size_t StringUtil::split(const std::string& str,
                         char delim,
                         std::vector<std::string>* elems) {
    assert(elems);
    elems->clear();
    std::string::const_iterator it = str.begin();
    std::string::const_iterator pv = it;

    while (it != str.end()) {
        if (*it == delim) {
            std::string col(pv, it);
            elems->push_back(col);
            pv = it + 1;
        }

        ++it;
    }

    std::string col(pv, it);
    elems->push_back(col);
    return elems->size();
}

size_t StringUtil::split(const std::string& str,
                         const std::string& delims,
                         std::vector<std::string>* elems) {
    assert(elems);
    elems->clear();
    std::string::const_iterator it = str.begin();
    std::string::const_iterator pv = it;

    while (it != str.end()) {
        while (delims.find(*it, 0) != std::string::npos) {
            std::string col(pv, it);
            elems->push_back(col);
            pv = it + 1;
            break;
        }

        ++it;
    }

    std::string col(pv, it);
    elems->push_back(col);
    return elems->size();
}

std::string* StringUtil::toUpper(std::string* str) {
    BOOST_ASSERT(str);

    if (NULL == str) { return NULL; }

    size_t size = str->size();

    for (size_t i = 0; i < size; i++) {
        int32_t c = (unsigned char)(*str)[i];

        if (c >= 'a' && c <= 'z') { (*str)[i] = c - ('a' - 'A'); }
    }

    return str;
}

std::string* StringUtil::toLower(std::string* str) {
    BOOST_ASSERT(str);

    if (NULL == str) { return NULL; }

    size_t size = str->size();

    for (size_t i = 0; i < size; i++) {
        int32_t c = (unsigned char)(*str)[i];

        if (c >= 'A' && c <= 'Z') { (*str)[i] = c + ('a' - 'A'); }
    }

    return str;
}

template <typename Iterator> inline
void joinStringsIterator(const Iterator& start,
                         const Iterator& end,
                         const char* delim,
                         std::string* result) {
    BOOST_ASSERT(result != NULL);
    result->clear();
    int delim_length = strlen(delim);

    // Precompute resulting length so we can reserve() memory in one shot.
    int length = 0;

    for (Iterator iter = start; iter != end; ++iter) {
        if (iter != start) {
            length += delim_length;
        }

        length += iter->size();
    }

    result->reserve(length);

    // Now combine everything.
    for (Iterator iter = start; iter != end; ++iter) {
        if (iter != start) {
            result->append(delim, delim_length);
        }

        result->append(iter->data(), iter->size());
    }
}

std::string* StringUtil::join(const std::vector<std::string>& components,
                              const char* delim,
                              std::string* result) {
    joinStringsIterator(components.begin(), components.end(), delim, result);
    return result;
}

std::string StringUtil::join(const std::vector<std::string>& components,
                             const char* delim) {
    std::string result;
    joinStringsIterator(components.begin(), components.end(), delim, &result);
    return result;
}

// ----------------------------------------------------------------------
// strto32()
// strtou32()
// strto64()
// strtou64()
//    Architecture-neutral plug compatible replacements for strtol() and
//    strtoul().  Long's have different lengths on ILP-32 and LP-64
//    platforms, so using these is safer, from the point of view of
//    overflow behavior, than using the standard libc functions.
// ----------------------------------------------------------------------
int32_t strto32_adaptor(const char* nptr, char** endptr, int base) {
    const int saved_errno = errno;
    errno = 0;
    const long result = strtol(nptr, endptr, base);

    if (errno == ERANGE && result == LONG_MIN) {
        return MIN_INT32;
    }
    else if (errno == ERANGE && result == LONG_MAX) {
        return MAX_INT32;
    }
    else if (errno == 0 && result < MIN_INT32) {
        errno = ERANGE;
        return MIN_INT32;
    }
    else if (errno == 0 && result > MAX_INT32) {
        errno = ERANGE;
        return MAX_INT32;
    }

    if (errno == 0) {
        errno = saved_errno;
    }

    return static_cast<int32_t>(result);
}
uint32_t strtou32_adaptor(const char* nptr, char** endptr, int base) {
    const int saved_errno = errno;
    errno = 0;
    const unsigned long result = strtoul(nptr, endptr, base);

    if (errno == ERANGE && result == ULONG_MAX) {
        return MAX_UINT32;
    }
    else if (errno == 0 && result > MAX_UINT32) {
        errno = ERANGE;
        return MAX_UINT32;
    }

    if (errno == 0) {
        errno = saved_errno;
    }

    return static_cast<uint32_t>(result);
}

template<typename IntT> inline
IntT strtoint(const char* str) {
    BOOST_ASSERT(str);

    while (*str > '\0' && *str <= ' ') {
        str++;
    }

    int32_t sign = 1;
    IntT num = 0;

    if (*str == '-') {
        str++;
        sign = -1;
    }
    else if (*str == '+') {
        str++;
    }

    while (*str != '\0') {
        if (*str < '0' || *str > '9') { break; }

        num = num * 10 + *str - '0';
        str++;
    }

    return num * sign;
}

template<typename IntT> inline
IntT strtoint(const StringPiece& str) {
    BOOST_ASSERT(!str.empty());

    int32_t sign = 1;
    IntT num = 0;

    for (int i = 0, j = str.length(); i < j; ++i) {
        char c = str[i];

        if (c > '\0' && c <= ' ') { continue; }

        if (c == '-') {
            sign = -1;
            continue;
        }
        else if (c == '+') {
            continue;
        }

        if (c < '0' || c > '9') { break; }

        num = num * 10 + c - '0';
    }

    return num * sign;
}

double StringUtil::strtof(const char* str) {
    BOOST_ASSERT(str);

    while (*str > '\0' && *str <= ' ') {
        str++;
    }

    int32_t sign = 1;

    if (*str == '-') {
        str++;
        sign = -1;
    }
    else if (*str == '+') {
        str++;
    }

    if ((str[0] == 'i' || str[0] == 'I') && (str[1] == 'n' || str[1] == 'N') &&
            (str[2] == 'f' || str[2] == 'F')) {
        return HUGE_VAL * sign;
    }

    if ((str[0] == 'n' || str[0] == 'N') && (str[1] == 'a' || str[1] == 'A') &&
            (str[2] == 'n' || str[2] == 'N')) {
        return std::numeric_limits<double>::quiet_NaN();
    }

    long double num = 0;
    int32_t col = 0;

    while (*str != '\0') {
        if (*str < '0' || *str > '9') { break; }

        num = num * 10 + *str - '0';
        str++;

        if (num > 0) { col++; }
    }

    if (*str == '.') {
        str++;
        long double fract = 0.0;
        long double base = 10;

        while (col < 16 && *str != '\0') {
            if (*str < '0' || *str > '9') { break; }

            fract += (*str - '0') / base;
            str++;
            col++;
            base *= 10;
        }

        num += fract;
    }

    if (*str == 'e' || *str == 'E') {
        str++;
        num *= std::pow((long double)10, (long double)atoi(str));
    }

    return num * sign;
}

inline uint32_t strtou32(const char* nptr, char** endptr, int base) {
    if (sizeof(uint32_t) == sizeof(unsigned long)) {
        return strtoul(nptr, endptr, base);
    }
    else {
        return strtou32_adaptor(nptr, endptr, base);
    }
}

inline uint64_t strtou64(const char* nptr, char** endptr, int base) {
    BOOST_STATIC_ASSERT(sizeof(uint64_t) == sizeof(unsigned long long));
    return strtoull(nptr, endptr, base);
}

int32_t StringUtil::strto32(const char* str) {
    return strtoint<int32_t>(str);
}

int32_t StringUtil::strto32(const std::string& str) {
    return strtoint<int32_t>(str.c_str());
}

int32_t StringUtil::strto32(const StringPiece& str) {
    return strtoint<int32_t>(str);
}

int32_t StringUtil::strto32(const char* str, int base) {
    if (sizeof(int32_t) == sizeof(long)) {
        return strtol(str, NULL, base);
    }
    else {
        return strto32_adaptor(str, NULL, base);
    }
}

int32_t StringUtil::strto32(const std::string& str, int base) {
    return strto32(str.c_str(), base);
}

int32_t StringUtil::strto32(const StringPiece& str, int base) {
    return strto32(str.c_str(), base);
}

int64_t StringUtil::strto64(const char* str) {
    return strtoint<int64_t>(str);
}

int64_t StringUtil::strto64(const std::string& str) {
    return strtoint<int64_t>(str.c_str());
}

int64_t StringUtil::strto64(const StringPiece& str) {
    return strtoint<int64_t>(str);
}

// For now, long long is 64-bit on all the platforms we care about, so these
// functions can simply pass the call to strto[u]ll.
int64_t StringUtil::strto64(const char* str, int base) {
    BOOST_STATIC_ASSERT(sizeof(int64_t) == sizeof(long long));
    return strtoll(str, NULL, base);
}

int64_t StringUtil::strto64(const std::string& str, int base) {
    return strto64(str.c_str(), base);
}

int64_t StringUtil::strto64(const StringPiece& str, int base) {
    return strto64(str.c_str(), base);
}

// ----------------------------------------------------------------------
// FastIntToBuffer()
// FastHexToBuffer()
// FastHex64ToBuffer()
// FastHex32ToBuffer()
// FastTimeToBuffer()
//    These are intended for speed.  FastIntToBuffer() assumes the
//    integer is non-negative.  FastHexToBuffer() puts output in
//    hex rather than decimal.  FastTimeToBuffer() puts the output
//    into RFC822 format.
//
//    FastHex64ToBuffer() puts a 64-bit unsigned value in hex-format,
//    padded to exactly 16 bytes (plus one byte for '\0')
//
//    FastHex32ToBuffer() puts a 32-bit unsigned value in hex-format,
//    padded to exactly 8 bytes (plus one byte for '\0')
//
//       All functions take the output buffer as an arg.
//    They all return a pointer to the beginning of the output,
//    which may not be the beginning of the input buffer.
// ----------------------------------------------------------------------

// Suggested buffer size for FastToBuffer functions.  Also works with
// DoubleToBuffer() and FloatToBuffer().
static const int FAST_TO_BUFFER_SIZE = 32;

// Offset into buffer where FastInt32ToBuffer places the end of string
// null character.  Also used by FastInt32ToBufferLeft
static const int FAST_INT32_TO_BUFFER_OFFSET = 11;

// Offset into buffer where FastInt64ToBuffer places the end of string
// null character.  Also used by FastInt64ToBufferLeft.
static const int FAST_INT64_TO_BUFFER_OFFSET = 21;

// Yes, this is a duplicate of FastInt64ToBuffer.  But, we need this for the
// compiler to generate 32 bit arithmetic instructions.  It's much faster, at
// least with 32 bit binaries.
inline char* fastInt32ToBuffer(int32_t i, char* buffer) {
    // We could collapse the positive and negative sections, but that
    // would be slightly slower for positive numbers...
    // 12 bytes is enough to store -2**32, -4294967296.
    char* p = buffer + FAST_INT32_TO_BUFFER_OFFSET;
    *p-- = '\0';

    if (i >= 0) {
        do {
            *p-- = '0' + i % 10;
            i /= 10;
        }
        while (i > 0);

        return p + 1;
    }
    else {
        // On different platforms, % and / have different behaviors for
        // negative numbers, so we need to jump through hoops to make sure
        // we don't divide negative numbers.
        if (i > -10) {
            i = -i;
            *p-- = '0' + i;
            *p = '-';
            return p;
        }
        else {
            // Make sure we aren't at MIN_INT, in which case we can't say i = -i
            i = i + 10;
            i = -i;
            *p-- = '0' + i % 10;
            // Undo what we did a moment ago
            i = i / 10 + 1;

            do {
                *p-- = '0' + i % 10;
                i /= 10;
            }
            while (i > 0);

            *p = '-';
            return p;
        }
    }
}

inline char* fastInt64ToBuffer(int64_t i, char* buffer) {
    // We could collapse the positive and negative sections, but that
    // would be slightly slower for positive numbers...
    // 22 bytes is enough to store -2**64, -18446744073709551616.
    char* p = buffer + FAST_INT64_TO_BUFFER_OFFSET;
    *p-- = '\0';

    if (i >= 0) {
        do {
            *p-- = '0' + i % 10;
            i /= 10;
        }
        while (i > 0);

        return p + 1;
    }
    else {
        // On different platforms, % and / have different behaviors for
        // negative numbers, so we need to jump through hoops to make sure
        // we don't divide negative numbers.
        if (i > -10) {
            i = -i;
            *p-- = (char)('0' + i);
            *p = '-';
            return p;
        }
        else {
            // Make sure we aren't at MIN_INT, in which case we can't say i = -i
            i = i + 10;
            i = -i;
            *p-- = '0' + i % 10;
            // Undo what we did a moment ago
            i = i / 10 + 1;

            do {
                *p-- = '0' + i % 10;
                i /= 10;
            }
            while (i > 0);

            *p = '-';
            return p;
        }
    }
}

static const char two_ASCII_digits[100][2] = {
    {'0','0'}, {'0','1'}, {'0','2'}, {'0','3'}, {'0','4'},
    {'0','5'}, {'0','6'}, {'0','7'}, {'0','8'}, {'0','9'},
    {'1','0'}, {'1','1'}, {'1','2'}, {'1','3'}, {'1','4'},
    {'1','5'}, {'1','6'}, {'1','7'}, {'1','8'}, {'1','9'},
    {'2','0'}, {'2','1'}, {'2','2'}, {'2','3'}, {'2','4'},
    {'2','5'}, {'2','6'}, {'2','7'}, {'2','8'}, {'2','9'},
    {'3','0'}, {'3','1'}, {'3','2'}, {'3','3'}, {'3','4'},
    {'3','5'}, {'3','6'}, {'3','7'}, {'3','8'}, {'3','9'},
    {'4','0'}, {'4','1'}, {'4','2'}, {'4','3'}, {'4','4'},
    {'4','5'}, {'4','6'}, {'4','7'}, {'4','8'}, {'4','9'},
    {'5','0'}, {'5','1'}, {'5','2'}, {'5','3'}, {'5','4'},
    {'5','5'}, {'5','6'}, {'5','7'}, {'5','8'}, {'5','9'},
    {'6','0'}, {'6','1'}, {'6','2'}, {'6','3'}, {'6','4'},
    {'6','5'}, {'6','6'}, {'6','7'}, {'6','8'}, {'6','9'},
    {'7','0'}, {'7','1'}, {'7','2'}, {'7','3'}, {'7','4'},
    {'7','5'}, {'7','6'}, {'7','7'}, {'7','8'}, {'7','9'},
    {'8','0'}, {'8','1'}, {'8','2'}, {'8','3'}, {'8','4'},
    {'8','5'}, {'8','6'}, {'8','7'}, {'8','8'}, {'8','9'},
    {'9','0'}, {'9','1'}, {'9','2'}, {'9','3'}, {'9','4'},
    {'9','5'}, {'9','6'}, {'9','7'}, {'9','8'}, {'9','9'}
};

inline char* fastUInt32ToBufferLeft(uint32_t u, char* buffer) {
    int digits;
    const char* ASCII_digits = NULL;

    // The idea of this implementation is to trim the number of divides to as few
    // as possible by using multiplication and subtraction rather than mod (%),
    // and by outputting two digits at a time rather than one.
    // The huge-number case is first, in the hopes that the compiler will output
    // that case in one branch-free block of code, and only output conditional
    // branches into it from below.
    if (u >= 1000000000) {  // >= 1,000,000,000
        digits = u / 100000000;  // 100,000,000
        ASCII_digits = two_ASCII_digits[digits];
        buffer[0] = ASCII_digits[0];
        buffer[1] = ASCII_digits[1];
        buffer += 2;
sublt100_000_000:
        u -= digits * 100000000;  // 100,000,000
lt100_000_000:
        digits = u / 1000000;  // 1,000,000
        ASCII_digits = two_ASCII_digits[digits];
        buffer[0] = ASCII_digits[0];
        buffer[1] = ASCII_digits[1];
        buffer += 2;
sublt1_000_000:
        u -= digits * 1000000;  // 1,000,000
lt1_000_000:
        digits = u / 10000;  // 10,000
        ASCII_digits = two_ASCII_digits[digits];
        buffer[0] = ASCII_digits[0];
        buffer[1] = ASCII_digits[1];
        buffer += 2;
sublt10_000:
        u -= digits * 10000;  // 10,000
lt10_000:
        digits = u / 100;
        ASCII_digits = two_ASCII_digits[digits];
        buffer[0] = ASCII_digits[0];
        buffer[1] = ASCII_digits[1];
        buffer += 2;
sublt100:
        u -= digits * 100;
lt100:
        digits = u;
        ASCII_digits = two_ASCII_digits[digits];
        buffer[0] = ASCII_digits[0];
        buffer[1] = ASCII_digits[1];
        buffer += 2;
done:
        *buffer = 0;
        return buffer;
    }

    if (u < 100) {
        digits = u;

        if (u >= 10) { goto lt100; }

        *buffer++ = '0' + digits;
        goto done;
    }

    if (u  <  10000) {   // 10,000
        if (u >= 1000) { goto lt10_000; }

        digits = u / 100;
        *buffer++ = '0' + digits;
        goto sublt100;
    }

    if (u  <  1000000) {   // 1,000,000
        if (u >= 100000) { goto lt1_000_000; }

        digits = u / 10000;  //    10,000
        *buffer++ = '0' + digits;
        goto sublt10_000;
    }

    if (u  <  100000000) {   // 100,000,000
        if (u >= 10000000) { goto lt100_000_000; }

        digits = u / 1000000;  //   1,000,000
        *buffer++ = '0' + digits;
        goto sublt1_000_000;
    }

    // we already know that u < 1,000,000,000
    digits = u / 100000000;   // 100,000,000
    *buffer++ = '0' + digits;
    goto sublt100_000_000;
}

char* fastUInt64ToBufferLeft(uint64_t u64, char* buffer) {
    int digits;
    const char* ASCII_digits = NULL;

    uint32_t u = static_cast<uint32_t>(u64);

    if (u == u64) { return fastUInt32ToBufferLeft(u, buffer); }

    uint64_t top_11_digits = u64 / 1000000000;
    buffer = fastUInt64ToBufferLeft(top_11_digits, buffer);
    u = (uint32_t)(u64 - (top_11_digits * 1000000000));

    digits = u / 10000000;  // 10,000,000
    //GOOGLE_DCHECK_LT(digits, 100);
    BOOST_ASSERT(digits < 100);

    ASCII_digits = two_ASCII_digits[digits];
    buffer[0] = ASCII_digits[0];
    buffer[1] = ASCII_digits[1];
    buffer += 2;
    u -= digits * 10000000;  // 10,000,000
    digits = u / 100000;  // 100,000
    ASCII_digits = two_ASCII_digits[digits];
    buffer[0] = ASCII_digits[0];
    buffer[1] = ASCII_digits[1];
    buffer += 2;
    u -= digits * 100000;  // 100,000
    digits = u / 1000;  // 1,000
    ASCII_digits = two_ASCII_digits[digits];
    buffer[0] = ASCII_digits[0];
    buffer[1] = ASCII_digits[1];
    buffer += 2;
    u -= digits * 1000;  // 1,000
    digits = u / 10;
    ASCII_digits = two_ASCII_digits[digits];
    buffer[0] = ASCII_digits[0];
    buffer[1] = ASCII_digits[1];
    buffer += 2;
    u -= digits * 10;
    digits = u;
    *buffer++ = '0' + digits;
    *buffer = 0;
    return buffer;
}

inline char* fastInt32ToBufferLeft(int32_t i, char* buffer) {
    uint32_t u = i;

    if (i < 0) {
        *buffer++ = '-';
        u = -i;
    }

    return fastUInt32ToBufferLeft(u, buffer);
}

inline char* fastInt64ToBufferLeft(int64_t i, char* buffer) {
    uint64_t u = i;

    if (i < 0) {
        *buffer++ = '-';
        u = -i;
    }

    return fastUInt64ToBufferLeft(u, buffer);
}

char* fastHexToBuffer(int i, char* buffer) {
    //GOOGLE_CHECK(i >= 0) << "FastHexToBuffer() wants non-negative integers, not " << i;

    static const char* hexdigits = "0123456789abcdef";
    char* p = buffer + 21;
    *p-- = '\0';

    do {
        *p-- = hexdigits[i & 15];   // mod by 16
        i >>= 4;                    // divide by 16
    }
    while (i > 0);

    return p + 1;
}

char* fastHexToBuffer(uint64_t value, char* buffer, int numBytes) {
    static const char* hexdigits = "0123456789abcdef";
    buffer[numBytes] = '\0';

    for (int i = numBytes - 1; i >= 0; i--) {
        buffer[i] = hexdigits[uint32_t(value) & 0xf];
        value >>= 4;
    }

    return buffer;
}

// In practice, doubles should never need more than 24 bytes and floats
// should never need more than 14 (including null terminators), but we
// overestimate to be safe.
static const int DOUBLE_TO_BUFFER_SIZE = 32;
static const int FLOAT_TO_BUFFER_SIZE = 24;

// ----------------------------------------------------------------------
// SimpleDtoa()
// SimpleFtoa()
// DoubleToBuffer()
// FloatToBuffer()
//    We want to print the value without losing precision, but we also do
//    not want to print more digits than necessary.  This turns out to be
//    trickier than it sounds.  Numbers like 0.2 cannot be represented
//    exactly in binary.  If we print 0.2 with a very large precision,
//    e.g. "%.50g", we get "0.2000000000000000111022302462515654042363167".
//    On the other hand, if we set the precision too low, we lose
//    significant digits when printing numbers that actually need them.
//    It turns out there is no precision value that does the right thing
//    for all numbers.
//
//    Our strategy is to first try printing with a precision that is never
//    over-precise, then parse the result with strtod() to see if it
//    matches.  If not, we print again with a precision that will always
//    give a precise result, but may use more digits than necessary.
//
//    An arguably better strategy would be to use the algorithm described
//    in "How to Print Floating-Point Numbers Accurately" by Steele &
//    White, e.g. as implemented by David M. Gay's dtoa().  It turns out,
//    however, that the following implementation is about as fast as
//    DMG's code.  Furthermore, DMG's code locks mutexes, which means it
//    will not scale well on multi-core machines.  DMG's code is slightly
//    more accurate (in that it will never use more digits than
//    necessary), but this is probably irrelevant for most users.
//
//    Rob Pike and Ken Thompson also have an implementation of dtoa() in
//    third_party/fmt/fltfmt.cc.  Their implementation is similar to this
//    one in that it makes guesses and then uses strtod() to check them.
//    Their implementation is faster because they use their own code to
//    generate the digits in the first place rather than use snprintf(),
//    thus avoiding format string parsing overhead.  However, this makes
//    it considerably more complicated than the following implementation,
//    and it is embedded in a larger library.  If speed turns out to be
//    an issue, we could re-implement this in terms of their
//    implementation.
// ----------------------------------------------------------------------
static inline bool isValidFloatChar(char c) {
    return ('0' <= c && c <= '9') ||
           c == 'e' || c == 'E' ||
           c == '+' || c == '-';
}

void delocalizeRadix(char* buffer) {
    // Fast check:  if the buffer has a normal decimal point, assume no
    // translation is needed.
    if (strchr(buffer, '.') != NULL) { return; }

    // Find the first unknown character.
    while (isValidFloatChar(*buffer)) { ++buffer; }

    if (*buffer == '\0') {
        // No radix character found.
        return;
    }

    // We are now pointing at the locale-specific radix character.  Replace it
    // with '.'.
    *buffer = '.';
    ++buffer;

    if (!isValidFloatChar(*buffer) && *buffer != '\0') {
        // It appears the radix was a multi-byte character.  We need to remove the
        // extra bytes.
        char* target = buffer;

        do { ++buffer; }
        while (!isValidFloatChar(*buffer) && *buffer != '\0');

        memmove(target, buffer, strlen(buffer) + 1);
    }
}

inline bool isNaN(double value) {
    // NaN is never equal to anything, even itself.
    return value != value;
}

char* doubleToBuffer(double value, char* buffer) {
    // DBL_DIG is 15 for IEEE-754 doubles, which are used on almost all
    // platforms these days.  Just in case some system exists where DBL_DIG
    // is significantly larger -- and risks overflowing our buffer -- we have
    // this assert.
    BOOST_STATIC_ASSERT(DBL_DIG < 20);

    if (value == std::numeric_limits<double>::infinity()) {
        strcpy(buffer, "inf");
        return buffer;
    }
    else if (value == -std::numeric_limits<double>::infinity()) {
        strcpy(buffer, "-inf");
        return buffer;
    }
    else if (isNaN(value)) {
        strcpy(buffer, "nan");
        return buffer;
    }

    int snprintf_result =
        snprintf(buffer, DOUBLE_TO_BUFFER_SIZE, "%.*g", DBL_DIG, value);

    // The snprintf should never overflow because the buffer is significantly
    // larger than the precision we asked for.
    BOOST_ASSERT(snprintf_result > 0 && snprintf_result < DOUBLE_TO_BUFFER_SIZE);

    // We need to make parsed_value volatile in order to force the compiler to
    // write it out to the stack.  Otherwise, it may keep the value in a
    // register, and if it does that, it may keep it as a long double instead
    // of a double.  This long double may have extra bits that make it compare
    // unequal to "value" even though it would be exactly equal if it were
    // truncated to a double.
    volatile double parsed_value = strtod(buffer, NULL);

    if (parsed_value != value) {
        int snprintf_result =
            snprintf(buffer, DOUBLE_TO_BUFFER_SIZE, "%.*g", DBL_DIG+2, value);

        // Should never overflow; see above.
        BOOST_ASSERT(snprintf_result > 0 && snprintf_result < DOUBLE_TO_BUFFER_SIZE);
    }

    delocalizeRadix(buffer);
    return buffer;
}

bool safe_strtof(const char* str, float* value) {
    char* endptr;
    errno = 0;  // errno only gets set on errors
#if defined(_WIN32) || defined (__hpux)  // has no strtof()
    *value = (float)strtod(str, &endptr);
#else
    *value = strtof(str, &endptr);
#endif
    return *str != 0 && *endptr == 0 && errno == 0;
}

char* floatToBuffer(float value, char* buffer) {
    // FLT_DIG is 6 for IEEE-754 floats, which are used on almost all
    // platforms these days.  Just in case some system exists where FLT_DIG
    // is significantly larger -- and risks overflowing our buffer -- we have
    // this assert.
    BOOST_STATIC_ASSERT(FLT_DIG < 10);

    if (value == std::numeric_limits<double>::infinity()) {
        strcpy(buffer, "inf");
        return buffer;
    }
    else if (value == -std::numeric_limits<double>::infinity()) {
        strcpy(buffer, "-inf");
        return buffer;
    }
    else if (isNaN(value)) {
        strcpy(buffer, "nan");
        return buffer;
    }

    int snprintf_result =
        snprintf(buffer, FLOAT_TO_BUFFER_SIZE, "%.*g", FLT_DIG, value);

    // The snprintf should never overflow because the buffer is significantly
    // larger than the precision we asked for.
    BOOST_ASSERT(snprintf_result > 0 && snprintf_result < FLOAT_TO_BUFFER_SIZE);

    float parsed_value;

    if (!safe_strtof(buffer, &parsed_value) || parsed_value != value) {
        int snprintf_result =
            snprintf(buffer, FLOAT_TO_BUFFER_SIZE, "%.*g", FLT_DIG+2, value);

        // Should never overflow; see above.
        BOOST_ASSERT(snprintf_result > 0 && snprintf_result < FLOAT_TO_BUFFER_SIZE);
    }

    delocalizeRadix(buffer);
    return buffer;
}

std::string StringUtil::numtostr(int number) {
    char buffer[FAST_TO_BUFFER_SIZE];
    return (sizeof(int) == 4) ?
           fastInt32ToBuffer(number, buffer) :
           fastInt64ToBuffer(number, buffer);
}

std::string* StringUtil::numtostr(int number, std::string* str) {
    StringUtil::printf(str, "%d", number);
    return str;
}

std::string StringUtil::numtostr(unsigned int number) {
    char buffer[FAST_TO_BUFFER_SIZE];
    return (sizeof(unsigned int) == 4) ?
           fastUInt32ToBufferLeft(number, buffer) :
           fastUInt64ToBufferLeft(number, buffer);
}

std::string* StringUtil::numtostr(unsigned int number, std::string* str) {
    StringUtil::printf(str, "%ud", number);
    return str;
}

std::string StringUtil::numtostr(long number) {
    char buffer[FAST_TO_BUFFER_SIZE];
    return (sizeof(long) == 4) ?
           fastInt32ToBuffer(number, buffer) :
           fastInt64ToBuffer(number, buffer);
}

std::string* StringUtil::numtostr(long number, std::string* str) {
    StringUtil::printf(str, "%ld", number);
    return str;
}

std::string StringUtil::numtostr(unsigned long number) {
    char buffer[FAST_TO_BUFFER_SIZE];
    return (sizeof(unsigned long) == 4) ?
           fastUInt32ToBufferLeft(number, buffer) :
           fastUInt64ToBufferLeft(number, buffer);
}

std::string* StringUtil::numtostr(unsigned long number, std::string* str) {
    StringUtil::printf(str, "%uld", number);
    return str;
}

std::string StringUtil::numtostr(long long number) {
    char buffer[FAST_TO_BUFFER_SIZE];
    return (sizeof(long long) == 4) ?
           fastInt32ToBuffer(static_cast<int32_t>(number), buffer) :
           fastInt64ToBuffer(number, buffer);
}

std::string* StringUtil::numtostr(long long number, std::string* str) {
    StringUtil::printf(str, "%lld", number);
    return str;
}

std::string StringUtil::numtostr(unsigned long long number) {
    char buffer[FAST_TO_BUFFER_SIZE];
    return (sizeof(unsigned long long) == 4) ?
           fastUInt32ToBufferLeft(static_cast<uint32_t>(number), buffer) :
           fastUInt64ToBufferLeft(number, buffer);
}

std::string* StringUtil::numtostr(unsigned long long number, std::string* str) {
    StringUtil::printf(str, "%ulld", number);
    return str;
}

std::string StringUtil::numtostr(float number) {
    char buffer[FLOAT_TO_BUFFER_SIZE];
    return floatToBuffer(number, buffer);
}

std::string* StringUtil::numtostr(float number, std::string* str) {
    StringUtil::printf(str, "%f", number);
    return str;
}

std::string StringUtil::numtostr(double number) {
    char buffer[DOUBLE_TO_BUFFER_SIZE];
    return doubleToBuffer(number, buffer);
}

std::string* StringUtil::numtostr(double number, std::string* str) {
    StringUtil::printf(str, "%f", number);
    return str;
}

std::string StringUtil::hextostr(uint32_t hex) {
    char buffer[FAST_INT32_TO_BUFFER_OFFSET];
    return fastHexToBuffer(hex, buffer, 8);
}

std::string* StringUtil::hextostr(uint32_t hex, std::string* str) {
    std::size_t oldSize = str->size();
    STLStringResizeUninitialized(str, oldSize + 8);

    char* start = stringAsArray(str) + oldSize;
    fastHexToBuffer(hex, start, 8);

    return str;
}

std::string StringUtil::hextostr(uint64_t hex) {
    char buffer[FAST_INT64_TO_BUFFER_OFFSET];
    return fastHexToBuffer(hex, buffer, 16);
}

std::string* StringUtil::hextostr(uint64_t hex, std::string* str) {
    std::size_t oldSize = str->size();
    STLStringResizeUninitialized(str, oldSize + 16);

    char* start = stringAsArray(str) + oldSize;
    fastHexToBuffer(hex, start, 16);

    return str;
}

namespace detail {
typedef unsigned short ucs2_t;  /* Unicode character [D5] */
typedef unsigned int   ucs4_t;  /* Unicode scalar character [D28] */
typedef ucs4_t    ucs_t;

const static int UCS_CHAR_INVALID = 0xFFFE;
const static int UCS_CHAR_NONE = 0xFFFF;

int convertToUcs(const unsigned char** inbuf, std::size_t* inbytesleft) {
    const unsigned char* in = *inbuf;
    unsigned char byte = *in++;
    ucs_t res = byte;

    if (byte >= 0xC0) {
        if (byte < 0xE0) {
            if (*inbytesleft < 2) {
                return UCS_CHAR_NONE;
            }

            res = (*in & 0xC0) == 0x80 ?
                  ((byte & 0x1F) << 6) | (*in++ & 0x3F) : UCS_CHAR_INVALID;
        }
        else if (byte < 0xF0) {
            if (*inbytesleft < 3) { return UCS_CHAR_NONE; }

            if (((in[0] & 0xC0) == 0x80) && ((in[1] & 0xC0) == 0x80)) {
                res = ((byte & 0x0F) << 12) | ((in[0] & 0x3F) << 6)
                      | (in[1] & 0x3F);
                in += 2;
            }
            else {
                res = UCS_CHAR_INVALID;
            }
        }
        else if (byte <= 0xF4) {
            if (*inbytesleft < 4) { return UCS_CHAR_NONE; }

            if (((byte == 0xF4 && ((in[0] & 0xF0) == 0x80))
                    || ((in[0] & 0xC0) == 0x80))
                    && ((in[1] & 0xC0) == 0x80)
                    && ((in[2] & 0xC0) == 0x80)) {
                res = ((byte & 0x7) << 18) | ((in[0] & 0x3F) << 12)
                      | ((in[1] & 0x3F) << 6) | (in[2] & 0x3F);
                in += 3;
            }
            else {
                res = UCS_CHAR_INVALID;
            }
        }
        else {
            res = UCS_CHAR_INVALID;
        }
    }
    else if (byte & 0x80) {
        res = UCS_CHAR_INVALID;
    }

    *inbytesleft -= (in - *inbuf);
    *inbuf = in;
    return res;
}
}

void StringUtil::utftoucs(const std::string& src, std::wstring* dest) {
    std::size_t inSize = src.size();
    const unsigned char* inBuf =
        reinterpret_cast<const unsigned char*>(src.c_str());

    while (inSize > 0) {
        int byte = detail::convertToUcs(&inBuf, &inSize);

        if (byte == detail::UCS_CHAR_INVALID) { return; }

        if (byte == detail::UCS_CHAR_NONE) { break; }

        dest->push_back(static_cast<wchar_t>(byte));
    }
}

void StringUtil::utftoucs(const std::string& src, std::vector<uint32_t>* dest) {

}

void StringUtil::replace(const std::string& s,
                         const std::string& oldsub,
                         const std::string& newsub,
                         bool replaceAll,
                         std::string* res) {
    BOOST_ASSERT(res);

    if (oldsub.empty()) {
        res->append(s);  // if empty, append the given string.
        return;
    }

    //std::string* output = (&s == res) ? new std::string : res;

    std::string::size_type startPos = 0;
    std::string::size_type pos;

    do {
        pos = s.find(oldsub, startPos);

        if (pos == std::string::npos) {
            break;
        }

        res->append(s, startPos, pos - startPos);
        res->append(newsub);
        startPos = pos + oldsub.size();  // start searching again after the "old"
    }
    while (replaceAll);

    res->append(s, startPos, s.length() - startPos);
}

void StringUtil::replace(const std::string& s,
                         char oldChar,
                         char newChar,
                         bool replaceAll,
                         std::string* res) {
    BOOST_ASSERT(res);

    if (s.empty()) {
        return;
    }

    if (&s != res) {
        res->assign(s);
    }

    std::string::size_type startPos = 0;
    std::string::size_type pos;

    do {
        pos = res->find(oldChar, startPos);

        if (pos == std::string::npos) {
            break;
        }

        (*res)[pos] = newChar;
    }
    while (replaceAll);
}

std::string* StringUtil::trim(std::string* str) {
    boost::trim(*str);
    return str;
}

}
}
