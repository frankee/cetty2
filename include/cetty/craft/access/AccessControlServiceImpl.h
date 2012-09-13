#if !defined(CETTY_CRAFT_ACCESS_ACCESSCONTROLSERVICEIMPL_H)
#define CETTY_CRAFT_ACCESS_ACCESSCONTROLSERVICEIMPL_H

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

#include <cetty/craft/access/AccessControl.pb.h>

namespace cetty {
namespace craft {
namespace access {

class AccessControlServiceImpl : public AccessControlService {
public:
    AccessControlServiceImpl() {}
    virtual ~AccessControlServiceImpl() {}

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

#endif //#if !defined(CETTY_CRAFT_ACCESS_ACCESSCONTROLSERVICEIMPL_H)

// Local Variables:
// mode: c++
// End:
