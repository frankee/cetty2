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

#include <cetty/shiro/subject/PrincipalCollection.h>
#include <cetty/shiro/util/ByteSource.h>

namespace cetty {
namespace shiro {
namespace authc {

using namespace cetty::shiro::subject;
using namespace cetty::shiro::util;

/**
 * Simple AuthenticationInfo that holds the principals and credentials.
 */
class AuthenticationInfo {

public:
    AuthenticationInfo(){}

    /**
     * Constructor that takes in a single 'primary' principal of the account and its corresponding credentials,
     * associated with the specified realm.
     * <p/>
     * This is a convenience constructor and will construct a {@link PrincipalCollection PrincipalCollection} based
     * on the {@code principal} and {@code realmName} argument.
     *
     * @param principal   the 'primary' principal associated with the specified realm.
     * @param credentials the credentials that verify the given principal.
     * @param realmName   the realm from where the principal and credentials were acquired.
     */
    AuthenticationInfo(const std::string &principal, const std::string &credentials, const std::string &realmName);

    /**
     * Constructor that takes in a single 'primary' principal of the account, its corresponding hashed credentials,
     * the salt used to hash the credentials, and the name of the realm to associate with the principals.
     * <p/>
     * This is a convenience constructor and will construct a {@link PrincipalCollection PrincipalCollection} based
     * on the <code>principal</code> and <code>realmName</code> argument.
     *
     * @param principal         the 'primary' principal associated with the specified realm.
     * @param hashedCredentials the hashed credentials that verify the given principal.
     * @param credentialsSalt   the salt used when hashing the given hashedCredentials
     * @param realmName         the realm from where the principal and credentials were acquired.
     */
    AuthenticationInfo(const std::string &principal,
                       const std::string &hashedCredentials,
                       const ByteSource &credentialsSalt,
                       const std::string &realmName);

    /**
     * Constructor that takes in an account's identifying principal(s) and its corresponding credentials that verify
     * the principals.
     *
     * @param principals  a Realm's account's identifying principal(s)
     * @param credentials the accounts corresponding principals that verify the principals.
     */
    AuthenticationInfo(const PrincipalCollection &principals, const std::string &credentials);

    /**
     * Constructor that takes in an account's identifying principal(s), hashed credentials used to verify the
     * principals, and the salt used when hashing the credentials.
     *
     * @param principals        a Realm's account's identifying principal(s)
     * @param hashedCredentials the hashed credentials that verify the principals.
     * @param credentialsSalt   the salt used when hashing the hashedCredentials.
     */
    AuthenticationInfo(const PrincipalCollection &principals,
                       const std::string &hashedCredentials,
                       const ByteSource &credentialsSalt);

    AuthenticationInfo(const AuthenticationInfo &info){
        this->principals = info.principals;
        this->credentials = info.credentials;
        this->credentialsSalt = info.credentialsSalt;
    }

    AuthenticationInfo& operator=(const AuthenticationInfo &info){
        this->principals = info.principals;
        this->credentials = info.credentials;
        this->credentialsSalt = info.credentialsSalt;
        return *this;
    }

    /**
     * Returns all principals associated with the corresponding Subject.  Each principal is an identifying piece of
     * information useful to the application such as a username, or user id, a given name, etc - anything useful
     * to the application to identify the current <code>Subject</code>.
     * <p/>
     * The returned PrincipalCollection should <em>not</em> contain any credentials used to verify principals, such
     * as passwords, private keys, etc.  Those should be instead returned by {@link #getCredentials() getCredentials()}.
     *
     * @return all principals associated with the corresponding Subject.
     */
    const PrincipalCollection &getPrincipals() const{ return principals; }

    /**
     * Sets the identifying principal(s) represented by this instance.
     *
     * @param principals the indentifying attributes of the corresponding Realm account.
     */
    void setPrincipals(const PrincipalCollection &principals) {
        this->principals = principals;
    }

    /**
     * Returns the credentials associated with the corresponding Subject.  A credential verifies one or more of the
     * {@link #getPrincipals() principals} associated with the Subject, such as a password or private key.  Credentials
     * are used by Shiro particularly during the authentication process to ensure that submitted credentials
     * during a login attempt match exactly the credentials here in the <code>AuthenticationInfo</code> instance.
     *
     * @return the credentials associated with the corresponding Subject.
     */
    const std::string &getCredentials() const{ return credentials; }

    /**
     * Sets the credentials that verify the principals/identity of the associated Realm account.
     *
     * @param credentials attribute(s) that verify the account's identity/principals, such as a password or private key.
     */
    void setCredentials(const std::string &credentials) { this->credentials = credentials; }

    /**
     * Merges the given {@link AuthenticationInfo} into this instance.  The specific way
     * that the merge occurs is up to the implementation, but typically it involves combining
     * the principals and credentials together in this instance.  The <code>info</code> argument should
     * not be modified in any way.
     *
     * @param info the info that should be merged into this instance.
     */
    void merge(const AuthenticationInfo &info){}

    /**
     * Returns the salt used to hash the credentials, or {@code null} if no salt was used or credentials were not
     * hashed at all.
     * <p/>
     * Note that this attribute is <em>NOT</em> handled in the
     * {@link #merge(AuthenticationInfo) merge} method - a hash salt is only useful within a single realm (as each
     * realm will perform it's own Credentials Matching logic), and once finished in that realm, Shiro has no further
     * use for salts.  Therefore it doesn't make sense to 'merge' salts in a multi-realm scenario.
     *
     * @return the salt used to hash the credentials, or {@code null} if no salt was used or credentials were not
     *         hashed at all.
     * @since 1.1
     */
    const ByteSource& getCredentialsSalt() const{ return credentialsSalt; }

    /**
     * Sets the salt used to hash the credentials, or {@code null} if no salt was used or credentials were not
     * hashed at all.
     * <p/>
     * Note that this attribute is <em>NOT</em> handled in the
     * {@link #merge(AuthenticationInfo) merge} method - a hash salt is only useful within a single realm (as each
     * realm will perform it's own Credentials Matching logic), and once finished in that realm, Shiro has no further
     * use for salts.  Therefore it doesn't make sense to 'merge' salts in a multi-realm scenario.
     *
     * @param salt the salt used to hash the credentials, or {@code null} if no salt was used or credentials were not
     *             hashed at all.
     * @since 1.1
     */
    void setCredentialsSalt(const ByteSource &salt){ this->credentialsSalt = salt; }


    /*

    int compare(const AuthenticationInfo& info);

    bool operator<(const AuthenticationInfo& info) {
            return compare(info) < 0;
    }
    */
private:
    /**
     * The principals identifying the account associated with this AuthenticationInfo instance.
     */
    PrincipalCollection principals;

    /**
     * The credentials verifying the account principals.
     */
    std::string credentials;

    /* Any salt used in hashing the credentials.*/
    ByteSource credentialsSalt;
};

}
}
}
#endif // #if !defined(CETTY_SHIRO_AUTHC_AUTHENTICATIONINFO_H)
