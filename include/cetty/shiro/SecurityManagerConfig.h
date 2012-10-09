#if !defined(CETTY_SHIRO_SECURITYMANAGERCONFIG_H)
#define CETTY_SHIRO_SECURITYMANAGERCONFIG_H

/*
 * Copyright (c) 2010-2012 frankee zhou (frankee.zhou at gmail dot com)
 *
 * Distributed under under the Apache License, version 2.0 (the "License").
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 */

#include <cetty/config/ConfigObject.h>
#include <cetty/shiro/realm/SqlRealm.h>

namespace cetty {
namespace shiro {

using namespace cetty::config;
using namespace cetty::shiro::realm;

class SecurityManagerConfig: public ConfigObject {
public:
    class AuthenticatorConfig: ConfigObject {
    public:
        class HashedCredentialsMatcherConfig: public ConfigObject{
        public:
            bool hashSalted;
            bool storedCredentialsHexEncoded;

            int hashIterations;

            std::string hashAlgorithm;

            HashedCredentialsMatcherConfig();
            virtual ConfigObject* create() const { return new HashedCredentialsMatcherConfig; }
        };

        HashedCredentialsMatcherConfig *config;

        AuthenticatorConfig();
        virtual ConfigObject* create() const { return new AuthenticatorConfig; }
    };

    /*
    class AuthorizerConfig: ConfigObject {
        AuthorizerConfig();
        virtual ConfigObject* create() const { return new AuthorizerConfig; }
    };
    */

    class SessionManagerConfig: ConfigObject {
    public:
        bool deleteInvalidSessions;
        bool sessionValidationSchedulerEnabled;

        int globalSessionTimeout;
        int sessionValidationInterval;

        SessionManagerConfig();
        virtual ConfigObject* create() const { return new SessionManagerConfig; }
    };

    class RealmConfig: ConfigObject {
    public:
        std::string name;

        std::string backend;
        std::string connectionString;

        std::string authenticationQuery;
        std::string userRolesQuery;
        std::string permissionsQuery;

        SqlRealm::SaltStyle saltStyle;
        bool permissionsLookupEnabled;

        RealmConfig();

        virtual ConfigObject* create() const { return new RealmConfig; }
    };
    
    AuthenticatorConfig *authenticatorConfig;
    //AuthorizerConfig *authorizerConfig;
    SessionManagerConfig *sessionManagerConfig;
    RealmConfig *realmConfig;

    SecurityManagerConfig();
    virtual ConfigObject* create() const { return new SecurityManagerConfig; }
};

}
}

#endif //#if !defined(CETTY_SHIRO_SECURITYMANAGERCONFIG_H)

// Local Variables:
// mode: c++
// End:
