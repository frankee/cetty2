#if !defined(CETTY_UTIL_TIMEUNIT_H)
#define CETTY_UTIL_TIMEUNIT_H

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
#include <boost/cstdint.hpp>

#include "cetty/util/Enum.h"

namespace cetty {
namespace util {

class TimeUnit : public Enum<TimeUnit> {
public:
    static const TimeUnit MICROSECONDS;
    static const TimeUnit MILLISECONDS;
    static const TimeUnit NANOSECONDS;
    static const TimeUnit SECONDS;

    /**
     * @param duration - the duration
     *
     * @return the converted duration.
     *
     * @remark if conversion negatively overflow, return MIN_VALUE,
     *         if positively overflow, return MAX_VALUE.
     */
    boost::int64_t toNanos(boost::int64_t duration) const;

    /**
     * @param duration - the duration
     *
     * @return the converted duration.
     *
     * @remark if conversion negatively overflow, return MIN_VALUE,
     *         if positively overflow, return MAX_VALUE.
     */
    boost::int64_t toMicros(boost::int64_t duration) const;

    /**
     * @param duration - the duration
     *
     * @return the converted duration.
     *
     * @remark if conversion negatively overflow, return MIN_VALUE,
     *         if positively overflow, return MAX_VALUE.
     */
    boost::int64_t toMillis(boost::int64_t duration) const;

    /**
     * @param duration - the duration
     *
     * @return the converted duration.
     *
     * @remark if conversion negatively overflow, return MIN_VALUE,
     *         if positively overflow, return MAX_VALUE.
     */
    boost::int64_t toSeconds(boost::int64_t duration) const;

    /**
     * Get a string describing this enum.
     * This method returns the string "MICROSECONDS" for MICROSECONDS,
     * "MILLISECONDS" for MILLISECONDS, "NANOSECONDS" for NANOSECONDS and
     * "SECONDS" for SECONDS.
     *
     * @return The specified string
     */
    std::string toString() const;

private:
    TimeUnit(int i) : Enum<TimeUnit>(i) {}
};

}
}

#endif //#if !defined(CETTY_UTIL_TIMEUNIT_H)

// Local Variables:
// mode: c++
// End:
