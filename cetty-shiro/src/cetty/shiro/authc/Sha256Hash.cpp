/*
 * Sha64Hash.cpp
 *
 *  Created on: 2012-8-20
 *      Author: chenhl
 */
#include <cetty/shiro/authc/Sha256Hash.h>

#include <sha.h>

#include <cstring>

namespace cetty {
namespace shiro {
namespace authc {

const int Sha256Hash::SHA_DIGEST_LEN = 32;

// MessageDigest Sha256Hash::getDigest(){ return MessageDigest(); };

std::string Sha256Hash::hash(const std::string &msg, const std::string &salt, int hashIterations){
    if(msg.empty()) return std::string();

    std::string imsg(msg);
    imsg.append(salt);

    byte digest[SHA_DIGEST_LEN + 1];
    byte temp[SHA_DIGEST_LEN + 1];

    CryptoPP::SHA256 sha256;
    sha256.CalculateDigest(digest, (byte *)(imsg.c_str()), imsg.size());
    for(--hashIterations; hashIterations > 0; --hashIterations){
        memcpy(temp, digest, sizeof(digest));
        sha256.CalculateDigest(digest, temp, SHA_DIGEST_LEN);
    }

    this->bytes.assign((const char *) digest, SHA_DIGEST_LEN);
    return getBytes();
}

}
}
}


