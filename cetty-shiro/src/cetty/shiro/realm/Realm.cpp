/*
 * Realm.cpp
 *
 *  Created on: 2012-8-20
 *      Author: chenhl
 */

#include <cetty/shiro/realm/Realm.h>

#include <unistd.h>
#include <sys/types.h>

#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>

namespace cetty {
namespace shiro {
namespace realm {

using namespace boost::posix_time;

std::string Realm::autoName(){
    std::stringstream sid;

    ptime epoch(boost::gregorian::date(1970,1,1));
    ptime now = microsec_clock::local_time();
    time_duration::tick_type x = (now - epoch).total_nanoseconds();
    pid_t pid = getpid();

    sid << x << pid;
    return sid.str();
}

}
}
}
