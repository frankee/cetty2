/*
 * SecurityManagerConfig.cpp
 *
 *  Created on: 2012-9-29
 *      Author: chenhl
 */

#include <cetty/shiro/SecurityManagerConfig.h>
#include <cetty/config/ConfigDescriptor.h>
#include <cetty/shiro/session/SessionManager.h>


namespace cetty {
namespace shiro {

using namespace cetty::shiro::session;

CETTY_CONFIG_ADD_DESCRIPTOR(SecurityManagerConfig_AuthenticatorConfig_HashedCredentialsMatcherConfig,
                            new ConfigDescriptor(
                                4,
                                CETTY_CONFIG_FIELD(SecurityManagerConfig::AuthenticatorConfig::HashedCredentialsMatcherConfig, hashSalted, BOOL),
                                CETTY_CONFIG_FIELD(SecurityManagerConfig::AuthenticatorConfig::HashedCredentialsMatcherConfig, storedCredentialsHexEncoded, BOOL),
                                CETTY_CONFIG_FIELD(SecurityManagerConfig::AuthenticatorConfig::HashedCredentialsMatcherConfig, hashIterations, INT32),
                                CETTY_CONFIG_FIELD(SecurityManagerConfig::AuthenticatorConfig::HashedCredentialsMatcherConfig, hashAlgorithm, STRING)
                            ),
                            new SecurityManagerConfig::AuthenticatorConfig::HashedCredentialsMatcherConfig);


CETTY_CONFIG_ADD_DESCRIPTOR(SecurityManagerConfig_AuthenticatorConfig,
                            new ConfigDescriptor(
                                1,
                                CETTY_CONFIG_OBJECT_FIELD(SecurityManagerConfig::AuthenticatorConfig, config, HashedCredentialsMatcherConfig)
                            ),
                            new SecurityManagerConfig::AuthenticatorConfig);

/*
CETTY_CONFIG_ADD_DESCRIPTOR(SecurityManagerConfig_AuthorizerConfig,
                            new ConfigDescriptor(

                            ),
                            new SecurityManagerConfig::AuthorizerConfig);
*/

CETTY_CONFIG_ADD_DESCRIPTOR(SecurityManagerConfig_SessionManagerConfig,
                            new ConfigDescriptor(
                                4,
                                CETTY_CONFIG_FIELD(SecurityManagerConfig::SessionManagerConfig, deleteInvalidSessions, BOOL),
                                CETTY_CONFIG_FIELD(SecurityManagerConfig::SessionManagerConfig, sessionValidationSchedulerEnabled, BOOL),
                                CETTY_CONFIG_FIELD(SecurityManagerConfig::SessionManagerConfig, globalSessionTimeout, INT32),
                                CETTY_CONFIG_FIELD(SecurityManagerConfig::SessionManagerConfig, sessionValidationInterval, INT32)
                            ),
                            new SecurityManagerConfig::SessionManagerConfig);

CETTY_CONFIG_ADD_DESCRIPTOR(SecurityManagerConfig_RealmConfig,
                            new ConfigDescriptor(
                                8,
                                CETTY_CONFIG_FIELD(SecurityManagerConfig::RealmConfig, name, STRING),
                                CETTY_CONFIG_FIELD(SecurityManagerConfig::RealmConfig, backend, STRING),
                                CETTY_CONFIG_FIELD(SecurityManagerConfig::RealmConfig, connectionString, STRING),
                                CETTY_CONFIG_FIELD(SecurityManagerConfig::RealmConfig, authenticationQuery, STRING),
                                CETTY_CONFIG_FIELD(SecurityManagerConfig::RealmConfig, userRolesQuery, STRING),
                                CETTY_CONFIG_FIELD(SecurityManagerConfig::RealmConfig, permissionsQuery, STRING),
                                CETTY_CONFIG_FIELD(SecurityManagerConfig::RealmConfig, saltStyle, INT32),
                                CETTY_CONFIG_FIELD(SecurityManagerConfig::RealmConfig, permissionsLookupEnabled, BOOL)
                            ),
                            new SecurityManagerConfig::RealmConfig);


CETTY_CONFIG_ADD_DESCRIPTOR(SecurityManagerConfig,
                            new ConfigDescriptor(
                                3,
                                CETTY_CONFIG_OBJECT_FIELD(SecurityManagerConfig, authenticatorConfig, AuthenticatorConfig),
                                /*CETTY_CONFIG_OBJECT_FIELD(SecurityManagerConfig, authorizerConfig, AuthorizerConfig),*/
                                CETTY_CONFIG_OBJECT_FIELD(SecurityManagerConfig, sessionManagerConfig, SessionManagerConfig),
                                CETTY_CONFIG_OBJECT_FIELD(SecurityManagerConfig, realmConfig, RealmConfig)
                            ),
                            new SecurityManagerConfig);



SecurityManagerConfig::AuthenticatorConfig::HashedCredentialsMatcherConfig::HashedCredentialsMatcherConfig()
    : ConfigObject("SecurityManagerConfig_AuthenticatorConfig_HashedCredentialsMatcherConfig"),
      hashSalted(true),
      storedCredentialsHexEncoded(true),
      hashIterations(1){};

SecurityManagerConfig::AuthenticatorConfig::AuthenticatorConfig()
    : ConfigObject("SecurityManagerConfig_AuthenticatorConfig"),
      config(0){}



SecurityManagerConfig::SessionManagerConfig::SessionManagerConfig()
    : ConfigObject("SecurityManagerConfig_SessionManagerConfig"),
      deleteInvalidSessions(true),
      sessionValidationSchedulerEnabled(false),
      globalSessionTimeout(SessionManager::DEFAULT_GLOBAL_SESSION_TIMEOUT),
      sessionValidationInterval(SessionManager::DEFAULT_SESSION_VALIDATION_INTERVAL){}

SecurityManagerConfig::RealmConfig::RealmConfig()
    : ConfigObject("SecurityManagerConfig_RealmConfig"),
      saltStyle(SqlRealm::NO_SALT),
      permissionsLookupEnabled(true){}

SecurityManagerConfig::SecurityManagerConfig()
    : ConfigObject("SecurityManagerConfig"),
      authenticatorConfig(NULL),
      sessionManagerConfig(NULL),
      realmConfig(NULL){}

}
}
