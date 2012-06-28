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

#include <boost/crc.hpp>
#include <boost/assert.hpp>
#include <cetty/util/StringUtil.h>

namespace cetty {
namespace util {

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
        if (Character::isISOControl(value.at(i))) {
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
        if (!Character::isISOControl(value.at(i))) {
            break;
        }
    }

    // Copy non control characters and substitute control characters with
    // a space.  The last control characters are trimmed.
    bool suppressingControlChars = false;

    for (; i < value.length(); ++i) {
        if (Character::isISOControl(value.at(i))) {
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

void StringUtil::vstrprintf(std::string* dest, const char* format, va_list ap) {
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

void StringUtil::strprintf(std::string* dest, const char* format, ...) {
    BOOST_ASSERT(dest && format);
    va_list ap;
    va_start(ap, format);
    vstrprintf(dest, format, ap);
    va_end(ap);
}

std::string StringUtil::strprintf(const char* format, ...) {
    BOOST_ASSERT(format);
    std::string str;
    va_list ap;
    va_start(ap, format);
    vstrprintf(&str, format, ap);
    va_end(ap);
    return str;
}

size_t StringUtil::strsplit(const std::string& str, char delim, std::vector<std::string>* elems) {
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

size_t StringUtil::strsplit(const std::string& str, const std::string& delims, std::vector<std::string>* elems) {
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

bool StringUtil::strfwm(const std::string& str, const std::string& key) {
    std::size_t ksiz = key.size();
    if (ksiz > str.size()) return false;
    return !std::memcmp(str.data(), key.data(), ksiz);
}

bool StringUtil::strbwm(const std::string& str, const std::string& key) {
    std::size_t ksiz = key.size();
    if (ksiz > str.size()) return false;
    return !std::memcmp(str.data() + str.size() - ksiz, key.data(), ksiz);
}

boost::int64_t StringUtil::atoi(const char* str) {
    BOOST_ASSERT(str);
    while (*str > '\0' && *str <= ' ') {
        str++;
    }
    int32_t sign = 1;
    int64_t num = 0;
    if (*str == '-') {
        str++;
        sign = -1;
    } else if (*str == '+') {
        str++;
    }
    while (*str != '\0') {
        if (*str < '0' || *str > '9') break;
        num = num * 10 + *str - '0';
        str++;
    }
    return num * sign;
}

double StringUtil::atof(const char* str) {
    BOOST_ASSERT(str);
    while (*str > '\0' && *str <= ' ') {
        str++;
    }
    int32_t sign = 1;
    if (*str == '-') {
        str++;
        sign = -1;
    } else if (*str == '+') {
        str++;
    }
    if ((str[0] == 'i' || str[0] == 'I') && (str[1] == 'n' || str[1] == 'N') &&
        (str[2] == 'f' || str[2] == 'F')) return HUGE_VAL * sign;
    if ((str[0] == 'n' || str[0] == 'N') && (str[1] == 'a' || str[1] == 'A') &&
        (str[2] == 'n' || str[2] == 'N')) return nan();
    long double num = 0;
    int32_t col = 0;
    while (*str != '\0') {
        if (*str < '0' || *str > '9') break;
        num = num * 10 + *str - '0';
        str++;
        if (num > 0) col++;
    }
    if (*str == '.') {
        str++;
        long double fract = 0.0;
        long double base = 10;
        while (col < 16 && *str != '\0') {
            if (*str < '0' || *str > '9') break;
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

std::string* StringUtil::strtoupper(std::string* str) {
    BOOST_ASSERT(str);
    if (NULL == str) return NULL;

    size_t size = str->size();
    for (size_t i = 0; i < size; i++) {
        int32_t c = (unsigned char)(*str)[i];
        if (c >= 'a' && c <= 'z') (*str)[i] = c - ('a' - 'A');
    }
    return str;
}

std::string* StringUtil::strtolower(std::string* str) {
    BOOST_ASSERT(str);
    if (NULL == str) return NULL;

    size_t size = str->size();
    for (size_t i = 0; i < size; i++) {
        int32_t c = (unsigned char)(*str)[i];
        if (c >= 'A' && c <= 'Z') (*str)[i] = c + ('a' - 'A');
    }
    return str;
}

}
}