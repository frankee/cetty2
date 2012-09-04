/*
 * AuthenticationToken.cpp
 *
 *  Created on: 2012-8-17
 *      Author: chenhl
 */

#include <cetty/shiro/authc/AuthenticationToken.h>

namespace cetty {
namespace shiro {
namespace authc {

void AuthenticationToken::init(const std::string &principal,
                               const std::string& credentials,
                               const std::string& host,
                               bool rememberMe){
    this->principal = principal;
    this->credentials = credentials;
    this->host = host;
    this->rememberMe = rememberMe;
}

void AuthenticationToken::clear(){
    principal.clear();
    credentials.clear();
    host.clear();
    rememberMe = false;
}

std::string AuthenticationToken::toString(){
    std::string string;
    string += "principal: ";
    string += principal;
    string += " host: ";
    string += host;
    string += " rememberMe: ";
    string += (rememberMe ? std::string("ture") : std::string("false"));
    return string;
}

}
}
}


