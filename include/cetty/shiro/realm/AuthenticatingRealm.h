#if !defined(CETTY_SHIRO_REALM_AUTHENTICATINGREALM_H)
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

#include <cetty/shiro/authc/Sha256CredentialsMatcher.h>
#include <cetty/shiro/realm/Realm.h>
#include <cetty/shiro/realm/AuthenticatingRealmPtr.h>

namespace cetty {
namespace shiro {
namespace realm {

using namespace cetty::shiro::authc;

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
public:
    AuthenticatingRealm() { matcher = new Sha256CredentialsMatcher(); }
    AuthenticatingRealm(Sha256CredentialsMatcher *matcher) {
        setCredentialsMatcher(matcher);
    }
    AuthenticatingRealm(const AuthenticatingRealm &authenticationRealm){
        this->matcher = new Sha256CredentialsMatcher();
    }

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
    Sha256CredentialsMatcher *getCredentialsMatcher() const { return matcher; }

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
    void setCredentialsMatcher(Sha256CredentialsMatcher *matcher) {
        this->matcher = matcher;
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
    bool doGetAuthenticationInfo(const AuthenticationToken &token, AuthenticationInfo *);

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
    void onLogout(const std::string &sessionId) {
        //no-op, here for subclass override if desired.
    }

    virtual ~AuthenticatingRealm(){
        if(matcher){
            delete matcher;
            matcher = NULL;
        }
    }

private:
    /**
     * Password matcher used to determine if the provided password matches
     * the password stored in the data store.
     */
    Sha256CredentialsMatcher *matcher;
};

}
}
}
#endif //#if !definded(CETTY_SHIRO_REALM_AUTHENTICATINGREALM_H)
