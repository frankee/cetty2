/*
 * LoginUtil.cpp
 *
 *  Created on: 2012-9-11
 *      Author: chenhl
 */

#include <cetty/shiro/util/LoginUtil.h>

#include <cstdlib>
#include <ctime>
#include <cstring>
#include <cstdio>

namespace cetty {
namespace shiro {
namespace util {

const int LoginUtil::NONCE_LENGTH = 6;
const int LoginUtil::SERVER_TIME_EXPIRE = 10 * 60;

LoginUtil::LoginUtil(const saveInfo& save,
              const getStoreInfo& retrieveNonce,
              const getStoreInfo& retrieveServerTime,
              const getStoreInfo& retrieveHost){
    this->save = save;
    this->retrieveHost = retrieveNonce;
    this->retrieveServerTime = retrieveServerTime;
    this->retrieveHost = retrieveHost;
}


std::string LoginUtil::getNonce(){
    std::srand((unsigned int)std::time(NULL));
    int random = std::rand();

    std::string nonce;
    char ch = '\0';
    int i = 0;
    for(; i < LoginUtil::NONCE_LENGTH; ++i){
        int t = (rand() % 61);
        if(t >= 0 && t <= 9) ch = '0' + t;
        else if(t >= 10 && t <= 35) ch = t + 55;
        else ch = t + 61;
        nonce.append(1, ch);
    }

    return nonce;
}

std::string LoginUtil::getServerTime(){
    std::string serverTime;

    std::time_t secs;
    time(&secs);

    char temp[20];
    std::memset(temp, 0x00, sizeof(temp));
    sprintf(temp, "%ld", secs);
    serverTime.assign(temp);

    return serverTime;
}

bool LoginUtil::verifyServerTime(const std::string& oldTime,
                                 const std::string& newTime){
    long int time1 = std::atol(oldTime.c_str());
    long int time2 = std::atol(newTime.c_str());

    long int diff = time2 - time1;
    if(diff <= LoginUtil::SERVER_TIME_EXPIRE) return true;
    return false;
}


}
}
}


