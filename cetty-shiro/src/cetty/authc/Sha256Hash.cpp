/*
 * Sha64Hash.cpp
 *
 *  Created on: 2012-8-20
 *      Author: chenhl
 */
#include <cetty/shiro/authc/Sha256Hash.h>

namespace cetty {
namespace shiro {
namespace authc {
// MessageDigest Sha256Hash::getDigest(){ return MessageDigest(); };

std::string Sha256Hash::hash(const std::string &bytes, const std::string &salt, int hashIterations){
    this->bytes = bytes;
    return getBytes();
}
}
}
}


