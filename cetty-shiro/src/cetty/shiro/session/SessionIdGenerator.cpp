/*
 * SessionIdGenerator.cpp
 *
 *  Created on: 2012-8-24
 *      Author: chenhl
 */

#include <cetty/shiro/session/SessionIdGenerator.h>

#include <unistd.h>
#include <sys/types.h>

#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>

#include <cetty/shiro/session/Session.h>

namespace cetty {
namespace shiro {
namespace session {

using namespace boost::posix_time;

std::string SessionIdGenerator::generateId(const SessionPtr& session){
    std::stringstream sid;
    std::string host = (session ? session->getHost() : "");
    ptime epoch(boost::gregorian::date(1970,1,1));
    ptime now = microsec_clock::local_time();
    time_duration::tick_type x = (now - epoch).total_nanoseconds();
    pid_t pid = getpid();

    sid << host << x << pid;
    return sid.str();
}

}
}
}
