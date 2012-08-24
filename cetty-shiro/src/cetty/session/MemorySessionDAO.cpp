/*
 * MemorySessionDAO.cpp
 *
 *  Created on: 2012-8-24
 *      Author: chenhl
 */

#include <cetty/shiro/session/MemorySessionDAO.h>

namespace cetty {
namespace shiro {
namespace session {

void MemorySessionDAO::getActiveSessions(std::vector<SessionPtr> *actives){
    if(actives == NULL) return;
    std::map<std::string, SessionPtr>::iterator it = sessions.begin();
    for(; it != sessions.end(); ++it)
        actives->push_back(it->second);
}

void MemorySessionDAO::remove(SessionPtr &session) {
    std::string id = session->getId();
    if (!id.empty()) {
        sessions.erase(id);
    }
}

std::string MemorySessionDAO::doCreate(SessionPtr &session) {
    std::string sessionId = generateSessionId(session);
    assignSessionId(session, sessionId);
    storeSession(sessionId, session);
    return sessionId;
}

void MemorySessionDAO::storeSession(const std::string &id, SessionPtr &session) {
    if (id.empty()) return;
    if(sessions.count(session->getId()) > 0) {
        sessions.erase(session->getId());
    }
    sessions.insert(std::pair<std::string, SessionPtr>(id, session));
}

SessionPtr MemorySessionDAO::doReadSession(const std::string &sessionId) {
    std::map<std::string, SessionPtr >::iterator ret = sessions.find(sessionId);
    if(ret == sessions.end()) return SessionPtr();
    return ret->second;
}

}
}
}
