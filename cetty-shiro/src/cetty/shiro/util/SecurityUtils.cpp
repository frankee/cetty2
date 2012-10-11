/*
 * SecurityUtils.cpp
 *
 *  Created on: 2012-8-22
 *      Author: chenhl
 */

#include <cetty/shiro/util/SecurityUtils.h>

#include <cstdlib>

#include <cetty/logging/LoggerHelper.h>

namespace cetty {
namespace shiro {
namespace util {

SecurityManager SecurityUtils::securityManager;
WSSE SecurityUtils::wsse;

}
}
}


