/*
 * SecurityUtils.cpp
 *
 *  Created on: 2012-8-22
 *      Author: chenhl
 */

#include <cetty/shiro/util/SecurityUtils.h>

#include <cstdlib>

#include <cetty/logging/LoggerHelper.h>
#include <cetty/shiro/crypt/DigestEngine.h>

namespace cetty {
namespace shiro {
namespace util {

const std::string SecurityUtils::SECURITY_MANAGER = "security_manager";
const std::string SecurityUtils::LOGIN_UTIL = "login_util";
const std::string SecurityUtils::MD5_ENGINE = "md5_engine";
const std::string SecurityUtils::SHA1_ENGINE = "sha1_engine";

SecurityUtils *SecurityUtils::instance = NULL;

SecurityUtils::SecurityUtils(){
    readConfigure();
    configure();
}

SecurityUtils *SecurityUtils::getInstance(){
    if(instance == NULL){
        instance = new SecurityUtils();
    }

    return instance;
}

void SecurityUtils::readConfigure(){

}

void SecurityUtils::configure(){

}

SecurityManager *SecurityUtils::getSecurityManager(){
    SecurityManager *manager = NULL;

    std::map<std::string, void *>::iterator it =
        objects.find(SecurityUtils::SECURITY_MANAGER);
    if(it != objects.end())
        manager = (SecurityManager *)it->second;
    else
        LOG_ERROR << "No security manager is found,"
                     "you must configure one.";

    return manager;
}

DigestEngine *SecurityUtils::getDigestEngine(){
    DigestEngine *engine = NULL;

    std::string hashAlgorithm;
    getValue("hash_algorithm", &hashAlgorithm);

    if(!hashAlgorithm.empty()){
        LOG_TRACE << "Using configured hash algorithm"
                  <<"[" << hashAlgorithm << "]";

        if("md5" == hashAlgorithm){
            engine = (DigestEngine *)getObject(SecurityUtils::MD5_ENGINE);
        }
        else if("sha1" == hashAlgorithm){
            engine = (DigestEngine *)getObject(SecurityUtils::SHA1_ENGINE);
        }
        else{
            LOG_TRACE << "Can't find algorithm for ["
                      << hashAlgorithm << "]";
        }
    }

    if(engine == NULL){
        LOG_TRACE << "use default hash algorithm[SHA1]";
        engine = (DigestEngine *)getObject(SecurityUtils::SHA1_ENGINE);
    }

    return engine;
}

DigestEngine *SecurityUtils::getDigestEngine(const std::string &engineName){
    DigestEngine *engine = NULL;

    if(engineName.empty() || ("md5" != engineName && "sha1" != engineName))
        engine = getDigestEngine();
    else {
        if("md5" != engineName){
            LOG_TRACE << "use [MD5] hash algorithm";
            engine = (DigestEngine *)getObject(SecurityUtils::MD5_ENGINE);
        }
        else{
            LOG_TRACE << "use [SHA1] hash algorithm";
            engine = (DigestEngine *)getObject(SecurityUtils::SHA1_ENGINE);
        }
    }

    return engine;
}

void SecurityUtils::getValue(const std::string &key, std::string *value){
    if(key.empty()){
        LOG_TRACE << "Key can't be empty!";
        return;
    }
    if(value == NULL){
        LOG_TRACE << "value can't NULL!";
        return;
    }

    std::map<std::string, std::string>::iterator it =
        configuration.find(key);
    if(it != configuration.end())
        value->assign(it->second);
}

void *SecurityUtils::getObject(const std::string &name){
    void *object = NULL;

    if(name.empty()){
        LOG_TRACE << "Name can't be empty!";
        return object;
    }

    std::map<std::string, void *>::iterator it = objects.find(name);
    if(it != objects.end()) object = it->second;

    return object;
}

}
}
}


