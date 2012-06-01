
/**
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

#include <cstdarg>
#include <cassert>
#include <string>
#include <ctime>
#include <boost/cstdint.hpp>

#include <cetty/logging/LoggerHelperPrivate.h>

namespace cetty {
namespace logging {
namespace internal {

/** The buffer size for numeric data. */
static const size_t NUMBUFSIZ = 32;

/**
 * Append a formatted string at the end of a string.
 */
static void vstrprintf(std::string* dest, const char* format, va_list ap) {
    assert(dest && format);

    while (*format != '\0') {
        if (*format == '%') {
            char cbuf[NUMBUFSIZ];
            cbuf[0] = '%';
            size_t cbsiz = 1;
            boost::int32_t lnum = 0;
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

/**
 * Append a formatted string at the end of a string.
 */
inline void strprintf(std::string* dest, const char* format, ...) {
    assert(dest && format);
    va_list ap;
    va_start(ap, format);
    vstrprintf(dest, format, ap);
    va_end(ap);
}


class Timer {
public:
    Timer() {
        start = std::clock();
    }
    void restart() {
        start = std::clock();
    }
    int elapsed() const {
        return (std::clock() - start) / (CLOCKS_PER_SEC / 1000);
    }

private:
    std::clock_t start;
};

LoggerWrapper::~LoggerWrapper() {
    if (timer) { delete timer; }
}

void LoggerWrapper::log(InternalLogLevel level, const char* file, int line, const char* format, ...) {
    if (this->logger && this->logger->isEnabled(level)) {
        std::string buffer;
        buffer.reserve(2048);

        va_list ap;
        va_start(ap, format);
        vstrprintf(&buffer, format, ap);
        va_end(ap);

        this->logger->log(level, buffer, file, line);
    }
}

void LoggerWrapper::startTimer(const char* file, int line) {
    if (NULL == timer) {
        timer = new Timer;
    }
    else {
        timer->restart();
    }
}

void LoggerWrapper::checkTimer(const char* file, int line, const char* format, ...) {
    if (NULL == timer || this->logger == NULL || !this->logger->isInfoEnabled()) { return; }

    std::string buffer;
    int elapsed = timer->elapsed();
    timer->restart();

    buffer.reserve(2048);
    strprintf(&buffer, "[TIMER: %d ms] ", elapsed);

    va_list vp;
    va_start(vp, format);
    vstrprintf(&buffer, format, vp);
    va_end(vp);

    this->logger->log(InternalLogLevel::INFO, buffer, file, line);
}

}
}
}