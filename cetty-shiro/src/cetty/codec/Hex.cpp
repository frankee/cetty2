/*
 * Hex.cpp
 *
 *  Created on: 2012-8-29
 *      Author: chenhl
 */

#include <cetty/shiro/codec/Hex.h>

#include <hex.h>
#include <filters.h>

namespace cetty {
namespace shiro {
namespace codec {

std::string Hex::encode(const std::string &in){
    std::string hex;
    CryptoPP::StringSource ss((const byte *)in.c_str(), in.size(), true, new CryptoPP::HexEncoder(new CryptoPP::StringSink(hex)));
    return hex;
}

std::string Hex::decode(const std::string &in){
    return std::string();
}

}
}
}


