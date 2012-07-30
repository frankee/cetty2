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
#ifndef AUTHENTICATOR_HPP_
#define AUTHENTICATOR_HPP_

namespace cetty { namespace shiro {
    class AuthenticationToken;
    class AuthenticationInfo;
}}

namespace cetty {
namespace shiro {

/**
 * An Authenticator is responsible for authenticating accounts in an application.  It
 * is one of the primary entry points into the Shiro API.
   */
class Authenticator {

    /**
  * Authenticates a user based on the submitted AuthenticationToken.
  * @param authenticationToken Representation of a user's principals and credentials submitted during an
  *                            authentication attempt.
  * @param AuthenticationInfo  Representing the authenticating user's account data
  * @return If the authentication is successful, return true.
     */
public:
    virtual bool authenticate(const AuthenticationToken&, AuthenticationInfo*) = 0;
    virtual ~Authenticator(){}
};

}
}

#endif /* AUTHENTICATOR_HPP_ */
