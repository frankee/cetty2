#if !defined(CETTY_SHIRO_FIRSTSUCCESSFULSTRATEGY_H)
#define CETTY_SHIRO_FIRSTSUCCESSFULSTRATEGY_H
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
#include <cetty/shiro/authc/AuthenticationStrategy.h>

namespace cetty {
namespace shiro {
namespace authc {


/**
 * {@link AuthenticationStrategy} implementation that only accepts the account data from
 * the first successfully consulted Realm and ignores all subsequent realms.  This is slightly
 * different behavior than {@link AtLeastOneSuccessfulStrategy}, so please review both to see
 * which one meets your needs better.
 *
 * @see AtLeastOneSuccessfulStrategy AtLeastOneSuccessfulAuthenticationStrategy
 * @since 0.9
 */
class FirstSuccessfulStrategy : public AuthenticationStrategy {
public:
    /**
     * Returns {@code null} immediately, relying on this class's {@link #merge merge} implementation to return
     * only the first {@code info} object it encounters, ignoring all subsequent ones.
     */
    virtual bool beforeAllAttempts(const std::vector<AuthenticatingRealm> &realms, const AuthenticationToken &token, AuthenticationInfo *info);

    /**
     * Returns the specified {@code aggregate} instance if is non null and valid (that is, has principals and they are
     * not empty) immediately, or, if it is null or not valid, the {@code info} argument is returned instead.
     * <p/>
     * This logic ensures that the first valid info encountered is the one retained and all subsequent ones are ignored,
     * since this strategy mandates that only the info from the first successfully authenticated realm be used.
     */
    virtual bool merge(AuthenticationInfo info, AuthenticationInfo aggregate, AuthenticationInfo *info);
    virtual ~FirstSuccessfulStrategy();
};
}
}
}
#endif //#if !defined(CETTY_SHIRO_FIRSTSUCCESSFULSTRATEGY_H)
