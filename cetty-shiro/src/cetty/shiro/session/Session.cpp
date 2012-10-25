/*
 * Session.cpp
 *
 *  Created on: 2012-8-13
 *      Author: chenhl
 */
#include <cetty/shiro/session/Session.h>

#include <cstring>
#include <yaml-cpp/yaml.h>
#include <boost/date_time/posix_time/posix_time_types.hpp>

#include <cetty/logging/LoggerHelper.h>
#include <cetty/shiro/session/SessionManager.h>

namespace cetty {
namespace shiro {
namespace session {

using namespace boost::posix_time;

const int MILLIS_PER_SECOND = 1000;
const int MILLIS_PER_MINUTE = 60 * MILLIS_PER_SECOND;
const int MILLIS_PER_HOUR = 60 * MILLIS_PER_MINUTE;
static const std::string EMPTY_STR;
const std::string Session::SESSION_SELF = "$self";
const std::string Session::NULL_TIME = "null";

Session::Session()
    : expired(false),
      login(false),
      rememberMe(false),
      timeout(SessionManager::DEFAULT_GLOBAL_SESSION_TIMEOUT),
      startTime(second_clock::universal_time()),
      lastAccessTime(startTime) {
}

Session::Session(const std::string& host)
    : expired(false),
      login(false),
      rememberMe(false),
      timeout(SessionManager::DEFAULT_GLOBAL_SESSION_TIMEOUT),
      host(host),
      startTime(second_clock::universal_time()),
      lastAccessTime(startTime) {
}

Session::Session(const std::string& host,
                 const StopCallback& stopCallback,
                 const UpdateCallback& updateCallback,
                 const ExpireCallback& expireCallback)
    :expired(false),
     login(false),
     rememberMe(false),
     timeout(SessionManager::DEFAULT_GLOBAL_SESSION_TIMEOUT),
     host(host),
     startTime(second_clock::universal_time()),
     lastAccessTime(startTime),
     stopCallback(stopCallback),
     updateCallback(updateCallback),
     expireCallback(expireCallback){
}

bool Session::isTimedOut() const {
    if (isExpired()) {
        return true;
    }

    int timeout = getTimeout();

    if (timeout > 0) {
        ptime lastAccessTime = getLastAccessTime();

        if (lastAccessTime.is_not_a_date_time()) {
            return true;
        }

        time_duration diff = second_clock::universal_time() - lastAccessTime;

        if (diff.total_milliseconds() >= timeout) {
            return true;
        }
    }

    return false;
}

bool Session::isStopped() const {
    return !getStopTimestamp().is_not_a_date_time();
}

bool Session::isValid() const {
    return !isStopped() && !isExpired();
}

Session::SessionState Session::validate() {
    if (isStopped()) {
        LOG_INFO << "session [" << getId() << "] has stoped.";
        stop();
        return Session::STOPPED;
    }

    if (isTimedOut()) {
        LOG_INFO << "session [" << getId() << "] has expired.";
        expire();
        return Session::EXPIRED;
    }

    return Session::ACTIVE;
}

const std::string& Session::getAttribute(const std::string& key) const {
    std::map<std::string, std::string>::const_iterator it = attributes.find(key);

    if (it == attributes.end()) {
        return EMPTY_STR;
    }

    return it->second;
}

void Session::setAttribute(const std::string& key, const std::string& value) {
    if (key.empty()) {
        LOG_WARN << "set the session attribute without the key";
        return;
    }

    if (value.empty()) {
        LOG_WARN << "without value, will remove the attribute";
        removeAttribute(key);
    }

    if(key.empty() || value.empty()) return;
    attributes.insert(std::pair<std::string, std::string>(key, value));

    if (updateCallback) {
        updateCallback(shared_from_this());
    }
}

void Session::removeAttribute(const std::string& key) {
    if (key.empty()) {
        LOG_WARN << "set the session attribute without the key";
        return;
    }

    attributes.erase(key);

    if (updateCallback) {
        updateCallback(shared_from_this());
    }
}

void Session::stop() {
    doStop();

    if (stopCallback) {
        stopCallback(shared_from_this());
    }
}

void Session::expire() {
    doExpire();
    if(expireCallback) expireCallback(shared_from_this());
}

void Session::doStop() {
    if (this->stopTime.is_not_a_date_time())
        this->stopTime = second_clock::universal_time();
}

void Session::doExpire() {
    this->expired = true;
    doStop();
}

void Session::setId(const std::string& id) {
    this->id = id;
}

void Session::setStartTimestamp(const ptime& startTimestamp) {
    this->startTime = startTimestamp;
    if(updateCallback) updateCallback(shared_from_this());
}

void Session::setStopTimestamp(const ptime& stopTimestamp) {
    this->stopTime = stopTimestamp;
    if(updateCallback) updateCallback(shared_from_this());
}

void Session::setLastAccessTime(ptime& lastAccessTime) {
    this->lastAccessTime = lastAccessTime;
    if(updateCallback) updateCallback(shared_from_this());
}

void Session::setTimeout(int timeout) {
    if (this->timeout != timeout) {
        this->timeout = timeout;
        if (updateCallback) {
            updateCallback(shared_from_this());
        }
    }
}

void Session::setHost(const std::string& host) {
    this->host = host;
    if(updateCallback) updateCallback(shared_from_this());
}

void Session::touch() {
    this->lastAccessTime =  second_clock::universal_time();
    if(updateCallback) updateCallback(shared_from_this());
}

void Session::setLogin(bool login) {
    if(this->login != login){
        this->login = login;
        if(updateCallback) updateCallback(shared_from_this());
    }
}

void Session::toJson(std::string* json) const {
    std::stringstream ss;
    ss << "{" << SESSION_SELF << ": [";

    ss << (expired ? "true" : "false")
       << "," << (login ? "true" : "false")
       << "," << (rememberMe ? "true" : "false");

    char timeout[20];
    memset(timeout, 0x00, sizeof(timeout));
    sprintf(timeout, "%d", this->timeout);

    ss << "," << timeout
       << "," << id
       << "," << host;

    ss << ",";
    if(!startTime.is_not_a_date_time()){
        ss << to_simple_string(startTime);
    } else {
        LOG_ERROR << "Session start time[" << to_simple_string(startTime) << "]is invalid";
        ss << NULL_TIME;
    }

    ss << ",";
    if(!stopTime.is_not_a_date_time()){
        ss << to_simple_string(stopTime);
    } else {
        ss << NULL_TIME;
    }

    ss << ",";
    if(!lastAccessTime.is_not_a_date_time()){
        ss << to_simple_string(lastAccessTime);
    } else {
        LOG_ERROR << "Session last access time[" << to_simple_string(lastAccessTime) << "]is invalid";
        ss << NULL_TIME;
    }
    ss << "]";
    std::map<std::string, std::string>::const_iterator it;
    for(it = attributes.begin(); it != attributes.end(); ++ it){
        std::string key = it->first;
        std::string value = it->second;
        ss << "," << key << ": " << value;
    }
    ss << "}";

    json->assign(ss.str());
}

void Session::fromJson(const char* json) {

    YAML::Node node = YAML::Load(json);
    YAML::const_iterator it = node.begin();
    for(; it != node.end(); ++it){
        if(SESSION_SELF == it->first.as<std::string>()){
            YAML::Node temp = node[SESSION_SELF];
            assert(temp.IsSequence());
            expired = temp[0].as<bool>();
            login = temp[1].as<bool>();
            rememberMe = temp[2].as<bool>();
            timeout = temp[3].as<int>();
            id = temp[4].as<std::string>();
            host = temp[5].as<std::string>();

            if(NULL_TIME != temp[6].as<std::string>())
                startTime = time_from_string(temp[6].as<std::string>());
            if(NULL_TIME != temp[7].as<std::string>())
                stopTime = time_from_string(temp[7].as<std::string>());
            if(NULL_TIME != temp[8].as<std::string>())
                lastAccessTime = time_from_string(temp[8].as<std::string>());
        }else{
            attributes.insert(std::pair<std::string, std::string>(
                it->first.as<std::string>(),
                it->second.as<std::string>())
                );
        }
    }
}

}
}
}
