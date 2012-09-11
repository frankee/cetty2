/*
 * LoginUtil.cpp
 *
 *  Created on: 2012-9-11
 *      Author: chenhl
 */

#include <cetty/shiro/util/LoginUtil.h>

namespace cetty {
namespace shiro {
namespace util {

const int LoginUtil::NO_SESSION_CODE = 1;
const int LoginUtil::LOGIN_FAILED_CODE = 2;
const int LoginUtil::LOGIN_REFUSED_CODE = 3;

const std::string LoginUtil::NO_SESSION_MESSAGE = "Can't create session";
const std::string LoginUtil::LOGIN_FAILED_MESSAGE = "Wrong user name or password";
const std::string LoginUtil::LOGIN_REFUSED_MESSAGE = "Refuse login";

}
}
}


