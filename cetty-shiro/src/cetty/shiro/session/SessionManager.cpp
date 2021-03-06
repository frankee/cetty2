#include <cetty/shiro/session/SessionManager.h>

#include <boost/bind.hpp>
#include <cetty/shiro/session/Session.h>
#include <cetty/shiro/session/SessionDAO.h>
#include <cetty/shiro/session/RedisSessionDAO.h>

#include <cetty/shiro/util/SecurityUtils.h>

#include <cetty/config/ConfigCenter.h>

namespace cetty {
namespace shiro {
namespace session {

using namespace cetty::shiro;
using namespace cetty::shiro::util;
using namespace cetty::shiro::session;
using namespace cetty::config;

const int MILLIS_PER_SECOND = 1000;
const int MILLIS_PER_MINUTE = 60 * MILLIS_PER_SECOND;
const int MILLIS_PER_HOUR = 60 * MILLIS_PER_MINUTE;
const int SessionManager::DEFAULT_GLOBAL_SESSION_TIMEOUT = 30 * MILLIS_PER_MINUTE;
const int SessionManager::DEFAULT_SESSION_VALIDATION_INTERVAL = MILLIS_PER_HOUR;
//const int SessionManager::REMEMBER_ME_SESSION_TIMEOUT = 7 * 24 * MILLIS_PER_HOUR;

SessionManager::SessionManager()
    :sessionDAO(NULL),
     validationScheduler(NULL) {
    sessionDAO = new RedisSessionDAO();
    ConfigCenter::instance().configure(&config);
    init();
}

void SessionManager::init(){
    deleteInvalidSessions = config.deleteInvalidSessions;
    sessionValidationSchedulerEnabled = config.sessionValidationSchedulerEnabled;
    globalSessionTimeout = config.globalSessionTimeout;
    sessionValidationInterval = config.sessionValidationInterval;
}

SessionManager::~SessionManager() {
    if (sessionDAO != NULL) {
        delete sessionDAO;
        sessionDAO = NULL;
    }

    if (validationScheduler != NULL) {
        delete validationScheduler;
        validationScheduler = NULL;
    }
}

void SessionManager::validateSessions() {
    int invalidCount = 0;
    std::vector<SessionPtr> v;

    getActiveSessions(&v);
    std::vector<SessionPtr>::iterator it = v.begin();

    for (; it != v.end(); ++it) {
        Session::SessionState state = validate(*it);

        if (state == Session::STOPPED || state == Session::EXPIRED) {
            invalidCount ++;
        }
    }
}

Session::SessionState SessionManager::validate(SessionPtr session) {
    assert(session);
    Session::SessionState state = session->validate();
    return state;
}

void SessionManager::notifyStart(const SessionPtr& session) {
    std::vector<SessionChangeCallback>::iterator it = listeners.begin();
    for (; it != listeners.end(); ++it) {
        (*it)(session, Session::START);
    }
}

void SessionManager::notifyStop(const SessionPtr& session) {
    std::vector<SessionChangeCallback>::iterator it = listeners.begin();

    for (; it != listeners.end(); ++it) {
        (*it)(session, Session::STOPPED);
    }
}
void SessionManager::notifyExpiration(const SessionPtr& session) {
    std::vector<SessionChangeCallback>::iterator it = listeners.begin();

    for (; it != listeners.end(); ++it) {
        (*it)(session, Session::EXPIRED);
    }
}

void SessionManager::enableSessionValidation() {
    SessionValidationScheduler* scheduler = getSessionValidationScheduler();

    if (scheduler == NULL) {
        scheduler = createSessionValidationScheduler();
        setSessionValidationScheduler(scheduler);
    }

    scheduler->enableSessionValidation();
    afterSessionValidationEnabled();
}

void SessionManager::disableSessionValidation() {
    beforeSessionValidationDisabled();
    SessionValidationScheduler* scheduler = getSessionValidationScheduler();

    if (scheduler != NULL) {
        scheduler->disableSessionValidation();
        delete scheduler;
        scheduler = NULL;
        setSessionValidationScheduler(NULL);
    }
}

void SessionManager::stop(const SessionPtr& session) {
    if (session) {
        onStop(session);
        notifyStop(session);
        afterStopped(session);
    }
}

void SessionManager::start(const std::string& host,
                           const SessionCallback& callback) {

    enableSessionValidationIfNecessary();
    SessionPtr session = new Session(
        host,
        boost::bind(&SessionManager::stop, this, _1),
        boost::bind(&SessionManager::onChange, this, _1),
        boost::bind(&SessionManager::expire, this, _1)
    );

    sessionDAO->create(
    	session,
        boost::bind(
            &SessionManager::createSessionCallback,
            this,
            _1, // operation code: 0 success, -1 failed
            _2, // SessionPtr
            boost::cref(callback)
        )
    );
}

void SessionManager::createSessionCallback(int result,
        const SessionPtr& session,
        const SessionCallback& callback) {
    if (!result) {
        applyGlobalSessionTimeout(session);
        onStart(session);
        notifyStart(session);
        sessions.insert(std::pair<std::string, SessionPtr>(session->getId(), session));

        callback(session);
    } else {
        callback(SessionPtr());
    }
}

void SessionManager::getSession(const std::string& id,
                                const SessionCallback& callback) {
    enableSessionValidationIfNecessary();
    std::map<std::string, SessionPtr>::iterator it = sessions.find(id);
    if(it != sessions.end()){
        readSessionCallback(0, (*it).second, callback);
        return;
    }

    sessionDAO->readSession(
        id,
        boost::bind(&SessionManager::readSessionCallback,
            this,
            _1, // return code
            _2, // session
            callback
        )
    );
}

void SessionManager::readSessionCallback(
    int result,
    const SessionPtr& session,
    const SessionCallback& callback)
{
    if (!result) {
        Session::SessionState state = validate(session);
        if(state == Session::STOPPED|| state == Session::EXPIRED){
            callback(SessionPtr());
        } else{
            session->touch();

            // There will not cover the old session which has the same key in #sessions
            sessions.insert(std::pair<std::string, SessionPtr>(session->getId(), session));
            callback(session);
        } // if(session->isStopped() || session->isExpired())
    } else {
        callback(SessionPtr());
    } // if (!result)
}

void SessionManager::addSessionListeners(const SessionChangeCallback& callback) {
    if (callback) {
        listeners.push_back(callback);
    }
}

void SessionManager::onStop(const SessionPtr& session) {
    ptime stopTs = session->getStopTimestamp();
    session->setLastAccessTime(stopTs);
    session->setLogin(false);
}

void SessionManager::afterStopped(const SessionPtr& session) {
    if(clearCallback) clearCallback(session);
    if (isDeleteInvalidSessions()) remove(session);
}

void SessionManager::onExpiration(const SessionPtr& session) {
    onChange(session);
}

void SessionManager::afterExpired(const SessionPtr& session) {
    if(clearCallback) clearCallback(session);
    if (isDeleteInvalidSessions()) {
        remove(session);
    }
}

void SessionManager::onChange(const SessionPtr& session) {
    sessionDAO->update(session, SessionDAO::SessionCallback());
}

void SessionManager::applyGlobalSessionTimeout(const SessionPtr& session) {
    session->setTimeout(getGlobalSessionTimeout());
}

void SessionManager::expire(const SessionPtr& session) {
    if (session) {
        onExpiration(session);
        notifyExpiration(session);
        afterExpired(session);
    }
}

void SessionManager::enableSessionValidationIfNecessary() {
    SessionValidationScheduler* scheduler = getSessionValidationScheduler();

    if (isSessionValidationSchedulerEnabled() && (scheduler == NULL || !scheduler->isEnabled())) {
        enableSessionValidation();
    }
}

void SessionManager::remove(const SessionPtr& session) {
    if (session) {
        sessions.erase(session->getId());
        sessionDAO->remove(session, SessionDAO::SessionCallback());
    }
}

void SessionManager::getActiveSessions(std::vector<SessionPtr>* actives) {
    //sessionDAO->getActiveSessions(actives);
}

SessionValidationScheduler* SessionManager::createSessionValidationScheduler() {
    SessionValidationScheduler* svs = new SessionValidationScheduler();
    return svs;
}

}
}
}
