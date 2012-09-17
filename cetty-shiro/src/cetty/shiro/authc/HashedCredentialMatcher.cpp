/*
 * Sha256CredentialsMatcher.cpp
 *
 *  Created on: 2012-8-17
 *      Author: chenhl
 */
#include <cetty/shiro/authc/HashedCredentialMatcher.h>

#include <boost/assert.hpp>

#include <cetty/shiro/crypt/DigestEngine.h>
#include <cetty/shiro/authc/AuthenticationInfo.h>
#include <cetty/shiro/authc/AuthenticationToken.h>

#include <cetty/logging/LoggerHelper.h>

namespace cetty {
namespace shiro {
namespace authc {

using namespace cetty::shiro::crypt;

bool HashedCredentialsMatcher::match(
    const AuthenticationToken& token,
    const AuthenticationInfo& info) {

    std::string tokenCredentials;
    std::string accountCredentials;

    getCredentials(info, &accountCredentials);
    hashProvidedCredentials(token, info, &tokenCredentials);

    return tokenCredentials == accountCredentials;
}

void HashedCredentialsMatcher::getCredentials(const AuthenticationInfo& info,
        std::string* credentials) {

    const std::string& infoCredentials = info.getCredentials();

    if (isStoredCredentialsHexEncoded()) {
        DigestEngine::digestFromHex(infoCredentials, credentials);
    }
    else {
        DigestEngine::digestFromBase64(infoCredentials, credentials);
    }
}

void HashedCredentialsMatcher::getCredentials(const AuthenticationToken &token, std::string* credentials) {
    if (credentials) {
        *credentials = token.getCredentials();
    }
}

void HashedCredentialsMatcher::hashProvidedCredentials(
    const AuthenticationToken& token,
    const AuthenticationInfo& info,
    std::string* credentials) {
    if (!credentials) {
        return;
    }

    std::string salt = info.getCredentialsSalt();
    
    if (salt.empty() && isHashSalted()) {
        salt = token.getSalt();
    }

    std::string c = token.getCredentials();
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

    int iterations = hashIterations - 1; //already hashed once above
    //iterate remaining number:
    for (int i = 0; i < iterations; i++) {
        digestEngine->reset();
        digestEngine->update((const void*)digest.data(), (int)digest.size());
        digest = digestEngine->digest();
    }

    credentials->assign(digest.begin(), digest.end());
}

DigestEngine* HashedCredentialsMatcher::getDigestEngine() {
    return NULL;
}


}
}
}


