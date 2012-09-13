/*
 * AuthenticationToken.cpp
 *
 *  Created on: 2012-8-17
 *      Author: chenhl
 */

#include <cetty/shiro/authc/AuthenticationToken.h>
#include <cetty/util/StringUtil.h>

namespace cetty {
namespace shiro {
namespace authc {

using namespace cetty::util;

void AuthenticationToken::clear() {
    rememberMe = false;
    principal.clear();
    credentials.clear();
    host.clear();
}

std::string AuthenticationToken::toString() {
    return StringUtil::strprintf("principal: %s host: %s rememberMe: ",
                                 principal.c_str(),
                                 host.c_str(),
                                 rememberMe ? "true" : "false");
}

}
}
}


