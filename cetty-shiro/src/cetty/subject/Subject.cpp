#include <cetty/shiro/subject/Subject.h>
#include <cetty/shiro/SecurityManager.h>
#include <cetty/shiro/session/SessionContext.h>

namespace cetty {
namespace shiro {
namespace subject {

bool Subject::login(AuthenticationToken &token){
    Subject subject;
    if(!securityManager->login(token, *this, &subject)) return false;

    PrincipalCollection principals = subject.principals;
    assert(!principals.isEmpty());

    this->principals = principals;
    this->authenticated = true;

    host = token.getHost();
    this->host = host;

    SessionPtr session = subject.getSession(false);
    if (session) this->session = session;

    return true;
}

void Subject::logout(){
    this->securityManager->logout(*this);

    this->session->release();
    this->principals.clear();
    this->authenticated = false;

    //Don't set securityManager to null here - the Subject can still be
    //used, it is just considered anonymous at this point.  The SecurityManager instance is
    //necessary if the subject would log in again or acquire a new session.  This is in response to
    //https://issues.apache.org/jira/browse/JSEC-22
    //this.securityManager = null;
}

SessionPtr Subject::getSession(bool create){
    if (!this->session && create) {
        SessionContext sessionContext = createSessionContext();
        SessionPtr session = this->securityManager->start(sessionContext);
        this->session = session;
    }
    return this->session;
}

SessionContext Subject::createSessionContext(){
    SessionContext sessionContext;
    if (!host.empty()) {
        sessionContext.setHost(host);
     }
    return sessionContext;
}

void Subject::sessionStopped(){
    this->session->release();
}

}
}
}


