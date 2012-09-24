
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
#include <cetty/shiro/authc/AuthenticationToken.h>
#include <cetty/shiro/util/LoginUtil.h>
#include <cetty/shiro/SecurityManager.h>

namespace cetty {
namespace craft {
namespace access {

using namespace cetty::shiro::authc;
using namespace cetty::shiro::util;
using namespace cetty::shiro;

void AccessControlServiceImpl::preLogin(const ConstPreLoginRequestPtr& request,
                                        const PreLoginResponsePtr& response,
                                        const DoneCallback& done){
    std::string host = request->host();
    std::string userName = request->user_name();

    LoginUtil *loginUtil = (SecurityUtils::getInstance())->getLoginUtil();
    std::string nonce = loginUtil->getNonce();
    std::string serverTime = loginUtil->getServerTime();

    response->set_nonce(nonce);
    response->set_server_time(serverTime);

    loginUtil->saveNonceServerTime(userName, host, nonce, serverTime);
    done(response);
}

void AccessControlServiceImpl::login(const ConstLoginRequestPtr& request,
                                     const LoginResponsePtr& response,
                                     const DoneCallback& done) {
    const std::string& userName = request->user_name();
    const std::string& nonce = request->nonce();
    const std::string& serverTime = request->server_time();
    const std::string& encodeType = request->encode_type();
    const std::string& encodedPasswd = request->encoded_passwd();

    LoginUtil *loginUtil = (SecurityUtils::getInstance())->getLoginUtil();

    std::string storeHost = loginUtil->getHost(userName, serverTime);
    std::string storeNonce = loginUtil->getNonce(userName, serverTime);
    std::string storeServerTime = loginUtil->getServerTime(userName, serverTime);

    if (nonce == storeNonce &&
        serverTime == storeServerTime &&
        loginUtil->verifyServerTime(serverTime, loginUtil->getServerTime())) {

        AuthenticationToken token(userName, encodedPasswd, storeHost);
        token.setEncodeType(encodeType);
        token.setNonce(nonce);
        token.setServerTime(serverTime);

        SecurityManager *sm = (SecurityUtils::getInstance())->getSecurityManager();
        sm->login(token, boost::bind(&AccessControlServiceImpl::onLogin,
                                     this,
                                     _1,
                                     _2,
                                     _3,
                                     _4,
                                     response,
                                     done));
    }
    else {
        cetty::protobuf::service::Status *status = response->mutable_status();

        //todo modify code and message
        status->set_code(0x00);
        status->set_message(LoginUtil::LOGIN_REFUSED_MESSAGE);
        done(response);
    } // end
}

void AccessControlServiceImpl::onLogin(int code,
                                       const AuthenticationToken& token,
                                       const AuthenticationInfoPtr& info,
                                       const SessionPtr& session,
                                       const LoginResponsePtr& response,
                                       const DoneCallback& done){
    if(!code){
        if(sessionPtr){
            cetty::protobuf::service::Session *session = response->mutable_session();
            session->set_id(session->getId().c_str(););
            cetty::protobuf::service::KeyValue *item = session->add_items();
            tem->set_key("username");
            item->set_value(userName);
        }
        else{
            cetty::protobuf::service::Status *status = response->mutable_status();
            //status->set_status();
            // todo modify status code and status message
            status->set_code(code);
            status->set_message("");
        }
    }else{
        cetty::protobuf::service::Status *status = response->mutable_status();
        //status->set_status();
        // todo modify status code and status message
        status->set_code(code);
        status->set_message("");
    }
    done(response);
}

void AccessControlServiceImpl::logout(const ConstLogoutRequestPtr& request,
                                      const LogoutResponsePtr& response,
                                      const DoneCallback& done) {
    cetty::protobuf::service::Session session = request->session();
    std::string sessionId = session.id();
    SecurityManager *sm = (SecurityUtils::getInstance())->getSecurityManager();
    sm->logout(sessionId);
}

}
}
}
