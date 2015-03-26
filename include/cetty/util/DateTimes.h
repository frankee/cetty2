#ifndef CETTY_UTIL_DATETIMES_H
#define CETTY_UTIL_DATETIMES_H

/*
 * Copyright (c) 2010-2014 frankee zhou (frankee.zhou at gmail dot com)
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

#include <boost/date_time.hpp>
#include <boost/assert.hpp>
#include <sstream>

namespace cetty {
namespace util {

typedef boost::posix_time::ptime Time;
typedef boost::posix_time::time_duration TimeStamp;
typedef boost::posix_time::time_duration TimeDuration;
typedef boost::posix_time::ptime::date_type Date;

typedef boost::posix_time::hours Hours;
typedef boost::posix_time::minutes Minutes;
typedef boost::posix_time::seconds Seconds;
typedef boost::posix_time::milliseconds Milliseconds;
typedef boost::posix_time::microseconds Microseconds;

class DateTimes {
public:
    static const Time kEpoch;
    static const TimeDuration kEmptyDuration;
    static const std::string kTimeZone;

public:
    static Time now() {
        return boost::posix_time::microsec_clock::universal_time();
    }

    static TimeStamp nowStamp() {
        return now() - kEpoch;
    }

    static std::string nowStr() {
        std::string str = boost::posix_time::to_iso_extended_string(
                              boost::posix_time::second_clock::universal_time());
        str.append(1, 'z');
        return str;
    }

    static Time localNow() {
        return boost::posix_time::second_clock::local_time();
    }

    static std::string localNowStr() {
        std::string str = boost::posix_time::to_iso_extended_string(
            boost::posix_time::second_clock::local_time());
        str += kTimeZone;
        return str;
    }

    static std::string localDateStr() {
        return toDateString(boost::posix_time::second_clock::local_time());
    }

    static Time parseFrom(const std::string& t) {
        BOOST_ASSERT(!t.empty());

        if (t[t.size()-1] != 'z') {
            std::size_t pos = t.find_last_of('+');
            Time time = from_iso_extended_string(t.substr(0, pos));
            time -= boost::posix_time::hours(8);
            return time;
        }
        else {
            return from_iso_extended_string(t);
        }
    }

    static std::string toString(const Time& time) {
        std::string str = boost::posix_time::to_iso_extended_string(time);
        str += kTimeZone;
        return str;
    }

	static std::string toDateString(const Time& time);

private:
	static boost::posix_time::ptime from_iso_extended_string(const std::string& inDateString) {
		// If we get passed a zero length string, then return the "not a date" value. 
		if (inDateString.empty())
		{
			return boost::posix_time::not_a_date_time;
		}

		// Use the ISO extended input facet to interpret the string. 
		std::stringstream ss;
		boost::posix_time::time_input_facet* input_facet = new boost::posix_time::time_input_facet();
		input_facet->set_iso_extended_format();
		ss.imbue(std::locale(ss.getloc(), input_facet));
		ss.str(inDateString);
		boost::posix_time::ptime timeFromString;
		ss >> timeFromString;

		return timeFromString;
	}

private:
    DateTimes();
    ~DateTimes();
};

}

using util::Time;
using util::TimeStamp;
using util::TimeDuration;

using util::Hours;
using util::Minutes;
using util::Seconds;
using util::Milliseconds;
using util::Microseconds;

}

#endif //#ifndef CETTY_UTIL_DATETIMES_H
