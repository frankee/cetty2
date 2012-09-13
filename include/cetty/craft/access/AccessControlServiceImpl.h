#if !defined(CETTY_SHIRO_WEB_AUTHENTICATIONSERVICEIMPL_H)
#define CETTY_SHIRO_WEB_AUTHENTICATIONSERVICEIMPL_H

/*
 * Copyright (c) 2010-2012 frankee zhou (frankee.zhou at gmail dot com)
 *
 * Distributed under under the Apache License, version 2.0 (the "License").
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 */

#include <cetty/shiro/web/authentication.pb.h>

namespace cetty {
namespace shiro {
namespace web {

class AuthenticationServiceImpl : public cetty::shiro::web::AuthenticationService {
public:
    AuthenticationServiceImpl() {}
    virtual ~AuthenticationServiceImpl() {}

    virtual void preLogin(const ConstPreLoginRequestPtr& request,
                          const PreLoginResponsePtr& response,
                          const DoneCallback& done);

    virtual void login(const ConstLoginRequestPtr& request,
                       const LoginResponsePtr& response,
                       const DoneCallback& done);

    virtual void logout(const ConstLogoutRequestPtr& request,
                        const LogoutResponsePtr& response,
                        const DoneCallback& done);
};

}
}
}

#endif //#if !defined(CETTY_SHIRO_WEB_AUTHENTICATIONSERVICEIMPL_H)

// Local Variables:
// mode: c++
// End:
