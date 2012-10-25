/*
 * SqlRealm.cpp
 *
 *  Created on: 2012-9-19
 *      Author: chenhl
 */

#include <cetty/shiro/realm/SqlRealm.h>

#include <soci.h>
#include <sqlite3/soci-sqlite3.h>

#include <cetty/shiro/authc/AuthenticationToken.h>
#include <cetty/logging/LoggerHelper.h>
#include <cetty/config/ConfigCenter.h>

namespace cetty {
namespace shiro {
namespace realm {

const std::string SqlRealm::DEFAULT_AUTHENTICATION_QUERY =
    "SELECT password FROM users WHERE username = ?";

const std::string SqlRealm::DEFAULT_SALTED_AUTHENTICATION_QUERY =
    "SELECT password, password_salt FROM users WHERE username = ?";

const std::string SqlRealm::DEFAULT_USER_ROLES_QUERY =
    "SELECT role_name FROM user_roles WHERE username = ?";

const std::string SqlRealm::DEFAULT_PERMISSIONS_QUERY =
    "SELECT permission FROM roles_permissions WHERE role_name = ?";

const std::string SqlRealm::NAME = "SqlRealm";

SqlRealm::SqlRealm() {
    ConfigCenter::instance().configure(&config);
    init();
}

void SqlRealm::init() {
    if (config.saltStyle == SqlRealm::COLUMN
            && config.authenticationQuery == DEFAULT_AUTHENTICATION_QUERY) {
        config.authenticationQuery = DEFAULT_SALTED_AUTHENTICATION_QUERY;
    }

    if(config.authenticationQuery.empty())
        config.authenticationQuery = SqlRealm::DEFAULT_AUTHENTICATION_QUERY;
    if(config.userRolesQuery.empty())
        config.userRolesQuery = SqlRealm::DEFAULT_USER_ROLES_QUERY;
    if(config.permissionsQuery.empty())
        config.permissionsQuery = SqlRealm::DEFAULT_PERMISSIONS_QUERY;
}

void SqlRealm::doGetAuthenticationInfo(const AuthenticationToken& token,
                                       const GetAuthenticationInfoCallback& callback) {
    AuthenticationInfoPtr info;

    std::string principal = token.getPrincipal();

    if (principal.empty()) {
        LOG_TRACE << "Principal is empty.";
        callback(info);
        return;
    }

    std::vector<std::string> passwd;
    getPasswordForUser(principal, &passwd);

    if (passwd.size() == 0) {
        LOG_TRACE << "Can't find authentication info from db.";
        callback(info);
        return;
    }

    if (passwd.size() == 1) {
        info = new AuthenticationInfo(principal, passwd[0], getName());
    }
    else {
        info = new AuthenticationInfo(principal, passwd[0], passwd[1], getName());
    }

    callback(info);
}

void SqlRealm::getPasswordForUser(const std::string& username,
                                  std::vector<std::string>* passwd) {
    if (config.backend.empty() || config.connectionString.empty()) {
        LOG_ERROR << "DB connection info does not be set.";
        return;
    }

    std::string credentials;
    std::string salt;

    bool returningSeparatedSalt = false;

    switch (config.saltStyle) {
    case NO_SALT:
    case CRYPT:
    case EXTERNAL:
        break;

    default:
        returningSeparatedSalt = true;
        break;
    }

    try {
        soci::session sql(config.backend, config.connectionString);
        soci::rowset<soci::row> rows = (sql.prepare << config.authenticationQuery, soci::use(username));
        soci::rowset_iterator<soci::row> it = rows.begin();
        bool foundResult = false;

        for (; it != rows.end(); ++it) {
            // Check to ensure only one row is processed
            if (foundResult) {
                LOG_ERROR << "More than one user row found for user ["
                          << username
                          << "]. Usernames must be unique.";
                return;
            }

            if (returningSeparatedSalt) {
                (*it) >> credentials >> salt;
            }
            else {
                (*it) >> credentials;
            }

            foundResult = true;
        }

        if (foundResult) {
            passwd->push_back(credentials);

            if (returningSeparatedSalt) {
                passwd->push_back(salt);
            }
        }

    }
    catch (const soci::soci_error& e) {
        LOG_ERROR << e.what();
    }
}

void SqlRealm::doGetAuthorizationInfo(const PrincipalCollection& principals,
                                      const GetAuthorizationInfoCallback& callback) {
    AuthorizationInfoPtr info;

    //null usernames are invalid
    if (principals.isEmpty()) {
        LOG_TRACE << "principal is empty.";
        callback(info);
    }

    std::string principal = principals.getPrimaryPrincipal();
    std::vector<std::string> roles;
    std::vector<std::string> permissions;

    try {
        getRoleNamesForUser(principal, &roles);

        if (config.permissionsLookupEnabled) {
            getPermissions(principal, roles, &permissions);
        }
    }
    catch (const soci::soci_error& e) {
        LOG_ERROR << e.what();
        callback(info);
    }

    info = new AuthorizationInfo(roles);
    info->setStringPermissions(permissions);
    callback(info);
}

void SqlRealm::getRoleNamesForUser(const std::string& username,
                                   std::vector<std::string>* roles) {
    if ((config.backend).empty() || (config.connectionString).empty()) {
        LOG_ERROR << "DB connection info does not be set.";
        return;
    }

    if (roles == NULL) {
        LOG_ERROR << "roles argument can't be null";
        return;
    }

    try {
        soci::session sql(config.backend, config.connectionString);
        soci::rowset<soci::row> rows = (sql.prepare << config.userRolesQuery, soci::use(username));
        soci::rowset_iterator<soci::row> it = rows.begin();

        std::string userRole;

        for (; it != rows.end(); ++it) {
            (*it) >> userRole;

            if (userRole.empty()) { continue; }

            roles->push_back(userRole);
        }
    }
    catch (const soci::soci_error& e) {
        LOG_ERROR << e.what();
    }

    if (roles->empty()) {
        LOG_TRACE << "No roles are found for user ["
                  << username
                  << "]";
    }
}

void SqlRealm::getPermissions(const std::string& userName,
                              const std::vector<std::string>& roles,
                              std::vector<std::string>* permissions) {
    if ((config.backend).empty() || (config.connectionString).empty()) {
        LOG_ERROR << "DB connection info does not be set.";
        return;
    }

    if (roles.empty()) { return; }

    if (permissions == NULL) {
        LOG_ERROR << "permissions argument can't be null";
        return;
    }

    try {
        soci::session sql(config.backend, config.connectionString);

        std::string permission;
        std::vector<std::string>::const_iterator it = roles.begin();

        for (; it != roles.end(); ++it) {
            if (it->empty()) { continue; }

            soci::rowset<soci::row> rows = (sql.prepare << config.permissionsQuery, soci::use(*it));
            soci::rowset_iterator<soci::row> rowIt = rows.begin();

            for (; rowIt != rows.end(); ++rowIt) {
                (*rowIt) >> permission;

                if (!permission.empty()) {
                    permissions->push_back(permission);
                }
            }
        }
    }
    catch (const soci::soci_error& e) {
        LOG_ERROR << e.what();
    }

    if (permissions->empty()) {
        LOG_ERROR << "No permissions are found for user ["
                  << userName
                  <<"]";
    }
}

}
}
}
