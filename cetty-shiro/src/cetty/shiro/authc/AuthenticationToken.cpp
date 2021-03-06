/*
 * AuthenticationToken.cpp
 *
 *  Created on: 2012-8-17
 *      Author: chenhl
 */

#include <cetty/shiro/authc/AuthenticationToken.h>
#include <cetty/util/StringUtil.h>
#include <cstdio>

namespace cetty {
namespace shiro {
namespace authc {

using namespace cetty::util;

const std::string& AuthenticationToken::getCredentials() const {
    return credentials;
}


std::string AuthenticationToken::toString() {
    return StringUtil::printf("principal: %s host: %s rememberMe: %s",
                                 principal.c_str(),
                                 host.c_str(),
                                 rememberMe ? "true" : "false");
}

}
}
}
