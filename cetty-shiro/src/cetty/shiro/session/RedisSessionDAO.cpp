
#include <cetty/shiro/session/RedisSessionDAO.h>
#include <cetty/shiro/session/Session.h>
#include <cetty/config/ConfigCenter.h>
#include <cetty/redis/builder/RedisClientBuilder.h>
#include <cetty/shiro/util/SecurityUtils.h>

#include <boost/ref.hpp>

namespace cetty {
namespace shiro {
namespace session {

using namespace cetty::redis::builder;
using namespace cetty::shiro::util;

static const std::string KEY_PREFIX = "sess:";

RedisSessionDAO::RedisSessionDAO():redis(NULL){
    ConfigCenter::instance().configure(&config);

    RedisClientBuilder builder;
    builder.addConnection(config.ip, config.port);
    redis = new RedisClient(builder.build());
}

RedisSessionDAO::~RedisSessionDAO(){
    if(redis){
        delete redis;
        redis = NULL;
    }
}

void RedisSessionDAO::readSession(const std::string& sessionId,
                                  const SessionCallback& callback) {
    if(callback.empty()) {
        LOG_TRACE << "Session reading callback is not set.";
        return;
    }
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
    if(callback.empty()) {
        LOG_TRACE << "Session create callback is not set.";
        return;
    }
    if (redis && session) {
        std::string sessionId = generateSessionId(session);
        assignSessionId(session, sessionId);

        std::string key(KEY_PREFIX);
        std::string value;

        key += session->getId();
        session->toJson(&value);

        redis->set(key, value);

        callback(0, session);
    }
    else {
        callback(-1, SessionPtr());
    }
}

void RedisSessionDAO::update(const SessionPtr& session,
                             const SessionCallback& callback) {
    if(callback.empty()) {
        LOG_TRACE << "Session updating callback is not set.";
        return;
    }

    if (redis && session) {
        std::string key(KEY_PREFIX);
        std::string value;

        key += session->getId();
        session->toJson(&value);

        redis->set(key, value);
        callback(0, session);
    }
    else {
        callback(1, session);
    }
}

void RedisSessionDAO::remove(const SessionPtr& session,
                             const SessionCallback& callback) {
    if(callback.empty()) {
        LOG_TRACE << "Session removing callback is not set.";
        return;
    }
    if (redis && session) {
        std::string key(KEY_PREFIX);
        key += session->getId();

        redis->del(key);

        callback(0, session);
    }
    else {
        callback(1, session);
    }
}

void RedisSessionDAO::getSessionCallback(int result,
        const StringPiece& data,
        const SessionCallback& callback) {
    if (result == 0) {
        SessionPtr session = new Session();

        session->setStopCallback(boost::bind(&SessionManager::stop,
            SecurityUtils::getSecurityManager()->getSessionManager(), _1));
        session->setUpdateCallback(boost::bind(&SessionManager::onChange,
            SecurityUtils::getSecurityManager()->getSessionManager(), _1));
        session->setExpireCallback(boost::bind(&SessionManager::expire,
            SecurityUtils::getSecurityManager()->getSessionManager(), _1));

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
