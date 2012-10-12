/*
 * WSSE.h
 *
 *  Created on: 2012-9-25
 *      Author: chenhl
 */

#ifndef WSSE_H_
#define WSSE_H_

#include <cetty/shiro/authc/WSSEConfig.h>

namespace cetty {
namespace shiro {
namespace authc {

class WSSE {
public:
    static WSSE& instance();

public:
    WSSE();
    virtual ~WSSE() {}

    void generatorSecret(const std::string& principal,
                         const std::string& host,
                         std::string* nonce,
                         std::string* created);

    bool verifySecret(const std::string& host,
                      const std::string& principal,
                      const std::string& nonce,
                      const std::string& created);

public:
    static const std::string USER_TOKEN_QUERY;
    static const std::string USER_TOKEN_UPDATE;
    static const std::string USER_TOKEN_SAVE;

    static const int NONCE_LENGTH;
    static const int SERVER_TIME_EXPIRE;

protected:
    virtual void generatorNonce(std::string* nonce);
    virtual void generatorCreated(std::string* created);

    /// @brief retrive login secret from data source
    virtual void retrive(const std::string& principal,
                         const std::string& created,
                         std::string* nonce);

    /// @brief save login secret to data source
    virtual void save(const std::string& host,
                      const std::string& principal,
                      const std::string& nonce,
                      const std::string& created);

private:
    bool isExpried(const std::string& created);

private:
    WSSEConfig config;
};

}
}
}


#endif /* WSSE_H_ */
