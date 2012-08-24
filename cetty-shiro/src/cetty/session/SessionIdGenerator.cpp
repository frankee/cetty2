/*
 * SessionIdGenerator.cpp
 *
 *  Created on: 2012-8-24
 *      Author: chenhl
 */



/*
namespace cetty {
namespace shrio {
namespace session {

using namespace boost::posix_time;

std::string SessionIdGenerator::generateId(SessionPtr session){
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
*/


