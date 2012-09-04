/*
 * SubjectFactory.cpp
 *
 *  Created on: 2012-8-22
 *      Author: chenhl
 */

#include <assert.h>

#include <cetty/shiro/subject/SubjectFactory.h>
#include <cetty/shiro/subject/SubjectContext.h>

namespace cetty {
namespace shiro {
namespace subject {

void SubjectFactory::createSubject(SubjectContext &context, Subject *subject){
    assert(subject != NULL);

    SecurityManager *securityManager = context.resolveSecurityManager();
    SessionPtr session = context.resolveSession();
    PrincipalCollection principals;
    context.resolvePrincipals(&principals);
    bool authenticated = context.resolveAuthenticated();
    std::string host = context.resolveHost();

    newSubjectInstance(principals, authenticated, host, session, securityManager, subject);
}

void SubjectFactory::newSubjectInstance(PrincipalCollection &principals,
                        bool authenticated,
                        const std::string &host,
                        SessionPtr &session,
                        SecurityManager *securityManager,
                        Subject *subject){
    assert(subject != NULL);
    *subject = Subject(principals, securityManager, session, authenticated, host);
}

}
}
}


