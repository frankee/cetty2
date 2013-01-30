#if !defined(CETTY_LOGGING_LOGPATTERNFORMATTER_H)
#define CETTY_LOGGING_LOGPATTERNFORMATTER_H

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
#include <iostream>

namespace cetty {
namespace logging {

class LogMessage;

/// This Formatter allows for custom formatting of
/// log messages based on format patterns.
///
/// The format pattern is used as a template to format the message and
/// is copied character by character except for the following special characters,
/// which are replaced by the corresponding value.
///
///   * %s - message source
///   * %t - message text
///   * %l - message priority level (1 .. 7)
///   * %L - message priority (Fatal, Critical, Error, Warning, Notice, Information, Debug, Trace)
///   * %p - abbreviated message priority (F, C, E, W, N, I, D, T)
///   * %P - message process identifier
///   * %T - message thread identifier (numeric)
///   * %N - node or host name
///   * %n - message source line number (0 if not set)
///   * %f - message source function (empty string if not set)
///   * %w - message date/time abbreviated weekday (Mon, Tue, ...)
///   * %W - message date/time full weekday (Monday, Tuesday, ...)
///   * %b - message date/time abbreviated month (Jan, Feb, ...)
///   * %B - message date/time full month (January, February, ...)
///   * %d - message date/time zero-padded day of month (01 .. 31)
///   * %D - message date/time day of month (1 .. 31)
///   * %m - message date/time zero-padded month (01 .. 12)
///   * %O - message date/time month (1 .. 12)
///   * %Y - message date/time year without century (70)
///   * %y - message date/time year with century (1970)
///   * %H - message date/time hour (00 .. 23)
///   * %h - message date/time hour (00 .. 12)
///   * %a - message date/time am/pm
///   * %A - message date/time AM/PM
///   * %M - message date/time minute (00 .. 59)
///   * %S - message date/time second (00 .. 59)
///   * %i - message date/time millisecond (000 .. 999)
///   * %z - time zone differential in ISO 8601 format (Z or +NN.NN)
///   * %Z - time zone differential in RFC format (GMT or +NNNN)
///   * %E - epoch time (UTC, seconds since midnight, January 1, 1970)
///   * %[name] - the value of the message parameter with the given name
///   * %% - percent sign
class LogPatternFormatter {
public:
    /// Creates a PatternFormatter using default format pattern.
    LogPatternFormatter();

    /// Creates a PatternFormatter that uses the given format pattern.
    LogPatternFormatter(const std::string& format);

    ~LogPatternFormatter() {}

    /// Formats the message according to the specified
    /// format pattern and places the result in text.
    void formatFirst(LogMessage& msg) const;
    void formatLast(LogMessage& msg) const;

private:
    void format(LogMessage& msg, const std::string& pattern) const;

private:
    std::string pattern;
    std::string lastPattern;
};

}
}

#endif //#if !defined(CETTY_LOGGING_LOGPATTERNFORMATTER_H)

// Local Variables:
// mode: c++
// End:
