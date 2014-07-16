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

public:
    static Time now() {
        return boost::posix_time::microsec_clock::universal_time();
    }

    static TimeStamp nowStamp() {
        return now() - kEpoch;
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
