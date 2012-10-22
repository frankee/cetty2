#if !defined(CETTY_SHIRO_UTIL_SECURITYUTILS_H)
#define CETTY_SHIRO_UTIL_SECURITYUTILS_H
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

#include <string>
#include <map>

#include <cetty/shiro/SecurityManager.h>
#include <cetty/shiro/authc/WSSE.h>

namespace cetty {
namespace shiro {
namespace util {

using namespace cetty::shiro;
using namespace cetty::shiro::authc;

class SecurityUtils {
public:
    static SecurityManager *getSecurityManager();
    static WSSE *getWsse();

private:
    SecurityUtils();
    SecurityUtils(SecurityUtils &);
    SecurityUtils &operator =(SecurityUtils &);
};

inline
SecurityManager *SecurityUtils::getSecurityManager(){
    return &SecurityManager::instance();
}

inline
WSSE *SecurityUtils::getWsse(){
    return &WSSE::instance();
}

}
}
}

#endif //#if !defined(CETTY_SHIRO_SECURITYUTILS_H)
