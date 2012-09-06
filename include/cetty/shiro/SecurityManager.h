#if !defined(CETTY_SHIRO_SECURITYMANAGER_H)
#define CETTY_SHIRO_SECURITYMANAGER_H
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

#include <cetty/shiro/authc/ModularRealmAuthenticator.h>
#include <cetty/shiro/authz/Authorizer.h>

#include <cetty/shiro/session/SessionManager.h>

namespace cetty {
namespace shiro {
namespace session {
    class DelegateSession;
}
}
}

namespace cetty {
namespace shiro {

using namespace cetty::shiro::authc;
using namespace cetty::shiro::authz;
using namespace cetty::shiro::session;

/**
 * exec authentication and authorisation
 */
class SecurityManager {
public:
    SecurityManager(): sessionManager(NULL) {init();}

    /// login by session id
    bool getSession(const std::string &id, DelegateSession*);

    /// login by user name and password
    const std::string &login(AuthenticationToken &token);
    void logout(const std::string &sessionId);

    /// judge user's operation
    bool authoriseUser(const std::string &sessionId, const std::string &operation);
    /// judge user and third app's operation
    bool authoriseApp(const std::string &appkey, const std::string &operation);

    ~SecurityManager(){ destroy(); }

private:

    ModularRealmAuthenticator authenticator;
    Authorizer authorizer;
    SessionManager *sessionManager;

private:
    void init();

    bool authenticate(const AuthenticationToken &token, AuthenticationInfo *info);
    void bind(AuthenticationInfo &info, DelegateSession &session);

    void onSuccessfulLogin(const AuthenticationToken &token, const AuthenticationInfo &info) {};
    void onFailedLogin(const AuthenticationToken &token){};

    void beforeLogout(const std::string &sessionId){};
    void stopSession(const std::string &sessionId);

    void destroy();
};

}
}
#endif // #if !defined(CETTY_SHIRO_SECURITYMANAGER_H)
