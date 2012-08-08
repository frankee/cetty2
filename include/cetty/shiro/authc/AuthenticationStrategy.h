#if !defined(CETTY_SHIRO_AUTHENTICATIONSTRATEGY_H)
#define CETTY_SHIRO_AUTHENTICATIONSTRATEGY_H
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

namespace cetty { namespace shiro { namespace realm {
    class AuthenticingRealm;
} }}

namespace cetty {
namespace shiro {
namespace authc {

using namespace cetty::shiro::realm;
/**
 * Abstract base implementation for Shiro's concrete <code>AuthenticationStrategy</code>
 * implementations.
 *
 * @since 0.9
 */
class AuthenticationStrategy {
public:
    /**
     * Simply returns <code>new {@link org.apache.shiro.authc.SimpleAuthenticationInfo SimpleAuthenticationInfo}();</code>, which supports
     * aggregating account data across realms.
     */
    virtual bool beforeAllAttempts(const std::vector<AuthenticatingRealm> &realms, const AuthenticationToken &token, AuthenticationInfo *info);

    /**
     * Simply returns the <code>aggregate</code> method argument, without modification.
     */
    virtual bool beforeAttempt(AuthenticatingRealm realm, AuthenticationToken token, AuthenticationInfo aggregate, AuthenticationInfo *info);
    /**
     * Base implementation that will aggregate the specified <code>singleRealmInfo</code> into the
     * <code>aggregateInfo</code> and then returns the aggregate.  Can be overridden by subclasses for custom behavior.
     */
    virtual bool afterAttempt(const AuthenticatingRealm &realm,
        const AuthenticationToken &token,
        const AuthenticationInfo &singleRealmInfo,
        const AuthenticationInfo &aggregateInfo,
        AuthenticationInfo *info);

    /**
     * Merges the specified <code>info</code> argument into the <code>aggregate</code> argument and then returns an
     * aggregate for continued use throughout the login process.
     * <p/>
     * This implementation merely checks to see if the specified <code>aggregate</code> argument is an instance of
     * {@link org.apache.shiro.authc.MergableAuthenticationInfo MergableAuthenticationInfo}, and if so, calls
     * <code>aggregate.merge(info)</code>  If it is <em>not</em> an instance of
     * <code>MergableAuthenticationInfo</code>, an {@link IllegalArgumentException IllegalArgumentException} is thrown.
     * Can be overridden by subclasses for custom merging behavior if implementing the
     * {@link org.apache.shiro.authc.MergableAuthenticationInfo MergableAuthenticationInfo} is not desired for some reason.
     */
    virtual bool merge(AuthenticationInfo info, AuthenticationInfo aggregate, AuthenticationInfo *info);

    /**
     * Simply returns the <code>aggregate</code> argument without modification.  Can be overridden for custom behavior.
     */
    virtual bool afterAllAttempts(AuthenticationToken token, AuthenticationInfo aggregate, AuthenticationInfo *info);
    virtual ~AuthenticationStrategy();
};

}
}
}

#endif //#if !defined(CETTY_SHIRO_AUTHENTICATIONSTRATEGY_H)
