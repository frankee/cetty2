#if !defined(CETTY_SHIRO_REALM_SQLREALM_H)
#define CETTY_SHIRO_REALM_SQLREALM_H

#include <cetty/shiro/realm/AuthorizingRealm.h>

namespace cetty {
namespace shiro {
namespace realm {

class SqlRealm : public AuthorizingRealm {
public:
    enum SaltStyle {NO_SALT, CRYPT, COLUMN, EXTERNAL};

public:
    SqlRealm();
    SqlRealm(const std::string &backend, const std::string &connStr);

    virtual ~SqlRealm(){}

    void setAuthenticationQuery(const std::string &authenticationQuery) {
         this->authenticationQuery = authenticationQuery;
    }

    void setUserRolesQuery(const std::string &userRolesQuery) {
        this->userRolesQuery = userRolesQuery;
    }

    void setPermissionsQuery(const std::string &permissionsQuery) {
       this->permissionsQuery = permissionsQuery;
    }

    void setPermissionsLookupEnabled(bool permissionsLookupEnabled) {
        this->permissionsLookupEnabled = permissionsLookupEnabled;
    }

    void setSaltStyle(SqlRealm::SaltStyle saltStyle);

    void setBackend(const std::string &backend){
        this->backend = backend;
    }

    void setConnectionString(const std::string &connectionString){
        this->connectionString = connectionString;
    }

    virtual void getAuthenticationInfo(const AuthenticationToken& token,
                                       const GetAuthenticationInfoCallback& callback);

    virtual void doGetAuthorizationInfo(const PrincipalCollection& principals,
                                        const GetAuthorizationInfoCallback& callback);

protected:
    static const std::string DEFAULT_AUTHENTICATION_QUERY;
    static const std::string DEFAULT_SALTED_AUTHENTICATION_QUERY;
    static const std::string DEFAULT_USER_ROLES_QUERY;
    static const std::string DEFAULT_PERMISSIONS_QUERY;


    std::string authenticationQuery;
    std::string userRolesQuery;
    std::string permissionsQuery;

    static const std::string BACKEND;
    static const std::string CONNECTION_STRING;

    std::string backend;
    std::string connectionString;
    bool permissionsLookupEnabled;
    SaltStyle saltStyle;

private:
    void getPasswordForUser(const std::string &username, std::vector<std::string> *passwd);
    void getRoleNamesForUser(const std::string &username, std::vector<std::string> *roles);
    void getPermissions(const std::string &userName,
                        const std::vector<std::string> &roles,
                        std::vector<std::string> *permissions);
    void getSaltForUser(const std::string &userName, std::string *salt){}

};

}
}
}

#endif // #if !defined(CETTY_SHIRO_REALM_SQLREALM_H)
