/*
 * Realm.cpp
 *
 *  Created on: 2012-8-20
 *      Author: chenhl
 */

#include <cetty/shiro/realm/Realm.h>

#include <sys/types.h>

#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <cetty/util/Process.h>

namespace cetty {
namespace shiro {
namespace realm {

using namespace boost::posix_time;
using namespace cetty::util;

Realm::Realm(bool isCached, std::string name)
    :isCached(isCached), name(name) {
    if(name.empty()) name = autoName();
}

std::string Realm::autoName(){
    std::stringstream sid;

    ptime epoch(boost::gregorian::date(1970,1,1));
    ptime now = microsec_clock::local_time();
    time_duration::tick_type x = (now - epoch).total_nanoseconds();
    Process::PID pid = Process::id();

    sid << x << pid;
    return sid.str();
}

}
}
}


