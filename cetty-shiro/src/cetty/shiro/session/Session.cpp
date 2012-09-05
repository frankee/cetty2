/*
 * Session.cpp
 *
 *  Created on: 2012-8-13
 *      Author: chenhl
 */
#include <cetty/shiro/session/Session.h>
#include <cetty/shiro/session/SessionManager.h>

#include <cetty/shiro/util/EmptyObj.h>

namespace cetty {
namespace shiro {
namespace session {

using namespace boost::posix_time;

const int Session::MILLIS_PER_SECOND = 1000;
const int Session::MILLIS_PER_MINUTE = 60 * MILLIS_PER_SECOND;
const int Session::MILLIS_PER_HOUR = 60 * MILLIS_PER_MINUTE;

void Session::init() {
    this->timeout = SessionManager::DEFAULT_GLOBAL_SESSION_TIMEOUT;
    this->startTimestamp = second_clock::local_time();
    this->lastAccessTime = this->startTimestamp;
    }

bool Session::isTimedOut() {
    if (isExpired()) {
        return true;
    }

    long timeout = getTimeout();
    if (timeout >= 0l) {
        ptime lastAccessTime = getLastAccessTime();
        if (lastAccessTime.is_not_a_date_time()) {
            return true;
        }
        time_duration diff = ptime(second_clock::local_time()) - lastAccessTime;
        if (diff.seconds() >= (timeout / 1000)) { return true; }
    }
    return false;
}

Session::SessionState Session::validate(){
    if(isStopped()) return Session::STOPPED;
    if(isTimedOut()) {
        expire();
        return Session::EXPIRED;
    }
    return Session::ACTIVE;
}

void Session::stop() {
    if (this->stopTimestamp.is_not_a_date_time()) {
        this->stopTimestamp =  ptime( second_clock::local_time() );
    }
}

const std::string &Session::getAttribute(const std::string& key) {
    std::map<std::string, std::string>::iterator it = attributes.find(key);
    if (it == attributes.end()) { return cetty::shiro::util::emptyString; }
    return it->second;
}

void Session::setAttribute(const std::string& key, const std::string& value){
    if(key.empty()) return;
    attributes.insert(std::pair<std::string, std::string>(key, value));
}

void Session::removeAttribute(const std::string& key){
    if(key.empty()) return;
    attributes.erase(key);
}

void Session::expire() {
    stop();
    this->expired = true;
}

}
}
}
