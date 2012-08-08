#if !defined(CETTY_SHIRO_ALLSUCCESSFULSTRATEGY_H)
#define CETTY_SHIRO_ALLSUCCESSFULSTRATEGY_H
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

namespace cetty {
namespace shiro {
namespace authc {
/**
 * <tt>AuthenticationStrategy</tt> implementation that requires <em>all</em> configured realms to
 * <b>successfully</b> process the submitted <tt>AuthenticationToken</tt> during the log-in attempt.
 * <p/>
 * <p>If one or more realms do not support the submitted token, or one or more are unable to acquire
 * <tt>AuthenticationInfo</tt> for the token, this implementation will immediately fail the log-in attempt for the
 * associated subject (user).
 *
 * @since 0.2
 */
class AllSuccessfulStrategy : public AuthenticationStrategy {
public:
    /**
     * Because all realms in this strategy must complete successfully, this implementation ensures that the given
     * <code>Realm</code> {@link org.apache.shiro.realm.Realm#supports(org.apache.shiro.authc.AuthenticationToken) supports} the given
     * <code>token</code> argument.  If it does not, this method throws an
     * {@link UnsupportedTokenException UnsupportedTokenException} to end the authentication
     * process immediately. If the realm does support the token, the <code>info</code> argument is returned immediately.
     */
    virtual bool beforeAttempt(AuthenticatingRealm realm, AuthenticationToken token, AuthenticationInfo aggregate, AuthenticationInfo *info);
    /**
     * Merges the specified <code>info</code> into the <code>aggregate</code> argument and returns it (just as the
     * parent implementation does), but additionally ensures the following:
     * <ol>
     * <li>if the <code>Throwable</code> argument is not <code>null</code>, re-throws it to immediately cancel the
     * authentication process, since this strategy requires all realms to authenticate successfully.</li>
     * <li>neither the <code>info</code> or <code>aggregate</code> argument is <code>null</code> to ensure that each
     * realm did in fact authenticate successfully</li>
     * </ol>
     */
    virtual bool afterAttempt(const AuthenticatingRealm &realm,
            const AuthenticationToken &token,
            const AuthenticationInfo &singleRealmInfo,
            const AuthenticationInfo &aggregateInfo,
            AuthenticationInfo *info);
};
}
}
}

//#endif /#if !defined(CETTY_SHIRO_ALLSUCCESSFULSTRATEGY_H)
