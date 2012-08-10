#if !defined(CETTY_SHIRO_SUBJECT_SUBJECT_H)
#define CETTY_SHIRO_SUBJECT_SUBJECT_H
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
#include <vector>
#include <cstdlib>
#include <cetty/shiro/session/Session.h>
#include <cetty/shiro/SecurityManager.h>
#include <cetty/shiro/authc/AuthenticationToken.h>

class Permission;

namespace cetty {
namespace shiro {
namespace subject {

using namespace cetty::shiro;
using namespace cetty::shiro::session;
using namespace cetty::shiro::authc;
/**
 * Implementation of the {@code Subject} interface that delegates
 * method calls to an underlying {@link org.apache.shiro.mgt.SecurityManager SecurityManager} instance for security checks.
 * It is essentially a {@code SecurityManager} proxy.
 * <p/>
 * This implementation does not maintain state such as roles and permissions (only {@code Subject}
 * {@link #getPrincipals() principals}, such as usernames or user primary keys) for better performance in a stateless
 * architecture.  It instead asks the underlying {@code SecurityManager} every time to perform
 * the authorization check.
 * <p/>
 * A common misconception in using this implementation is that an EIS resource (RDBMS, etc) would
 * be &quot;hit&quot; every time a method is called.  This is not necessarily the case and is
 * up to the implementation of the underlying {@code SecurityManager} instance.  If caching of authorization
 * data is desired (to eliminate EIS round trips and therefore improve database performance), it is considered
 * much more elegant to let the underlying {@code SecurityManager} implementation or its delegate components
 * manage caching, not this class.  A {@code SecurityManager} is considered a business-tier component,
 * where caching strategies are better suited.
 * <p/>
 * Applications from large and clustered to simple and JVM-local all benefit from
 * stateless architectures.  This implementation plays a part in the stateless programming
 * paradigm and should be used whenever possible.
 *
 * @since 0.1
 */
class Subject {

protected:
    PrincipalCollection *principals;
    bool authenticated;
    std::string host;
    Session *session;

private:
    static const std::string RUN_AS_PRINCIPALS_SESSION_KEY;
protected:
    SecurityManager *securityManager;

public:
    Subject(SecurityManager *securityManager)
      : securityManager(securityManager),
        authenticated(false),
        session(false),
        principals(NULL){}

    Subject(PrincipalCollection *principals,
            bool authenticated,
            const std::string &host,
            Session *session,
            SecurityManager *securityManager)
        :securityManager(securityManager) {
        this->principals = principals;
        this->authenticated = authenticated;
        this->host = host;
            // ???
            this->session = session;
            //???
    }

public:
    const SecurityManager &getSecurityManager() {
        return securityManager;
    }

protected:
    bool hasPrincipals() {
        return !(getPrincipals()->isEmpty());
    }

public:
    /**
     * Returns the host name or IP associated with the client who created/is interacting with this Subject.
     *
     * @return the host name or IP associated with the client who created/is interacting with this Subject.
     */
    const std::string &getHost() {
        return this->host;
    }

private:
    const std::string &getPrimaryPrincipal(PrincipalCollection *principals) {
        if (!principals->isEmpty()) {
            return principals->getPrimaryPrincipal();
        }
        return std::string();
    }

public:
    /**
     * @see Subject#getPrincipal()
     */
    std::string getPrincipal() {
        return getPrimaryPrincipal(getPrincipals());
    }

     PrincipalCollection *getPrincipals() {
        return this->principals;
    }

    bool isPermitted(std::string permission){ return false; }

    bool isPermitted(Permission permission) { return false; }

    void isPermitted(std::string *perssions, std::vector<bool> *bools) { }

    void isPermitted(std::vector<Permission> permissions,std::vector<bool> *bools) { }

    bool isPermittedAll(std::vector<std::string> permissions) { return false; }

    bool isPermittedAll(std::vector<Permission> permissions) { return false; }



    void checkPermission(std::string permission){}

    void checkPermission(Permission permission){}

    void checkPermissions(std::vector<std::string> perssions){}

    void checkPermissions(std::vector<Permission> permissions){}

    bool hasRole(std::string roleIdentifier) { return false; }

    void hasRoles(const std::vector<std::string> &roleIdentifiers, std::vector<bool> *bools) {}

    bool hasAllRoles(const std::vector<std::string> &roleIdentifiers) { return false; }

    void checkRole(const std::string &role){}
    void checkRoles(const std::vector<std::string> &roleIdentifiers)  {}

    void checkRoles(const std::vector<std::string> roles){}

    bool login(const AuthenticationToken &token);

    bool isAuthenticated() {
        return authenticated;
    }

    bool isRemembered() {
        PrincipalCollection principals = getPrincipals();
        return !principals.isEmpty() && !isAuthenticated();
    }

    Session *getSession() {
        return getSession(true);
    }

    Session *getSession(bool create) {
        if (this->session == NULL && create) {
            SessionContext *sessionContext = createSessionContext();
            Session *session = this->securityManager->start(sessionContext);
            this->session = decorate(session);
        }
        return this->session;
    }

protected:
    SessionContext *createSessionContext() {
        SessionContext *sessionContext = new SessionContext();
        if (host.size() > 0) {
            sessionContext->setHost(host);
        }
        return sessionContext;
    }

    Session *decorate(Session *session);

public:
    void logout(){
        // session.stop() 其实是stopawaresession.stop，继承了代理类！！！
    };

private:

    // should delete the session?
    void sessionStopped();



    protected:
        void assertAuthzCheckPossible() {}

private:


    void pushIdentity(PrincipalCollection principals);

    PrincipalCollection popIdentity();
    public:
    virtual ~Subject(){}
};
}
}
}

#endif //#if !defined(CETTY_SHIRO_SUBJECT_SUBJECT_H)
