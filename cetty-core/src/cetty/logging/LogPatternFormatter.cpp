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

#include <cetty/logging/LogPatternFormatter.h>
#include <cetty/logging/LogMessage.h>

namespace cetty {
namespace logging {

static boost::posix_time::ptime epoch
    = boost::posix_time::time_from_string("1970-01-01 00:00:00.000");

LogPatternFormatter::LogPatternFormatter()
    : pattern("%m/%d/%y %H:%M:%S-%i [%L][%T] "), lastPattern(" [%s:%n%f]") {
}

LogPatternFormatter::LogPatternFormatter(const std::string& format) {
    std::string::size_type pos = format.find("%t");

    if (pos != format.npos) {
        pattern = format.substr(0, pos);
        lastPattern = format.substr(pos + 2);
    }
}

void LogPatternFormatter::format(LogMessage& msg, const std::string& pattern) const {
    std::string::const_iterator it  = pattern.begin();
    std::string::const_iterator end = pattern.end();

    boost::posix_time::ptime timeStamp = msg.timeStamp();
    boost::posix_time::ptime::date_type date = timeStamp.date();
    boost::posix_time::time_duration time = timeStamp.time_of_day();

    while (it != end) {
        if (*it == '%') {
            if (++it != end) {
                switch (*it) {
                case 'l': msg.stream() << msg.level().toInt(); break;

                case 'L': msg.stream() << msg.level().toString(); break;

                case 'p': msg.stream() << msg.level().toString().at(0); break;

                //case 'P': NumberFormatter::append(text, msg.getPid()); break;

                case 'T': msg.stream() << msg.threadId(); break;

                //case 'N': text.append(Environment::nodeName()); break;

                case 's': msg.stream() << msg.sourceFile() ? msg.sourceFile() : ""; break;

                case 'n': msg.stream() << msg.souceLine(); break;

                case 'f': msg.stream() << msg.function() ? msg.function() : ""; break;

                case 'w': msg.stream() << date.day_of_week().as_short_string(); break;

                case 'W': msg.stream() << date.day_of_week().as_long_string(); break;

                case 'b': msg.stream() << date.month().as_short_string(); break;

                case 'B': msg.stream() << date.month().as_long_string(); break;

                case 'd': msg.stream() << date.day().as_number(); break;

                case 'D': msg.stream() << date.day().as_number(); break;

                case 'm': msg.stream() << date.month().as_number(); break;

                case 'O': msg.stream() << date.month().as_number(); break;

                case 'Y': msg.stream() << date.year(); break;

                case 'y': msg.stream() << date.year(); break;

                case 'H': msg.stream() << time.hours(); break;

                    //case 'h': NumberFormatter::append0(text, dateTime.hourAMPM(), 2); break;

                    //case 'a': text.append(dateTime.isAM() ? "am" : "pm"); break;

                    //case 'A': text.append(dateTime.isAM() ? "AM" : "PM"); break;

                case 'M': msg.stream() << time.minutes(); break;

                case 'S': msg.stream() << time.seconds(); break;

                case 'i': msg.stream() << time.fractional_seconds(); break;

                    //case 'c': NumberFormatter::append(text, dateTime.millisecond()/100); break;

                    //case 'F': NumberFormatter::append0(text, dateTime.millisecond()*1000 + dateTime.microsecond(), 6); break;

                    //case 'z': text.append(DateTimeFormatter::tzdISO(_localTime ? Timezone::tzd() : DateTimeFormatter::UTC)); break;

                case 'Z': msg.stream() << timeStamp.zone_abbrev(); break;

                case 'E': msg.stream() << (timeStamp - epoch).total_seconds(); break;
#if 0

                case '[': {
                    ++it;
                    std::string prop;

                    while (it != end && *it != ']') { prop += *it++; }

                    if (it == end) { --it; }

                    try {
                        text.append(msg[prop]);
                    }
                    catch (...) {
                    }

                    break;
                }

#endif

                default: msg.stream() << *it;
                }

                ++it;
            }
        }
        else {
            msg.stream() << *it++;
        }
    }
}

void LogPatternFormatter::formatLast(LogMessage& msg) const {
    format(msg, lastPattern);
}

void LogPatternFormatter::formatFirst(LogMessage& msg) const {
    format(msg, pattern);
}

}
}
