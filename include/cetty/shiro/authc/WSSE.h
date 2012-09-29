/*
 * WSSE.h
 *
 *  Created on: 2012-9-25
 *      Author: chenhl
 */

#ifndef WSSE_H_
#define WSSE_H_

namespace cetty {
namespace shiro {
namespace authc {

struct LoginSecret {
    LoginSecret(const std::string &host,
                const std::string &principal,
                const std::string &nonce,
                const std::string &created)
        :host(host),
         principal(principal),
         nonce(nonce),
         created(created){}

    std::string host;
    std::string principal;
    std::string nonce;
    std::string created;
};

class WSSE {
public:
    WSSE(const std::string &backend, const std::string &connectionString);
    virtual ~WSSE(){}

    void generatorSecret(const std::string& principal,
                         const std::string& host,
                         LoginSecret* ls);

    bool verifySecret(const std::string& host,
                      const std::string& principal,
                      const std::string& nonce,
                      const std::string& created);

    bool verifySecret(const LoginSecret& secret);

    void setBackend(const std::string &backend);
    void setConnectionString(const std::string &connStr);
    void setUserTokenQuery(const std::string &query);
    void setUserTokenUpdate(const std::string &update);
    void setUserTokenSave(const std::string &saving);

protected:
    static const std::string BACKEND;
    static const std::string CONNECTION_STRING;

    static const std::string USER_TOKEN_QUERY;
    static const std::string USER_TOKEN_UPDATE;
    static const std::string USER_TOKEN_SAVE;

protected:
    virtual void generatorNonce(std::string *nonce);
    virtual void generatorCreated(std::string *created);

    /// @brief retrive login secret from data source
    virtual void retrive(const LoginSecret &secret, LoginSecret *ls);

    /// @brief save login secret to data source
    virtual void save(const LoginSecret &ls);

private:
    bool isExpried(const std::string &created);

private:
    static const int NONCE_LENGTH;
    static const int SERVER_TIME_EXPIRE;

    std::string backend;
    std::string connectionString;

    std::string userTokenQuery;
    std::string userTokenUpdate;
    std::string userTokenSave;
};

inline
void WSSE::setBackend(const std::string &backend){
    this->backend = backend;
}

inline
void WSSE::setConnectionString(const std::string &connStr){
    this->connectionString = connStr;
}

inline
void WSSE::setUserTokenQuery(const std::string &query){
    this->userTokenQuery = query;
}

inline
void WSSE::setUserTokenUpdate(const std::string &update){
    this->userTokenUpdate = update;
}

inline
void WSSE::setUserTokenSave(const std::string &saving){
    this->userTokenSave = saving;
}

}
}
}


#endif /* WSSE_H_ */
