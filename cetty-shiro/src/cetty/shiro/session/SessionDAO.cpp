#include <cetty/shiro/session/SessionDAO.h>

#include <cetty/shiro/session/Session.h>
#include <cetty/shiro/session/SessionIdGenerator.h>

namespace cetty {
namespace shiro {
namespace session {

std::string SessionDAO::generateSessionId(const SessionPtr& session) {
    return SessionIdGenerator::generateId(session);
}

void SessionDAO::assignSessionId(const SessionPtr& session,
                                 const std::string& sessionId) {
    session->setId(sessionId);
}

}
}
}
