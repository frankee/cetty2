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

namespace cetty {
namespace shiro {
    class SecurityManager;
}}

namespace cetty {
namespace shiro {
namespace crypt {
    class DigestEngine;
}}}

namespace cetty {
namespace shiro {
namespace util {

class LoginUtil;

using namespace cetty::shiro;
using namespace cetty::shiro::crypt;

class SecurityUtils {
public:
    static SecurityUtils *getInstance();

    SecurityManager *getSecurityManager();
    LoginUtil *getLoginUtil();

    DigestEngine *getDigestEngine();
    DigestEngine *getDigestEngine(const std::string &engineName);

public:
    static const std::string SECURITY_MANAGER;
    static const std::string LOGIN_UTIL;
    static const std::string MD5_ENGINE;
    static const std::string SHA1_ENGINE;

private:
    SecurityUtils();

    /// @brief read configuration file
    /// @param path configuration file path which must be absolute.
    void readConfigure();

    /// @brief configure object crossing configuration
    void configure();

    /// @brief Get configuration item which has name key
    void getValue(const std::string &key, std::string *value);
    /// @brief Get object which has name #name
    void *getObject(const std::string &name);

private:
    /// map name to object
    /// The objects in #objects are established crossing
    /// configure file when system is launched.
   std::map<std::string, void *> objects;

   /// configuration item from file
   std::map<std::string, std::string> configuration;

   static SecurityUtils *instance;

};

}
}
}
#endif //#if !defined(CETTY_SHIRO_SECURITYUTILS_H)
