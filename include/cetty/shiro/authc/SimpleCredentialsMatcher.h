#if !defined(CETTY_SHIRO_AUTHC_SIMPLECREDENTIALSMATCHER_H)
#define CETTY_SHIRO_AUTHC_SIMPLECREDENTIALSMATCHER_H

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

#include <cetty/shiro/authc/AuthenticationInfo.h>
#include <cetty/shiro/authc/AuthenticationToken.h>

namespace cetty {
namespace shiro {
namespace authc {

/**
 * Simple CredentialsMatcher implementation.  Supports direct (plain) comparison for credentials of type
 * byte[], char[], and Strings, and if the arguments do not match these types, then reverts back to simple
 * <code>Object.equals</code> comparison.
 * <p/>
 * <p>Hashing comparisons (the most common technique used in secure applications) are not supported by this class, but
 * instead by the {@link org.apache.shiro.authc.credential.HashedCredentialsMatcher HashedCredentialsMatcher}.
 *
 * @see org.apache.shiro.authc.credential.HashedCredentialsMatcher
 * @since 0.9
 */
class SimpleCredentialsMatcher {
public:
    /**
     * This implementation acquires the {@code token}'s credentials
     * (via {@link #getCredentials(AuthenticationToken) getCredentials(token)})
     * and then the {@code account}'s credentials
     * (via {@link #getCredentials(org.apache.shiro.authc.AuthenticationInfo) getCredentials(account)}) and then passes both of
     * them to the {@link #equals(Object,Object) equals(tokenCredentials, accountCredentials)} method for equality
     * comparison.
     *
     * @param token the {@code AuthenticationToken} submitted during the authentication attempt.
     * @param info  the {@code AuthenticationInfo} stored in the system matching the token principal.
     * @return {@code true} if the provided token credentials are equal to the stored account credentials,
     *         {@code false} otherwise
     */
    static bool match(const AuthenticationToken& token, const AuthenticationInfo& info) {
        return token.getCredentials() == info.getCredentials();
    }
};

}
}
}

#endif //#if !defined(CETTY_SHIRO_AUTHC_SIMPLECREDENTIALSMATCHER_H)

// Local Variables:
// mode: c++
// End:
