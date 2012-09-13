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

#include <boost/function.hpp>
#include <cetty/shiro/realm/Realm.h>
#include <cetty/shiro/realm/AuthenticatingRealmPtr.h>
#include <cetty/shiro/authc/AuthenticationInfoPtr.h>

namespace cetty {
namespace shiro {
namespace authc {
class AuthenticationToken;
}
}
}

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
    typedef boost::function1<void, const AuthenticationInfoPtr&> GetAuthenticationInfoCallback;

public:
    AuthenticatingRealm() {}
    virtual ~AuthenticatingRealm() {
    }

    virtual void getAuthenticationInfo(const AuthenticationToken& token,
                                       const GetAuthenticationInfoCallback& callback) = 0;

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
    void onLogout(const std::string& sessionId) {
        //no-op, here for subclass override if desired.
    }

private:

};

}
}
}
#endif //#if !definded(CETTY_SHIRO_REALM_AUTHENTICATINGREALM_H)
