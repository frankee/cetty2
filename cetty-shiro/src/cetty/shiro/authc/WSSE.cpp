/*
 * WSSE.cpp
 *
 *  Created on: 2012-9-25
 *      Author: chenhl
 */

#include <cetty/shiro/authc/WSSE.h>

#include <soci/soci.h>
#include <soci/sqlite3/soci-sqlite3.h>

#include <cetty/logging/LoggerHelper.h>
#include <cetty/config/ConfigCenter.h>
#include <cetty/util/StringUtil.h>

namespace cetty {
namespace shiro {
namespace authc {

using namespace cetty::util;
using namespace cetty::config;

const int WSSE::NONCE_LENGTH = 6;
const int WSSE::SERVER_TIME_EXPIRE = 10 * 60;

const std::string WSSE::USER_TOKEN_QUERY =
    "SELECT nonce "
    "FROM user_token "
    "WHERE username = ? AND created = ? AND is_used = 0";

const std::string WSSE::USER_TOKEN_UPDATE =
    "UPDATE user_token "
    "SET is_used = 1 "
    "WHERE username = ? AND created = ?";

const std::string WSSE::USER_TOKEN_SAVE =
    "INSERT INTO user_token(username, host, nonce, created, is_used) "
    "VALUES (?, ?, ?, ?, 0)";

WSSE& WSSE::instance() {
    static WSSE wsse;
    return wsse;
}

WSSE::WSSE() {
    ConfigCenter::instance().configure(&config);
}

void WSSE::generatorSecret(const std::string& principal,
                           const std::string& host,
                           std::string* nonce,
                           std::string* created) {
    if (nonce == NULL || created == NULL) {
        LOG_ERROR << "Login secret (nonce or created) can't be NULL";
        return;
    }

    generatorNonce(nonce);
    generatorCreated(created);

    save(host, principal, *nonce, *created);
}

bool WSSE::verifySecret(const std::string& host,
                        const std::string& principal,
                        const std::string& nonce,
                        const std::string& created) {
    if (principal.empty()
            || nonce.empty()
            || created.empty()
            || isExpried(created)) {
        return false;
    }

    std::string storedNonce;
    retrive(principal, created, &storedNonce);

    return nonce == storedNonce;
}

bool WSSE::isExpried(const std::string& created) {
    std::string now;
    generatorCreated(&now);

    int64_t createdTime = StringUtil::atoi(created);
    int64_t nowTime = StringUtil::atoi(now);

    int64_t interval = nowTime - createdTime;

    if (interval > config.serverTimeExpire) {
        return true;
    }

    return false;
}

void WSSE::generatorNonce(std::string* nonce) {
    assert(nonce != NULL);

    std::srand((unsigned int)std::time(NULL));

    char ch = '\0';
    int i = 0;

    for (; i < config.nonceLength; ++i) {
        int t = (rand() % 61);

        if (t >= 0 && t <= 9) { ch = '0' + (char)t; }
        else if (t >= 10 && t <= 35) { ch = (char)t + 55; }
        else { ch = (char)t + 61; }

        nonce->append(1, ch);
    }
}

void WSSE::generatorCreated(std::string* created) {
    assert(created != NULL);

    std::time_t secs;
    time(&secs);

    char temp[20];
    std::memset(temp, 0x00, sizeof(temp));
    sprintf(temp, "%ld", secs);
    created->assign(temp);
}

void WSSE::retrive(const std::string& principal,
                   const std::string& created,
                   std::string* nonce) {
    if (nonce == NULL) {
        LOG_ERROR << "Argument ls can't be NULL.";
        return;
    }

    if (config.backend.empty() || config.connectionString.empty()) {
        LOG_ERROR << "DB connection info does not be set.";
        return;
    }

    soci::session sql;

    try {
        sql.open(config.backend, config.connectionString);
        sql << config.userTokenQuery,
            soci::into(*nonce),
            soci::use(principal),
            soci::use(created);
    }
    catch (soci::soci_error const& e) {
        LOG_ERROR << e.what();
        return;
    }

    try {
        sql << config.userTokenUpdate,
            soci::use(principal),
            soci::use(created);
        //sql.commit();
    }
    catch (soci::soci_error const& e) {
        LOG_ERROR << e.what();
        //sql.rollback();
    }

    sql.close();
}

void WSSE::save(const std::string& host,
                const std::string& principal,
                const std::string& nonce,
                const std::string& created) {
    if (config.backend.empty() || config.connectionString.empty()) {
        LOG_ERROR << "DB connection info does not be set.";
        return;
    }

    soci::session sql;

    try {
        sql.open(config.backend, config.connectionString);
        sql << config.userTokenSave,
            soci::use(principal),
            soci::use(host),
            soci::use(nonce),
            soci::use(created);
        //sql.commit();
    }
    catch (soci::soci_error const& e) {
        LOG_ERROR << e.what();
        //sql.rollback();
    }

    sql.close();
}

}
}
}
