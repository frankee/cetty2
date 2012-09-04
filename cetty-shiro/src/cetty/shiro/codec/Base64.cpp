/*
 * Base64.cpp
 *
 *  Created on: 2012-8-30
 *      Author: chenhl
 */



#include <cetty/shiro/codec/Base64.h>

#include <base64.h>
#include <filters.h>

namespace cetty {
namespace shiro {
namespace codec {

std::string Base64::encode(const std::string &in, bool isChunked){
    if(in.empty() || isChunked) return in;
    std::string encoded;
    CryptoPP::StringSource ss((const byte *)in.c_str(),
                              in.size(),
                              true,
                              new CryptoPP::Base64Encoder(new CryptoPP::StringSink(encoded))
                             );

    return encoded;
}

std::string Base64::decode(const std::string &in){
    if(in.empty()) return std::string();

    CryptoPP::Base64Decoder decoder;
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

