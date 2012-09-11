
#include <cetty/shiro/session/RedisSessionDAO.h>
#include <cetty/shiro/session/Session.h>

namespace cetty {
namespace shiro {
namespace session {

static const std::string KEY_PREFIX = "sess:";

void RedisSessionDAO::readSession(const std::string& sessionId,
                                  const SessionCallback& callback) {
    if (redis) {
        std::string key(KEY_PREFIX);
        key += sessionId;
        redis->get(key, boost::bind(
                       &RedisSessionDAO::getSessionCallback,
                       this,
                       _1,
                       _2,
                       boost::cref(callback)));
    }
    else {

    }
}

void RedisSessionDAO::create(const SessionPtr& session,
                             const SessionCallback& callback) {
    if (redis && session) {
        std::string key(KEY_PREFIX);
        std::string value;

        key += session->getId();
        session->toJson(&value);

        redis->set(key, value);
    }
    else {

    }
}

void RedisSessionDAO::update(const SessionPtr& session,
                             const SessionCallback& callback) {
    if (redis && session) {
        std::string key(KEY_PREFIX);
        std::string value;

        key += session->getId();
        session->toJson(&value);

        redis->set(key, value);
    }
    else {

    }
}

void RedisSessionDAO::remove(const SessionPtr& session,
                             const SessionCallback& callback) {
    if (redis && session) {
        std::string key(KEY_PREFIX);
        key += session->getId();

        redis->del(key);
    }
    else {

    }
}

void RedisSessionDAO::getSessionCallback(int result,
        const StringPiece& data,
        const SessionCallback& callback) {
    if (result == 0) {
        SessionPtr session = new Session;
        session->fromJson(data.c_str());
        callback(0, session);
    }
    else {
        callback(result, SessionPtr());
    }
}

}
}
}