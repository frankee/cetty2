#if !defined(CETTY_SHIRO_AUTHC_MODULARREALMAUTHENTICATION_H)
#define CETTY_SHIRO_AUTHC_MODULARREALMAUTHENTICATION_H
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

#include <cetty/shiro/authc/Authenticator.h>
#include <cetty/shiro/realm/AuthenticatingRealm.h>
#include <cetty/shiro/subject/PrincipalCollection.h>

namespace cetty {
namespace shiro {
namespace authc {

using namespace cetty::shiro::subject;
using namespace cetty::shiro::realm;
/**
 * A {@code ModularRealmAuthenticator} delgates account lookups to a pluggable (modular) collection of
 * {@link Realm}s.  This enables PAM (Pluggable Authentication Module) behavior in Shiro.
 * In addition to authorization duties, a Shiro Realm can also be thought of a PAM 'module'.
 * <p/>
 * Using this Authenticator allows you to &quot;plug-in&quot; your own
 * {@code Realm}s as you see fit.  Common realms are those based on accessing
 * LDAP, relational databases, file systems, etc.
 * <p/>
 * If only one realm is configured (this is often the case for most applications), authentication success is naturally
 * only dependent upon invoking this one Realm's
 * {@link Realm#getAuthenticationInfo(org.apache.shiro.authc.AuthenticationToken)} method.
 * <p/>
 * But if two or more realms are configured, PAM behavior is implemented by iterating over the collection of realms
 * and interacting with each over the course of the authentication attempt.  As this is more complicated, this
 * authenticator allows customized behavior for interpreting what happens when interacting with multiple realms - for
 * example, you might require all realms to be successful during the attempt, or perhaps only at least one must be
 * successful, or some other interpretation.  This customized behavior can be performed via the use of a
 * {@link #setAuthenticationStrategy(AuthenticationStrategy) AuthenticationStrategy}, which
 * you can inject as a property of this class.
 * <p/>
 * The strategy object provides callback methods that allow you to
 * determine what constitutes a success or failure in a multi-realm (PAM) scenario.  And because this only makes sense
 * in a mult-realm scenario, the strategy object is only utilized when more than one Realm is configured.
 * <p/>
 * As most multi-realm applications require at least one Realm authenticates successfully, the default
 * implementation is the {@link AtLeastOneSuccessfulStrategy}.
 */
class ModularRealmAuthenticator : public Authenticator {
public:
    /**
     * Default no-argument constructor which
     * {@link #setAuthenticationStrategy(AuthenticationStrategy) enables}  an
     * {@link org.apache.shiro.authc.pam.AtLeastOneSuccessfulStrategy} by default.
     */
    ModularRealmAuthenticator() {}

    /**
     * Sets all realms used by this Authenticator, providing PAM (Pluggable Authentication Module) configuration.
     *
     * @param realms the realms to consult during authentication attempts.
     */
    void setRealms(const std::vector<AuthenticatingRealm> &realms) {
        this->realms.assign(realms.begin(), realms.end());
    }

    /**
     * Returns the realm(s) used by this {@code Authenticator} during an authentication attempt.
     *
     * @return the realm(s) used by this {@code Authenticator} during an authentication attempt.
     */
    const std::vector<AuthenticatingRealm> &getRealms() const{
        return this->realms;
    }

    void clean(){ realms.clear(); }

    void onLogout(const PrincipalCollection &principals);

    virtual ~ModularRealmAuthenticator(){}


protected:
    bool assertRealmsConfigured() {
        std::vector<AuthenticatingRealm> realms = getRealms();
        return !realms.empty();
    }

    /**
     * Attempts to authenticate the given token by iterating over the internal collection of
     * {@link Realm}s.  For each realm, first the {@link Realm#supports(org.apache.shiro.authc.AuthenticationToken)}
     * method will be called to determine if the realm supports the {@code authenticationToken} method argument.
     * <p/>
     * If a realm does support
     * the token, its {@link Realm#getAuthenticationInfo(org.apache.shiro.authc.AuthenticationToken)}
     * method will be called.  If the realm returns a non-null account, the token will be
     * considered authenticated for that realm and the account data recorded.  If the realm returns {@code null},
     * the next realm will be consulted.  If no realms support the token or all supporting realms return null,
     * an {@link AuthenticationException} will be thrown to indicate that the user could not be authenticated.
     * <p/>
     * After all realms have been consulted, the information from each realm is aggregated into a single
     * {@link AuthenticationInfo} object and returned.
     *
     * @param authenticationToken the token containing the authentication principal and credentials for the
     *                            user being authenticated.
     * @return account information attributed to the authenticated user.
     * @throws IllegalStateException   if no realms have been configured at the time this method is invoked
     * @throws AuthenticationException if the user could not be authenticated or the user is denied authentication
     *                                 for the given principal and credentials.
     */
    virtual bool doAuthenticate(const AuthenticationToken &token, AuthenticationInfo *info);

    /**
     * First calls <code>super.onLogout(principals)</code> to ensure a logout notification is issued, and for each
     * wrapped {@code Realm} that implements the {@link LogoutAware LogoutAware} interface, calls
     * <code>((LogoutAware)realm).onLogout(principals)</code> to allow each realm the opportunity to perform
     * logout/cleanup operations during an user-logout.
     * <p/>
     * Shiro's Realm implementations all implement the {@code LogoutAware} interface by default and can be
     * overridden for realm-specific logout logic.
     *
     * @param principals the application-specific Subject/user identifier.
     */

private:
    /**
     * List of realms that will be iterated through when a user authenticates
     * notice the lifecycle of AuthenticationRealm.
     */
    std::vector<AuthenticatingRealm> realms;
};

}
}
}

#endif // #if !defined(CETTY_SHIRO_AUTHC_MODULARREALMAUTHENTICATION_H)
