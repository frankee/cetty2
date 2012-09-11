/*
 * Sha256CredentialsMatcher.cpp
 *
 *  Created on: 2012-8-17
 *      Author: chenhl
 */
#include <cetty/shiro/authc/Sha256CredentialsMatcher.h>
#include <cetty/shiro/authc/Sha256Hash.h>

#include <cetty/shiro/codec/Hex.h>
#include <cetty/shiro/codec/Base64.h>

namespace cetty {
namespace shiro {
namespace authc {

using namespace cetty::shiro::codec;

bool Sha256CredentialsMatcher::doCredentialsMatch(
    const AuthenticationToken &token,
    const AuthenticationInfo &info)
{
    /*
   std::string tokenHashedCredentials = hashProvidedCredentials(token, info);
   std::string accountCredentials = getCredentials(info);
   */
    std::string tokenCredentials = token.getCredentials();
    std::string infoCredentials = getCredentials(info);
    infoCredentials.append(token.getNonce());
    infoCredentials.append(token.getServerTime());

    std::string credentials = hashProvidedCredentials(infoCredentials, std::string(), 1);

   return equals(tokenCredentials, infoCredentials);
   }

std::string Sha256CredentialsMatcher::getCredentials(const AuthenticationInfo &info){
    const std::string credentials = info.getCredentials();
    std::string decodeString;

    if (isStoredCredentialsHexEncoded()) {
        return decodeString = Hex::decode(credentials);
    } else {
        return decodeString = Base64::decode(credentials);
    }
}

bool Sha256CredentialsMatcher::equals(
    const std::string &tokenCredentials,
    const std::string &accountCredentials)
{
    return tokenCredentials == accountCredentials;
}

std::string Sha256CredentialsMatcher::hashProvidedCredentials(
    const AuthenticationToken &token,
    const AuthenticationInfo &info)
{
    std::string salt;
    if (isHashSalted()) {
        salt = getSalt(token);
    }
    return hashProvidedCredentials(token.getCredentials(), salt, getHashIterations());
}

std::string Sha256CredentialsMatcher::hashProvidedCredentials(
    const std::string &credentials,
    const std::string &salt,
    int hashIterations)
{
    Sha256Hash hash(credentials, salt, hashIterations);
    return hash.getBytes();
}

}
}
}


