/*
 * RedisDao.cpp
 *
 *  Created on: 2012-8-23
 *      Author: chenhl
 */

#include <cetty/shiro/dao/RedisDao.h>

#include <string>

std::string getPassword(std::string username){
    std::string password;
    if(!username.empty()){
        redisContext *conn = redisConnect("127.0.0.1", 6379);
        if(conn->err){
            printf("connection error: %s", conn->err);
            return password;
        }

    redisReply *reply = (redisReply *)redisCommand(conn, "get %s", username.c_str());
    password.assign(reply->str, reply->len);
    freeReplyObject(reply);
    redisFree(conn);
    }

    return password;
}


