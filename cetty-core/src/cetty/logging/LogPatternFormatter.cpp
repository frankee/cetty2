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

    boost::posix_time::ptime timeStamp = msg.getTimeStamp();
    boost::posix_time::ptime::date_type date = timeStamp.date();
    boost::posix_time::time_duration time = timeStamp.time_of_day();

    while (it != end) {
        if (*it == '%') {
            if (++it != end) {
                switch (*it) {
                case 'l': msg.getStream() << msg.getLevel(); break;

                case 'L': msg.getStream() << msg.getLevel().toString(); break;

                case 'p': msg.getStream() << msg.getLevel().toString().at(0); break;

                //case 'P': NumberFormatter::append(text, msg.getPid()); break;

                case 'T': msg.getStream() << msg.getThreadId(); break;

                //case 'N': text.append(Environment::nodeName()); break;

                case 's': msg.getStream() << msg.getSourceFile() ? msg.getSourceFile() : ""; break;

                case 'n': msg.getStream() << msg.getSouceLine(); break;

                case 'f': msg.getStream() << msg.getFunction() ? msg.getFunction() : ""; break;

                case 'w': msg.getStream() << date.day_of_week().as_short_string(); break;

                case 'W': msg.getStream() << date.day_of_week().as_long_string(); break;

                case 'b': msg.getStream() << date.month().as_short_string(); break;

                case 'B': msg.getStream() << date.month().as_long_string(); break;

                case 'd': msg.getStream() << date.day().as_number(); break;

                case 'D': msg.getStream() << date.day().as_number(); break;

                case 'm': msg.getStream() << date.month().as_number(); break;

                case 'O': msg.getStream() << date.month().as_number(); break;

                case 'Y': msg.getStream() << date.year(); break;

                case 'y': msg.getStream() << date.year(); break;

                case 'H': msg.getStream() << time.hours(); break;

                    //case 'h': NumberFormatter::append0(text, dateTime.hourAMPM(), 2); break;

                    //case 'a': text.append(dateTime.isAM() ? "am" : "pm"); break;

                    //case 'A': text.append(dateTime.isAM() ? "AM" : "PM"); break;

                case 'M': msg.getStream() << time.minutes(); break;

                case 'S': msg.getStream() << time.seconds(); break;

                case 'i': msg.getStream() << time.fractional_seconds(); break;

                    //case 'c': NumberFormatter::append(text, dateTime.millisecond()/100); break;

                    //case 'F': NumberFormatter::append0(text, dateTime.millisecond()*1000 + dateTime.microsecond(), 6); break;

                    //case 'z': text.append(DateTimeFormatter::tzdISO(_localTime ? Timezone::tzd() : DateTimeFormatter::UTC)); break;

                case 'Z': msg.getStream() << timeStamp.zone_abbrev(); break;

                case 'E': msg.getStream() << (timeStamp - epoch).total_seconds(); break;
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

                default: msg.getStream() << *it;
                }

                ++it;
            }
        }
        else {
            msg.getStream() << *it++;
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