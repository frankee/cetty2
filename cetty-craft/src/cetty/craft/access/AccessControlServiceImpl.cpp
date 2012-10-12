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

#include <cetty/craft/access/AccessControlServiceImpl.h>

#include <cetty/shiro/SecurityManager.h>
#include <cetty/shiro/authc/WSSE.h>
#include <cetty/shiro/authc/AuthenticationToken.h>

#include <cetty/craft/craft.pb.h>

namespace cetty {
namespace craft {
namespace access {

using namespace cetty::shiro;
using namespace cetty::shiro::authc;

void AccessControlServiceImpl::preLogin(const ConstPreLoginRequestPtr& request,
                                        const PreLoginResponsePtr& response,
                                        const DoneCallback& done) {
    WSSE::instance().generatorSecret(request->user_name(),
                                     request->host(),
                                     response->mutable_nonce(),
                                     response->mutable_server_time());

    done(response);
}

void AccessControlServiceImpl::login(const ConstLoginRequestPtr& request,
                                     const LoginResponsePtr& response,
                                     const DoneCallback& done) {

    const std::string& principal = request->user_name();
    const std::string& digest = request->encoded_passwd();
    const std::string& nonce = request->nonce();
    const std::string& created = request->server_time();
    const std::string& host = request->host();

    if (WSSE::instance().verifySecret(host, principal, nonce, created)) {
        AuthenticationToken token(principal, digest, host);
        token.setSalt(nonce + created);

        SecurityManager::instance().login(token,
                                          boost::bind(&AccessControlServiceImpl::onLogin,
                                                  this,
                                                  _1,
                                                  _2,
                                                  _3,
                                                  _4,
                                                  response,
                                                  done));
    }
    else {
        // refuse login
        // todo add error code
        done(response);
    }
}

void AccessControlServiceImpl::onLogin(int code,
                                       const AuthenticationToken& token,
                                       const AuthenticationInfoPtr& info,
                                       const SessionPtr& session,
                                       const LoginResponsePtr& response,
                                       const DoneCallback& done) {
    if (!code) {
        if (session) {
            cetty::craft::Session* s = response->mutable_session();
            s->set_id(session->getId());
        }
        else {
            // Login success but no session
            // todo add dealing code
        } // if(sessionPtr)
    }
    else {
        // Login failed
        // todo add dealing code
    } // if(!code)

    done(response);
}

void AccessControlServiceImpl::logout(const ConstLogoutRequestPtr& request,
                                      const LogoutResponsePtr& response,
                                      const DoneCallback& done) {
    SecurityManager::instance().logout(request->session().id());
}

}
}
}
