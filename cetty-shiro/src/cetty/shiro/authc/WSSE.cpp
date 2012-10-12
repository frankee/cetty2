/*
 * WSSE.cpp
 *
 *  Created on: 2012-9-25
 *      Author: chenhl
 */

#include <cetty/shiro/authc/WSSE.h>

#include <soci.h>
#include <sqlite3/soci-sqlite3.h>

#include <cetty/logging/LoggerHelper.h>
#include <cetty/config/ConfigCenter.h>

namespace cetty {
namespace shiro {
namespace authc {

const int WSSE::NONCE_LENGTH = 6;
const int WSSE::SERVER_TIME_EXPIRE = 10 * 60;

const std::string WSSE::USER_TOKEN_QUERY =
    "SELECT username, host, nonce, created "
    "FROM user_token "
    "WHERE usrname = ? AND created = ? AND is_used = 0";

const std::string USER_TOKEN_UPDATE =
    "UPDATE user_token "
    "SET is_used = 1 "
    "WHERE username = ? AND created = ?";

const std::string WSSE::USER_TOKEN_SAVE =
    "INSERT INTO user_token(username, host, nonce, created, is_used) "
    "VALUES (?, ?, ?, ?, 0)";


WSSE::WSSE(){
    ConfigCenter::instance().configure(&config);
}

void WSSE::generatorSecret(const std::string &principal,
                           const std::string &host,
                           LoginSecret *ls){
    if(ls == NULL){
        LOG_ERROR << "Login secret can't be NULL";
        return;
    }

    ls->principal = principal;
    ls->host = host;
    generatorNonce(&(ls->nonce));
    generatorCreated(&(ls->created));

    save(*ls);
}

bool WSSE::verifySecret(const std::string& host,
                        const std::string& principal,
                        const std::string& nonce,
                        const std::string& created){
    LoginSecret ls(host, principal, nonce, created);
    return verifySecret(ls);
}

bool WSSE::verifySecret(const LoginSecret& secret){
    if(secret.principal.empty() || secret.nonce.empty() ||
       secret.created.empty())
        return false;

    LoginSecret ls;
    retrive(secret, &ls);

    bool ret = (secret.host == ls.host &&
                secret.principal == ls.principal &&
                secret.nonce == ls.nonce &&
                !isExpried(ls.created));

    return ret;
}

bool WSSE::isExpried(const std::string &created){
    std::string now;
    generatorCreated(&now);

    long int secs1 = std::atoi(created.c_str());
    long int secs2 = std::atoi(now.c_str());

    long int interval = secs2 - secs1;
    if(interval > config.serverTimeExpire)
        return true;

    return false;
}

void WSSE::generatorNonce(std::string *nonce){
    assert(nonce != NULL);

    std::srand((unsigned int)std::time(NULL));

    char ch = '\0';
    int i = 0;
    for(; i < config.nonceLength; ++i){
        int t = (rand() % 61);
        if(t >= 0 && t <= 9) ch = '0' + t;
        else if(t >= 10 && t <= 35) ch = t + 55;
        else ch = t + 61;
        nonce->append(1, ch);
    }
}

void WSSE::generatorCreated(std::string *created){
    assert(created != NULL);

    std::time_t secs;
    time(&secs);

    char temp[20];
    std::memset(temp, 0x00, sizeof(temp));
    sprintf(temp, "%ld", secs);
    created->assign(temp);
}

void WSSE::retrive(const LoginSecret &secret, LoginSecret *ls){
    if(ls == NULL){
        LOG_ERROR << "Argument ls can't be NULL.";
        return;
    }

    if((config.backend).empty() || (config.connectionString).empty()){
        LOG_ERROR << "DB connection info does not be set.";
        return;
    }

    soci::session sql;
    try{
        sql.open(config.backend, config.connectionString);
        sql << config.userTokenQuery,
               soci::into(ls->principal),
               soci::into(ls->host),
               soci::into(ls->nonce),
               soci::into(ls->created),
               soci::use(secret.principal),
               soci::use(secret.created);
    } catch (soci::soci_error const & e) {
        LOG_ERROR << e.what();
        return;
    }

    try{
         sql << config.userTokenUpdate,
                soci::use(secret.principal),
                soci::use(secret.created);
         sql.commit();
    } catch(soci::soci_error const & e){
         LOG_ERROR << e.what();
         sql.rollback();
    }
    sql.close();
}

void WSSE::save(const LoginSecret &ls){
    if((config.backend).empty() || (config.connectionString).empty()){
        LOG_ERROR << "DB connection info does not be set.";
        return;
    }

    soci::session sql;
    try{
        sql.open(config.backend, config.connectionString);
        sql << config.userTokenSave,
               soci::use(ls.principal),
               soci::use(ls.host),
               soci::use(ls.nonce),
               soci::use(ls.created);
        sql.commit();
    } catch(soci::soci_error const & e) {
        LOG_ERROR << e.what();
        sql.rollback();
    }

    sql.close();
}


}
}
}


