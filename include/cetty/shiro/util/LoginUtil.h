/*
 * LoginUtil.h
 *
 *  Created on: 2012-9-10
 *      Author: chenhl
 */

#if !defined(CETTY_SHIRO_UTIL_LOGINUTIL_H)
#define CETTY_SHIRO_UTIL_LOGINUTIL_H

#include <string>
#include <boost/function.hpp>

namespace cetty {
namespace shiro {
namespace util {

class LoginUtil {
public:
    typedef boost::function4<void,
                             const std::string&,
                             const std::string&,
                             const std::string&,
                             const std::string&
                             > saveInfo;

    typedef boost::function2<const std::string&,
                             const std::string&,
                             const std::string&
                             > getStoreInfo;

public:
    LoginUtil(){}
    LoginUtil(const saveInfo& save,
              const getStoreInfo& retrieveNonce,
              const getStoreInfo& retrieveServerTime,
              const getStoreInfo& retrieveHost);

    /// produce random nonce and server time
    std::string getNonce();
    std::string getServerTime();

    /// save user name, host, nonce produced by #getNonce,
    /// server time produced by #getServerTime() to db.
    void saveNonceServerTime(const std::string& userName,
                             const std::string& host,
                             const std::string& nonce,
                             const std::string& serverTime);

    /// get token{nonce, server time, host} from user info
    /// return from #getUserInfo
    std::string getNonce(const std::string& username, const std::string& serverTime);
    std::string getServerTime(const std::string& username, const std::string& serverTime);
    std::string getHost(const std::string& username, const std::string& serverTime);

    bool verifyServerTime(const std::string& oldTime,
                          const std::string& newTime);

    void setSaveInfo(const saveInfo& save);
    void setGetNonce(const getStoreInfo& retrieveNonce);
    void setGerServerTime(const getStoreInfo& retrieveServerTime);
    void setGetHost(const getStoreInfo& retrieveHost);

public:
    saveInfo save;
    getStoreInfo retrieveNonce;
    getStoreInfo retrieveServerTime;
    getStoreInfo retrieveHost;

    static const int NONCE_LENGTH;

private:
    static const int SERVER_TIME_EXPIRE;
};

inline
void LoginUtil::saveNonceServerTime(const std::string& userName,
                                    const std::string& host,
                                    const std::string& nonce,
                                    const std::string& serverTime){
    save(userName, host, nonce, serverTime);
}

inline
std::string LoginUtil::getNonce(const std::string& username,
                                const std::string& serverTime){
    return retrieveNonce(username, serverTime);
}

inline
std::string LoginUtil::getServerTime(const std::string& username,
                                     const std::string& serverTime){
    return retrieveServerTime(username, serverTime);
}

inline
std::string LoginUtil::getHost(const std::string& username,
                               const std::string& serverTime){
    return retrieveHost(username, serverTime);
}

inline
void LoginUtil::setSaveInfo(const saveInfo& save){
    this->save = save;
}

inline
void LoginUtil::setGetNonce(const getStoreInfo& retrieveNonce){
    this->retrieveNonce = retrieveNonce;
}

inline
void LoginUtil::setGerServerTime(const getStoreInfo& retrieveServerTime){
    this->retrieveServerTime = retrieveServerTime;
}

inline
void LoginUtil::setGetHost(const getStoreInfo& retrieveHost){
    this->retrieveHost = retrieveHost;
}

}
}
}

#endif // !defined(CETTY_SHIRO_UTIL_LOGINUTIL_H)
