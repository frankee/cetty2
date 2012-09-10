#include <cetty/shiro/session/SessionManager.h>
#include <cetty/shiro/util/EmptyObj.h>

namespace cetty {
namespace shiro {
namespace session {

using namespace cetty::shiro::util;

const int SessionManager::MILLIS_PER_SECOND = 1000;
const int SessionManager::MILLIS_PER_MINUTE = 60 * MILLIS_PER_SECOND;
const int SessionManager::MILLIS_PER_HOUR = 60 * MILLIS_PER_MINUTE;
const int SessionManager::DEFAULT_GLOBAL_SESSION_TIMEOUT = 30 * MILLIS_PER_MINUTE;
const int SessionManager::DEFAULT_SESSION_VALIDATION_INTERVAL = MILLIS_PER_HOUR;


SessionManager::SessionManager()
  :deleteInvalidSessions(true),
   sessionValidationSchedulerEnabled(true),
   globalSessionTimeout(DEFAULT_GLOBAL_SESSION_TIMEOUT),
   sessionValidationInterval(DEFAULT_SESSION_VALIDATION_INTERVAL),
   sessionDAO(NULL),
   sessionValidationScheduler(NULL)
{
    sessionDAO = new MemorySessionDAO();
}
SessionManager::~SessionManager(){
    if(sessionDAO != NULL) {
        delete sessionDAO;
        sessionDAO = NULL;
    }
    if(sessionValidationScheduler != NULL) {
        delete sessionValidationScheduler;
        sessionValidationScheduler = NULL;
    }
}

void SessionManager::validateSessions(){
    int invalidCount = 0;
    std::vector<SessionPtr> v;

    getActiveSessions(&v);
    if(v.size() > 0){
        std::vector<SessionPtr>::iterator it = v.begin();
        for(; it != v.end(); ++it) {
            Session::SessionState state = validate(*it);
            if(state == Session::STOPPED || state == Session::EXPIRED)
                invalidCount ++;
        }
    }
}

Session::SessionState SessionManager::validate(SessionPtr &session) {
    Session::SessionState state = session->validate();
    if(state == Session::EXPIRED) {
        onExpiration(session);
    }
    else if(state == Session::STOPPED){
        onInvalidation(session, state);
    }

    return state;
}

void SessionManager::onInvalidation(SessionPtr &s, Session::SessionState state){
    if(state == Session::EXPIRED){
        onExpiration(s);
        return;
    }
    onStop(s);
    notifyStop(s);
    afterStopped(s);
}

void SessionManager::notifyStart(const SessionPtr &session){
    std::vector<SessionChangeCallback>::iterator it = listeners.begin();
    for(; it != listeners.end(); ++it) (*it)(session, Session::START);
}

void SessionManager::notifyStop(const SessionPtr &session){
    std::vector<SessionChangeCallback>::iterator it = listeners.begin();
    for(; it != listeners.end(); ++it) (*it)(session, Session::STOPPED);
}
void SessionManager::notifyExpiration(const SessionPtr &session){
    std::vector<SessionChangeCallback>::iterator it = listeners.begin();
    for(; it != listeners.end(); ++it) (*it)(session, Session::EXPIRED);
}

void SessionManager::enableSessionValidation(){
    SessionValidationScheduler *scheduler = getSessionValidationScheduler();
    if (scheduler == NULL) {
        scheduler = createSessionValidationScheduler();
        setSessionValidationScheduler(scheduler);
    }

    scheduler->enableSessionValidation();
    afterSessionValidationEnabled();
}
void SessionManager::disableSessionValidation(){
    beforeSessionValidationDisabled();
    SessionValidationScheduler *scheduler = getSessionValidationScheduler();
    if (scheduler != NULL) {
        scheduler->disableSessionValidation();
        delete scheduler;
        scheduler = NULL;
        setSessionValidationScheduler(NULL);
    }
}

ptime SessionManager::getStartTimestamp(const std::string &sessionId) {
    SessionPtr s = lookupRequiredSession(sessionId);
    if(!s) return ptime();
    return s->getStartTimestamp();
}

ptime SessionManager::getLastAccessTime(const std::string &sessionId) {
    SessionPtr s = lookupRequiredSession(sessionId);
    if(!s) return ptime();
    return s->getLastAccessTime();
}

int SessionManager::getTimeout(const std::string &sessionId){
    SessionPtr s = lookupRequiredSession(sessionId);
    if(!s) return -1L;
    return s->getTimeout();
}

void SessionManager::setTimeout(const std::string &id, int maxIdleTimeInMillis){
    SessionPtr s = lookupRequiredSession(id);
    if (!s) return;
    s->setTimeout(maxIdleTimeInMillis);
    onChange(s);
}

void SessionManager::touch(const std::string &id){
    SessionPtr s = lookupRequiredSession(id);
    if(!s) return;
    s->touch();
    onChange(s);
}

std::string SessionManager::getHost(const std::string &id) {
    SessionPtr s = lookupRequiredSession(id);
    if(!s) return std::string();
    return s->getHost();
}

void SessionManager::getAttributeKeys(const std::string &id, std::vector<std::string> *keys) {
    SessionPtr s = lookupRequiredSession(id);
    if(!s) return;
    s->getAttributeKeys(keys);
}

std::string& SessionManager::getAttribute(const std::string &id, const std::string &key){
    SessionPtr s = lookupRequiredSession(id);
    if(!s) return emptyString;
    return s->getAttribute(key);
}

void SessionManager::setAttribute(const std::string &id, const std::string &key, const std::string &value) {
    if (value.empty()) {
        removeAttribute(id, key);
    } else {
        SessionPtr s = lookupRequiredSession(id);
        if(!s) return;
        s->setAttribute(key, value);
        onChange(s);
    }
}

void SessionManager::removeAttribute(const std::string &id, const std::string &key) {
    SessionPtr s = lookupRequiredSession(id);
    if(!s) return;
    s->removeAttribute(key);
    onChange(s);
}

bool SessionManager::isLogin(const std::string &id){
    if(id.empty()) return false;
    SessionPtr s = lookupRequiredSession(id);
    if(s == NULL) return false;
    return s->isLogin();
}

void SessionManager::setLogin(const std::string &id, bool login){
    if(id.empty()) return;
    SessionPtr s = lookupRequiredSession(id);
    if(s == NULL) return;
    s->setLogin(login);
    onChange(s);
}

void SessionManager::stop(const std::string &id){
    SessionPtr s = lookupRequiredSession(id);
    if(s == NULL) return;
    s->stop();
    onStop(s);
    notifyStop(s);
    afterStopped(s);
}

SessionPtr SessionManager::start() {
    SessionPtr session = createSession();
    if(session){
        applyGlobalSessionTimeout(session);
        onStart(session);
        notifyStart(session);
    }
    return session;
}
}
}
}
