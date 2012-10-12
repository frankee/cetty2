/*
 * Copyright (c) 2010-2012 frankee zhou (frankee.zhou at gmail dot com)
 *
 * Distributed under under the Apache License, version 2.0 (the "License").
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 */

#include <cetty/redis/command/Strings.h>
#include <cetty/redis/RedisCommand.h>

namespace cetty {
namespace redis {
namespace command {

using namespace cetty::redis;

// String Commands
static const std::string APPEND    = "APPEND";
static const std::string BITCOUNT  = "BITCOUNT";
static const std::string DECR      = "DECR";
static const std::string DECRBY    = "DECRBY";
static const std::string DECRBYFLOAT = "DECRBYFLOAT";
static const std::string GET       = "GET";
static const std::string GETBIT    = "GETBIT";
static const std::string GETRANGE  = "GETRANGE";
static const std::string GETSET    = "GETSET";
static const std::string INCR      = "INCR";
static const std::string INCRBY    = "INCRBY";
static const std::string INCRBYFLOAT = "INCRBYFLOAT";
static const std::string MGET      = "MGET";
static const std::string MSET      = "MSET";
static const std::string MSETNX    = "MSETNX";
static const std::string SET       = "SET";
static const std::string SETBIT    = "SETBIT";
static const std::string PSETEX    = "PSETEX";
static const std::string SETEX     = "SETEX";
static const std::string SETNX     = "SETNX";
static const std::string SETRANGE  = "SETRANGE";
static const std::string STRLEN    = "STRLEN";

RedisCommandPtr stringsCommandAppend(const std::string& key, const StringPiece& value) {
    RedisCommandPtr command(new RedisCommand(APPEND));
    (*command << key << value).done();
    return command;
}

RedisCommandPtr stringsCommandBitcount(const std::string& key, int start, int end) {
    RedisCommandPtr command(new RedisCommand(BITCOUNT));
    (*command << key << start << end).done();
    return command;
}

RedisCommandPtr stringsCommandDecrement(const std::string& key) {
    RedisCommandPtr command(new RedisCommand(DECR));
    (*command << key).done();
    return command;
}

RedisCommandPtr stringsCommandDecrementBy(const std::string& key, int64_t value) {
    RedisCommandPtr command(new RedisCommand(DECRBY));
    (*command << key << value).done();
    return command;
}

RedisCommandPtr stringsCommandGet(const std::string& key) {
    RedisCommandPtr command(new RedisCommand(GET));
    (*command << key).done();
    return command;
}

RedisCommandPtr stringsCommandGetBit(const std::string& key, int offset) {
    RedisCommandPtr command(new RedisCommand(GETBIT));
    (*command << key << offset).done();
    return command;
}

RedisCommandPtr stringsCommandGetRange(const std::string& key, int start, int end) {
    RedisCommandPtr command(new RedisCommand(GETRANGE));
    (*command << key << start << end).done();
    return command;
}

RedisCommandPtr stringsCommandGetset(const std::string& key, const StringPiece& value) {
    RedisCommandPtr command(new RedisCommand(GETSET));
    (*command << key << value).done();
    return command;
}

RedisCommandPtr stringsCommandIncrement(const std::string& key) {
    RedisCommandPtr command(new RedisCommand(INCR));
    (*command << key).done();
    return command;
}

RedisCommandPtr stringsCommandIncrement(const std::string& key, int64_t value) {
    RedisCommandPtr command(new RedisCommand(INCRBY));
    (*command << key << value).done();
    return command;
}

RedisCommandPtr stringsCommandIncrement(const std::string& key, double value) {
    RedisCommandPtr command(new RedisCommand(INCRBYFLOAT));
    (*command << key << value).done();
    return command;
}

RedisCommandPtr stringsCommandSet(const std::string& key, const StringPiece& value, int expiration) {
    RedisCommandPtr command;
    
    int second = expiration / 1000;
    if (second * 1000 == expiration) {
        command = new RedisCommand(PSETEX);
        (*command << key << expiration << value).done();
    }
    else {
        command = new RedisCommand(SETEX);
        (*command << key << second << value).done();
    }
    
    return command;
}

RedisCommandPtr stringsCommandSet(const std::string& key, const StringPiece& value) {
    RedisCommandPtr command(new RedisCommand(SET));
    (*command << key << value).done();
    return command;
}

RedisCommandPtr stringCommandSetBit(const std::string& key, const StringPiece& value, int offset) {
    RedisCommandPtr command(new RedisCommand(SETBIT));
    (*command << key << offset << value).done();
    return command;
}

RedisCommandPtr stringsCommandSetNx(const std::string& key, const StringPiece& value) {
    RedisCommandPtr command(new RedisCommand(SETNX));
    (*command << key << value).done();
    return command;
}

RedisCommandPtr stringsCommandSetRange(const std::string& key, int offset, const StringPiece& value) {
    RedisCommandPtr command(new RedisCommand(SETRANGE));
    (*command << key << offset << value).done();
    return command;
}

RedisCommandPtr stringsCommandStrlen(const std::string& key) {
    RedisCommandPtr command(new RedisCommand(STRLEN));
    (*command << key).done();
    return command;
}

}
}
}
