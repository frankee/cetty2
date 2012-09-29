/*
 * WSSEDAO.cpp
 *
 *  Created on: 2012-9-27
 *      Author: chenhl
 */


#include <cetty/shiro/dao/WSSEDAO.h>

#include <cetty/logging/LoggerHelper.h>

namespace cetty {
namespace shiro {
namespace dao {

void WSSEDAO::queryLoginSecret(const LoginSecret &oldLs, LoginSecret *newLs){
    if(newLs == NULL){
        LOG_ERROR << "Login secret pointer can't be null";
        return ;
    }
    doQuery(oldLs, newLs);
}

void WSSEDAO::saveLoginSecret(const LoginSecret &ls){
    doSave(ls);
}

void WSSEDAO::doQuery(const LoginSecret &oldLs, LoginSecret *newLs){
    soci::session &sql = getSql();

    try{
        sql << "SELECT user_id, host, nonce, created "
               "FROM user_token "
               "WHERE user_id = :id AND created = :created AND is_used = 0",
               soci::into(newLs->principal),
               soci::into(newLs->host),
               soci::into(newLs->nonce),
               soci::into(newLs->created),
               soci::use(oldLs.principal),
               soci::use(oldLs.created);
    } catch (soci::soci_error const & e) {
        LOG_ERROR << e.what();
        return;
    }

    try{
        sql << "UPDATE user_token "
               "SET is_used = 1 "
               "WHERE user_id = :id AND created = :created",
               soci::use(oldLs.principal),
               soci::use(oldLs.created);
    } catch(soci::soci_error const & e){
        LOG_ERROR << e.what();
    }
}

void WSSEDAO::doSave(const LoginSecret &ls){
    soci::session &sql = getSql();
    try{
        sql << "INSERT INTO user_token(user_id, host, nonce, created, is_used) "
               "VALUES (?, ?, ?, ?, 0)",
               soci::use(ls.principal),
               soci::use(ls.host),
               soci::use(ls.nonce),
               soci::use(ls.created);
    } catch(soci::soci_error const & e) {
        LOG_ERROR << e.what();
    }
}

}
}
}

