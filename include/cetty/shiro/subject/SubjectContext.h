#if !defined(CETTY_SHIRO_SUBJECT_SUBJECTCONTEXT_H)
#define CETTY_SHIRO_SUBJECT_SUBJECTCONTEXT_H
/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */
#include <cstdlib>

#include <cetty/shiro/session/Session.h>
#include <cetty/shiro/authc/AuthenticationInfo.h>

namespace cetty {
namespace shiro {
namespace subject {

using namespace cetty::shiro::session;
using namespace cetty::shiro::authc;
/**
 * Default implementation of the {@link SubjectContext} interface.  Note that the getters and setters are not
 * simple pass-through methods to an underlying attribute;  the getters will employ numerous heuristics to acquire
 * their data attribute as best as possible (for example, if {@link #getPrincipals} is invoked, if the principals aren't
 * in the backing map, it might check to see if there is a subject or session in the map and attempt to acquire the
 * principals from those objects).
 *
 * @since 1.0
 */
class SubjectContext : public MapContext{

private:
    static const std::string SECURITY_MANAGER;
    static const std::string SESSION_ID;
    static const std::string AUTHENTICATION_TOKEN;
    static const std::string AUTHENTICATION_INFO;
    static const std::string SUBJECT;
    static const std::string PRINCIPALS;
    static const std::string SESSION;
    static const std::string AUTHENTICATED ;
    static const std::string HOST;

public:
    /**
     * The session key that is used to store subject principals.
     */
    static const std::string PRINCIPALS_SESSION_KEY;

    /**
     * The session key that is used to store whether or not the user is authenticated.
     */
    static const std::string AUTHENTICATED_SESSION_KEY;

    //SubjectContext(): securityManager(NULL), subject(NULL), principal(NULL) {}

    SecurityManager *getSecurityManager() {
        return getTypedValue<SecurityManager>(SECURITY_MANAGER);
    }

    void setSecurityManager(SecurityManager *securityManager) {
        nullSafePut(SECURITY_MANAGER, (boost::any *)securityManager);
    }

    SecurityManager *resolveSecurityManager();

    std::string getSessionId() {
        return get<std::string>(SESSION_ID);
    }

    void setSessionId(std::string *sessionId) {
        nullSafePut(SESSION_ID, (boost::any *)sessionId);
    }

    Subject *getSubject() {
        return subject;
    }

    void setSubject(Subject *subject) {
        this->subject = subject;
    }

    PrincipalCollection *getPrincipals() {
        return principal;
    }

    void setPrincipals(PrincipalCollection *principals) {
        this->principal = principals;
    }

    PrincipalCollection *resolvePrincipals();

   Session *getSession();

    void setSession(Session *session);

    Session *resolveSession();

    bool isAuthenticated();

    void setAuthenticated(bool authc);

    bool resolveAuthenticated();

    AuthenticationInfo *getAuthenticationInfo();

    void setAuthenticationInfo(AuthenticationInfo *info);

    AuthenticationToken *getAuthenticationToken();

    void setAuthenticationToken(AuthenticationToken *token);
    std::string getHost();

    void setHost(std::string host);

    std::string resolveHost();
};
}
}
}

#endif // #if !defined(CETTY_SHIRO_SUBJECT_SUBJECTCONTEXT_H)
