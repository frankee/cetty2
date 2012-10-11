#if !defined(CETTY_SHIRO_AUTHC_CREDENTIALSMATCHER_H)
#define CETTY_SHIRO_AUTHC_CREDENTIALSMATCHER_H
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

namespace cetty {
namespace shiro {
namespace authc {

class AuthenticationInfo;
class AuthenticationToken;

/**
 * Interface implemented by classes that can determine if an AuthenticationToken's provided
 * credentials matches a corresponding account's credentials stored in the system.
 *
 * <p>Simple direct comparisons are handled well by the
 * {@link SimpleCredentialsMatcher SimpleCredentialsMatcher}.  If you
 * hash user's credentials before storing them in a realm (a common practice), look at the
 * {@link HashedCredentialsMatcher HashedCredentialsMatcher} implementations,
 * as they support this scenario.
 *
 * @see SimpleCredentialsMatcher
 * @see AllowAllCredentialsMatcher
 * @see Md5CredentialsMatcher
 * @see Sha1CredentialsMatcher
 * @since 0.1
 */

class CredentialsMatcher{
public:
    virtual bool match(const AuthenticationToken& token, const AuthenticationInfo& info);

    virtual ~CredentialsMatcher(){}
};

}
}
}

#endif //#if !defined(CETTY_SHIRO_AUTHC_CREDENTIALSMATCHER_H)

// Local Variables:
// mode: c++
// End:
