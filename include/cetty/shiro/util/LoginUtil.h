/*
 * LoginUtil.h
 *
 *  Created on: 2012-9-10
 *      Author: chenhl
 */

#if !defined(CETTY_SHIRO_UTIL_LOGINUTIL_H)
#define CETTY_SHIRO_UTIL_LOGINUTIL_H

namespace cetty {
namespace shiro {
namespace util {


class LoginUtil {
public:
    /// produce random nonce and server time
    static std::string getNonce();
    static std::string getServerTime();

    /// save user name, host, nonce produced by #getNonce,
    /// server time produced by #getServerTime() to db.
    static void saveNonceServerTime(const std::string& userName,
                                    const std::string& host,
                                    const std::string& nonce,
                                    const std::string& serverTime);

    /// get user info saved by #saveNonceServerTime
    static std::string getUserInfo(const std::string& userName);

    /// get token{nonce, server time, host} from user info
    /// return from #getUserInfo
    static std::string getNonce(const std::string& userInfo);
    static std::string getServerTime(const std::string& userInfo);
    static std::string getHost(const std::string& userInfo);

    static bool verifyServerTime(const std::string& oldTime,
                                 const std::string& newTime);

public:
    static const int NO_SESSION_CODE;
    static const int LOGIN_FAILED_CODE;
    static const int LOGIN_REFUSED_CODE;

    static const std::string NO_SESSION_MESSAGE;
    static const std::string LOGIN_FAILED_MESSAGE;
    static const std::string LOGIN_REFUSED_MESSAGE;
};

}
}
}

#endif // !defined(CETTY_SHIRO_UTIL_LOGINUTIL_H)
