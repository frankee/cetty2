/*
 * Sha256CredentialsMatcher.cpp
 *
 *  Created on: 2012-8-17
 *      Author: chenhl
 */
#include <cetty/shiro/authc/HashedCredentialsMatcher.h>

#include <boost/assert.hpp>

#include <cetty/shiro/crypt/DigestEngine.h>
#include <cetty/shiro/authc/AuthenticationInfo.h>
#include <cetty/shiro/authc/AuthenticationToken.h>
#include <cetty/shiro/crypt/DigestEngine.h>
#include <cetty/shiro/crypt/MD5Engine.h>
#include <cetty/shiro/crypt/SHA1Engine.h>

#include <cetty/logging/LoggerHelper.h>
#include <cetty/config/ConfigCenter.h>

namespace cetty {
namespace shiro {
namespace authc {

using namespace cetty::shiro::crypt;

const std::string HashedCredentialsMatcher::MD5ENGINE = "md5_engine";
const std::string HashedCredentialsMatcher::SHA1ENGINE = "sha1_engine";

HashedCredentialsMatcher::HashedCredentialsMatcher()
    : digestEngine(NULL){
    ConfigCenter::instance().configure(&config);
    init();
}

void HashedCredentialsMatcher::init(){
    if (config.hashIterations < 1) {
        config.hashIterations = 1;
    }

    if(config.hashAlgorithm == MD5ENGINE){
        digestEngine = new MD5Engine();
   } else if(config.hashAlgorithm == SHA1ENGINE){
       digestEngine = new SHA1Engine();
   } else {
       digestEngine = new SHA1Engine();
   }
}

bool HashedCredentialsMatcher::match(const AuthenticationToken& token,
                                     const AuthenticationInfo& info) {
    std::string tokenCredentials;
    std::string accountCredentials;

    getCredentials(token, &tokenCredentials);
    hashProvidedCredentials(token, info, &accountCredentials);

    return tokenCredentials == accountCredentials;
}

void HashedCredentialsMatcher::getCredentials(const AuthenticationInfo& info,
                                              std::string* credentials) {
    assert(credentials != NULL);

    const std::string& infoCredentials = info.getCredentials();

    if (isStoredCredentialsHexEncoded()) {
        DigestEngine::digestFromHex(infoCredentials, credentials);
    } else {
        DigestEngine::digestFromBase64(infoCredentials, credentials);
    }
}

void HashedCredentialsMatcher::getCredentials(const AuthenticationToken &token,
                                              std::string* credentials) {
    if (credentials != NULL) {
        if (isStoredCredentialsHexEncoded()){
            DigestEngine::digestFromHex(token.getCredentials(), credentials);
        } else {
            DigestEngine::digestFromBase64(token.getCredentials(), credentials);
        }
    }
}

void HashedCredentialsMatcher::hashProvidedCredentials(const AuthenticationToken& token,
                                                       const AuthenticationInfo& info,
                                                       std::string* credentials) {
    if (!credentials) {
        return;
    }

    std::string salt = info.getCredentialsSalt();
    if (salt.empty() && isHashSalted()) {
        salt = token.getSalt();
    }

    std::string c;
    getCredentials(info, &c);
    c.append(salt);

    DigestEngine* digestEngine = getDigestEngine();
    if (!digestEngine) {
        BOOST_ASSERT(false);
        LOG_ERROR << "has no proper digest engine.";
        return;
    }

    digestEngine->reset();
    digestEngine->update(c);
    DigestEngine::Digest digest = digestEngine->digest();

    int iterations = config.hashIterations - 1; //already hashed once above
    //iterate remaining number:
    for (int i = 0; i < iterations; i++) {
        digestEngine->reset();
        digestEngine->update((const void*)digest.data(), (int)digest.size());
        digest = digestEngine->digest();
    }

    credentials->assign(digest.begin(), digest.end());
}

DigestEngine* HashedCredentialsMatcher::getDigestEngine() {
    return digestEngine;
}

HashedCredentialsMatcher::~HashedCredentialsMatcher(){
    if(digestEngine){
        delete digestEngine;
        digestEngine = NULL;
    }
}

}
}
}
