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
    CryptoPP::StringSource ss((const byte *)in.c_str(), 
                              in.size(), 
                              true, 
                              new CryptoPP::HexEncoder(new CryptoPP::StringSink(hex))
                             );
    return hex;
}

std::string Hex::decode(const std::string &in){
    if(in.empty()) return std::string();

    CryptoPP::HexDecoder decoder;
    decoder.Put( (byte*)in.data(), in.size() );
    decoder.MessageEnd();

    size_t size = (size_t) decoder.MaxRetrievable();
    byte ch[size];
    memset(ch, 0x00, sizeof(ch));
    if(size) {
        decoder.Get(ch, size);
        return std::string((const char *)ch, size);
    }

    return std::string();
}

}
}
}


