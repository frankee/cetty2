#if !defined(CETTY_SHIRO_AUTHC_AUTHENTICATIONINFO_H)
#define CETTY_SHIRO_AUTHC_AUTHENTICATIONINFO_H
/**
 * <code>AuthenticationInfo</code> represents a Subject's (aka user's) stored account information relevant to the
 * authentication/log-in process only.
 * <p/>
 * It is important to understand the difference between this interface and the
 * {@link AuthenticationToken AuthenticationToken} interface.  <code>AuthenticationInfo</code> implementations
 * represent already-verified and stored account data, whereas an <code>AuthenticationToken</code> represents data
 * submitted for any given login attempt (which may or may not successfully match the verified and stored account
 * <code>AuthenticationInfo</code>).
 * <p/>
 * Because the act of authentication (log-in) is orthogonal to authorization (access control), this interface is
 * intended to represent only the account data needed by Shiro during an authentication attempt.  Shiro also
 * has a parallel {@link org.apache.shiro.authz.AuthorizationInfo AuthorizationInfo} interface for use during the
 * authorization process that references access control data such as roles and permissions.
 * <p/>
 * But because many if not most {@link org.apache.shiro.realm.Realm Realm}s store both sets of data for a Subject, it might be
 * convenient for a <code>Realm</code> implementation to utilize an implementation of the {@link Account Account}
 * interface instead, which is a convenience interface that combines both <code>AuthenticationInfo</code> and
 * <code>AuthorizationInfo</code>.  Whether you choose to implement these two interfaces separately or implement the one
 * <code>Account</code> interface for a given <code>Realm</code> is entirely based on your application's needs or your
 * preferences.
 * <p/>
 * <p><b>Pleae note:</b>  Since Shiro sometimes logs authentication operations, please ensure your AuthenticationInfo's
 * <code>toString()</code> implementation does <em>not</em> print out account credentials (password, etc), as these might be viewable to
 * someone reading your logs.  This is good practice anyway, and account credentials should rarely (if ever) be printed
 * out for any reason.  If you're using Shiro's default implementations of this interface, they only ever print the
 * account {@link #getPrincipals() principals}, so you do not need to do anything additional.</p>
 *
 * @see org.apache.shiro.authz.AuthorizationInfo AuthorizationInfo
 * @see Account
 * @since 0.9
 */

#include <string>

namespace cetty {
namespace shiro {
namespace authc {

class AuthenticationInfo {

public:
    static const std::string USER_ID;
    static const std::string CREDENTIALS;
    static const std::string CREDENTIALS_SALT;
    static const std::string CODE_TYPE;


public:
    AuthenticationInfo(){}


    const std::string &getUserId() const { return userId; }
    void setUserId(const std::string &userId){ this->userId = userId; }

    const std::string &getCredentials() const{ return credentials; }
    void setCredentials(const std::string &credentials) { this->credentials = credentials; }

    const std::string &getCredentialsSalt() const{ return credentialsSalt; }
    void setCredentialsSalt(const std::string &salt){ this->credentialsSalt = salt; }

    const std::string &getCodeType() const { return codeType; }
    void setCodeType(const std::string &codeType) { this->codeType = codeType; }


private:
    std::string userId;
    std::string credentials;
    std::string credentialsSalt;
    std::string codeType;
};

}
}
}
#endif // #if !defined(CETTY_SHIRO_AUTHC_AUTHENTICATIONINFO_H)
