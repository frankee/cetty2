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
#include <cetty/util/DateTimes.h>
#include <cetty/util/StringUtil.h>

using namespace boost;

namespace cetty {
namespace util {

const Time DateTimes::kEpoch(boost::posix_time::from_iso_string("19700101T000000"));
const TimeDuration DateTimes::kEmptyDuration = TimeDuration();
const std::string DateTimes::kTimeZone("+08:00");


std::string DateTimes::toDateString( const Time& time ) {
    tm date = posix_time::to_tm(time);
    return StringUtil::printf("%d%02d%02d", 1900+date.tm_year, 1+date.tm_mon, date.tm_mday);
}

}
}
