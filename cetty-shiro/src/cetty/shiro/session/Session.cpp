/*
 * Session.cpp
 *
 *  Created on: 2012-8-13
 *      Author: chenhl
 */
#include <cetty/shiro/session/Session.h>

#include <cstring>
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

static void splitStr(std::string src, std::string *key, std::string *value){
    size_t pos = src.find(':');
    if(pos == std::string::npos){
        // todo add log
        return;

    }

    key->assign(src, 0, pos);
    value->assign(src, pos + 1, src.size() - pos -1);
}

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
        stop();
        return Session::STOPPED;
    }

    if (isTimedOut()) {
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
        DLOG_DEBUG << "without value, will remove the attribute";
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
    ss << "expired:" << (expired ? "true" : "false")
       << ",login:" << (login ? "true" : "false")
       << ",remember:" << (rememberMe ? "true" : "false");

    char timeout[20];
    memset(timeout, 0x00, sizeof(timeout));
    sprintf(timeout, "%d", this->timeout);

    ss << ",timeout:" << timeout
       << ",id:" << id
       << ",host:" << host
       << ",startTime:" << to_simple_string(startTime)
       << ",stopTime:" << to_simple_string(stopTime)
       << ",lastAccessTime:" << to_simple_string(lastAccessTime);

    std::map<std::string, std::string>::const_iterator it;
    for(it = attributes.begin(); it != attributes.end(); ++ it){
        std::string key = it->first;
        std::string value = it->second;
        ss << "," << key << ":" << value;
    }

    json->assign(ss.str());
}

void Session::fromJson(const char* json) {
    /*
    std::string token;
    std::string key, value;
    const char *pre = json;

    const char *pos = strchr(pre, ',');
    if(pos == NULL){
        // todo add log
        return;
    }
    token.assign(pre, pos - pre);
    splitStr(token, &key, &value);
    expired = ("true" == value ? true : false);

    pre = pos + 1;
    pos = strchr(pre, ',');
    if(pos == NULL){
        // todo add log
        return;
    }
    token.assign(pre, pos - pre);
    splitStr(token, &key, &value);

    // todo find third lib to parse json
     * */
}



}
}
}
