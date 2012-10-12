/*
 * SqlRealmConfig.cpp
 *
 *  Created on: 2012-10-9
 *      Author: chenhl
 */

#include <cetty/shiro/realm/SqlRealmConfig.h>
#include <cetty/shiro/realm/SqlRealm.h>

#include <cetty/config/ConfigDescriptor.h>

namespace cetty {
namespace shiro {
namespace realm {

CETTY_CONFIG_ADD_DESCRIPTOR(SqlRealmConfig,
                            new ConfigDescriptor(
                                8,
                                CETTY_CONFIG_FIELD(SqlRealmConfig, name, STRING),
                                CETTY_CONFIG_FIELD(SqlRealmConfig, backend, STRING),
                                CETTY_CONFIG_FIELD(SqlRealmConfig, connectionString, STRING),
                                CETTY_CONFIG_FIELD(SqlRealmConfig, authenticationQuery, STRING),
                                CETTY_CONFIG_FIELD(SqlRealmConfig, userRolesQuery, STRING),
                                CETTY_CONFIG_FIELD(SqlRealmConfig, permissionsQuery, STRING),
                                CETTY_CONFIG_FIELD(SqlRealmConfig, saltStyle, INT32),
                                CETTY_CONFIG_FIELD(SqlRealmConfig, permissionsLookupEnabled, BOOL)
                            ),
                            new SqlRealmConfig);

SqlRealmConfig::SqlRealmConfig()
    : ConfigObject("SqlRealmConfig"),
      authenticationQuery(SqlRealm::DEFAULT_AUTHENTICATION_QUERY),
      userRolesQuery(SqlRealm::DEFAULT_USER_ROLES_QUERY),
      permissionsQuery(SqlRealm::DEFAULT_PERMISSIONS_QUERY),
      saltStyle(SqlRealm::NO_SALT),
      permissionsLookupEnabled(true){}


}
}
}
