#if !definded(CETTY_SHIRO_REALM_AUTHENTICATINGREALM_H)
#define CETTY_SHIRO_REALM_AUTHENTICATINGREALM_H
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

#include <cetty/shiro/authc/CredentialsMatcher.h>
#include <cetty/shiro/realm/Realm.h>

class CacheManager;

namespace cetty { namespace shiro { namespace subject {
    class PrincipalCollection;
}}}

namespace cetty {
namespace shiro {
namespace realm {

using namespace cetty::shiro::authc;
using namespace cetty::shiro::subject;
/**
 * A top-level abstract implementation of the <tt>Realm</tt> interface that only implements authentication support
 * (log-in) operations and leaves authorization (access control) behavior to subclasses.
 *
 * <p>Since a Realm provides both authentication <em>and</em> authorization operations, the implementation approach for
 * this class could have been reversed.  That is, authorization support could have been implemented here and
 * authentication support left to subclasses.
 *
 * <p>The reason the existing implementation is in place though
 * (authentication support) is that most authentication operations are fairly common across the large majority of
 * applications, whereas authorization operations are more so heavily dependent upon the application's data model, which
 * can vary widely.
 *
 * <p>By providing the most common authentication operations here and leaving data-model specific authorization checks
 * to subclasses, a top-level abstract class for most common authentication behavior is more useful as an extension
 * point for most applications.
 *
 * @since 0.2
 */
class AuthenticatingRealm : public Realm {
private:
    /**
     * Password matcher used to determine if the provided password matches
     * the password stored in the data store.
     */
    CredentialsMatcher credentialsMatcher;

public:
    /*--------------------------------------------
    |         C O N S T R U C T O R S           |
    ============================================*/
    AuthenticatingRealm() {}

    AuthenticatingRealm(const CacheManager &cacheManager) {
        setCacheManager(cacheManager);
    }

    AuthenticatingRealm(const CredentialsMatcher &matcher) {
        setCredentialsMatcher(matcher);
    }

    AuthenticatingRealm(CacheManager cacheManager, CredentialsMatcher matcher) {
        setCacheManager(cacheManager);
        setCredentialsMatcher(matcher);
    }

    /*--------------------------------------------
    |  A C C E S S O R S / M O D I F I E R S    |
    ============================================*/
    /**
     * Returns the <code>CredentialsMatcher</code> used during an authentication attempt to verify submitted
     * credentials with those stored in the system.
     *
     * <p>Unless overridden by the {@link #setCredentialsMatcher setCredentialsMatcher} method, the default
     * value is a {@link org.apache.shiro.authc.credential.SimpleCredentialsMatcher SimpleCredentialsMatcher} instance.
     *
     * @return the <code>CredentialsMatcher</code> used during an authentication attempt to verify submitted
     *         credentials with those stored in the system.
     */
    const CredentialsMatcher &getCredentialsMatcher() {
        return credentialsMatcher;
    }

    /**
     * Sets the CrendialsMatcher used during an authentication attempt to verify submitted credentials with those
     * stored in the system.  The implementation of this matcher can be switched via configuration to
     * support any number of schemes, including plain text comparisons, hashing comparisons, and others.
     *
     * <p>Unless overridden by this method, the default value is a
     * {@link org.apache.shiro.authc.credential.SimpleCredentialsMatcher} instance.
     *
     * @param credentialsMatcher the matcher to use.
     */
    void setCredentialsMatcher(const CredentialsMatcher &credentialsMatcher) {
        this->credentialsMatcher = credentialsMatcher;
    }

    bool getAuthenticationInfo(const AuthenticationToken &token, AuthenticationInfo *);

    /**
     * Retrieves authentication data from an implementation-specific datasource (RDBMS, LDAP, etc) for the given
     * authentication token.
     *
     * <p>For most datasources, this means just 'pulling' authentication data for an associated subject/user and nothing
     * more and letting Shiro do the rest.  But in some systems, this method could actually perform EIS specific
     * log-in logic in addition to just retrieving data - it is up to the Realm implementation.
     *
     * <p>A <tt>null</tt> return value means that no account could be associated with the specified token.
     *
     * @param token the authentication token containing the user's principal and credentials.
     * @return an {@link AuthenticationInfo} object containing account data resulting from the
     *         authentication ONLY if the lookup is successful (i.e. account exists and is valid, etc.)
     * @throws org.apache.shiro.authc.AuthenticationException
     *          if there is an error acquiring data or performing
     *          realm-specific authentication logic for the specified <tt>token</tt>
     */
    virtual bool doGetAuthenticationInfo(AuthenticationToken token, AuthenticationInfo *);

    /**
     * Default implementation that does nothing (no-op) and exists as a convenience mechanism in case subclasses
     * wish to override it to implement realm-specific logout logic for the given user account logging out.</p>
     * <p/>
     * In a single-realm Shiro configuration (most applications), the <code>principals</code> method
     * argument will be the same as that which is contained in the <code>AuthenticationInfo</code> object returned by the
     * {@link #doGetAuthenticationInfo} method (that is, {@link AuthenticationInfo#getPrincipals info.getPrincipals()}).
     * <p/>
     * In a multi-realm Shiro configuration, the given <code>principals</code> method
     * argument could contain principals returned by many realms.  Therefore the subclass implementation would need
     * to know how to extract the principal(s) relevant to only itself and ignore other realms' principals.  This is
     * usually done by calling {@link org.apache.shiro.subject.PrincipalCollection#fromRealm(String) principals.fromRealm(name)},
     * using the realm's own {@link Realm#getName() name}.
     *
     * @param principals the application-specific Subject/user identifier that is logging out.
     */
    void onLogout(const PrincipalCollection &principals) {
        //no-op, here for subclass override if desired.
    }

    virtual ~AuthenticatingRealm(){}

};
}
}
}
#endif //#if !definded(CETTY_SHIRO_REALM_AUTHENTICATINGREALM_H)
