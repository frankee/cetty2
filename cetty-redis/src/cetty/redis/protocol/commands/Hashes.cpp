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

#include <cetty/redis/protocol/commands/Hashes.h>
#include <cetty/redis/protocol/RedisCommand.h>

namespace cetty {
namespace redis {
namespace protocol {
namespace commands {

using namespace cetty::redis;

// Hash Sets
static const std::string HDEL         = "HDEL";
static const std::string HEXISTS      = "HEXISTS";
static const std::string HGET         = "HGET";
static const std::string HGETALL      = "HGETALL";
static const std::string HINCRBY      = "HINCRBY";
static const std::string HINCRBYFLOAT = "HINCRBYFLOAT";
static const std::string HKEYS        = "HKEYS";
static const std::string HLEN         = "HLEN";
static const std::string HMGET        = "HMGET";
static const std::string HMSET        = "HMSET";
static const std::string HSET         = "HSET";
static const std::string HSETNX       = "HSETNX";
static const std::string HVALS        = "HVALS";

RedisCommandPtr hashesCommandGetAll(const std::string& key) {
    RedisCommandPtr command(new RedisCommand(HGETALL));
    (*command << key).done();
    return command;
}

RedisCommandPtr hashesCommandIncrementBy(const std::string& key, const std::string& field, int64_t increment) {
    RedisCommandPtr command(new RedisCommand(HINCRBY));
    (*command << key << field << increment).done();
    return command;
}

RedisCommandPtr hashesCommandIncrementBy(const std::string& key, const std::string& field, double increment) {
    RedisCommandPtr command(new RedisCommand(HINCRBYFLOAT));
    (*command << key << field << increment).done();
    return command;
}

RedisCommandPtr hashesCommandKeys(const std::string& key) {
    RedisCommandPtr command(new RedisCommand(HKEYS));
    (*command << key).done();
    return command;
}

RedisCommandPtr hashesCommandSet(const std::string& key, const std::string& field, const std::string& value) {
    RedisCommandPtr command(new RedisCommand(HSET));
    (*command << key << field << value).done();
    return command;
}

RedisCommandPtr hashesCommandSet(const std::string& key, const std::string& field, const StringPiece& value) {
    RedisCommandPtr command(new RedisCommand(HSET));
    (*command << key << field << value).done();
    return command;
}

RedisCommandPtr hashesCommandSet(const std::string& key, const std::vector<std::pair<std::string, std::string> >& values) {
    RedisCommandPtr command(new RedisCommand(HMSET));
    *command << key;

    std::size_t j = values.size();

    for (std::size_t i = 0; i < j; ++i) {
        *command << values[i].first << values[i].second;
    }

    command->done();
    return command;
}

RedisCommandPtr hashesCommandSet(const std::string& key, const std::vector<std::pair<std::string, StringPiece> >& values) {
    RedisCommandPtr command(new RedisCommand(HMSET));
    *command << key;

    std::size_t j = values.size();

    for (std::size_t i = 0; i < j; ++i) {
        *command << values[i].first << values[i].second;
    }

    command->done();
    return command;
}

RedisCommandPtr hashesCommandSet(const std::string& key, const std::vector<std::string>& fields, const std::vector<std::string>& values) {
    RedisCommandPtr command(new RedisCommand(HMSET));
    *command << key;

    std::size_t j = values.size();

    for (std::size_t i = 0; i < j; ++i) {
        *command << fields[i] << values[i];
    }

    command->done();
    return command;
}

RedisCommandPtr hashesCommandSet(const std::string& key, const std::vector<std::string>& fields, const std::vector<StringPiece>& values) {
    RedisCommandPtr command(new RedisCommand(HMSET));
    *command << key;

    std::size_t j = values.size();

    for (std::size_t i = 0; i < j; ++i) {
        *command << fields[i] << values[i];
    }

    command->done();
    return command;
}

RedisCommandPtr hashesCommandSetNx(const std::string& key, const std::string& field, const std::string& value) {
    RedisCommandPtr command(new RedisCommand(HSETNX));
    (*command << key << value).done();
    return command;
}

RedisCommandPtr hashesCommandSetNx(const std::string& key, const std::string& field, const StringPiece& value) {
    RedisCommandPtr command(new RedisCommand(HSETNX));
    (*command << key << value).done();
    return command;
}

RedisCommandPtr hashesCommandValues(const std::string& key) {
    RedisCommandPtr command(new RedisCommand(HVALS));
    (*command << key).done();
    return command;
}

RedisCommandPtr hashesCommandDel(const std::string& key, const std::string& field) {
    RedisCommandPtr command(new RedisCommand(HDEL));
    (*command << key << field).done();
    return command;
}

RedisCommandPtr hashesCommandDel(const std::string& key, const std::vector<std::string>& fields) {
    RedisCommandPtr command(new RedisCommand(HDEL));
    (*command << key << fields).done();
    return command;
}

RedisCommandPtr hashesCommandExist(const std::string& key, const std::string& field) {
    RedisCommandPtr command(new RedisCommand(HEXISTS));
    (*command << key << field).done();
    return command;
}

RedisCommandPtr hashesCommandGet(const std::string& key, const std::string& field) {
    RedisCommandPtr command(new RedisCommand(HGET));
    (*command << key << field).done();
    return command;
}

RedisCommandPtr hashesCommandGet(const std::string& key, const std::vector<std::string>& fields) {
    RedisCommandPtr command(new RedisCommand(HMGET));
    (*command << key << fields).done();
    return command;
}

RedisCommandPtr hashesCommandFieldCnt(const std::string& key) {
    RedisCommandPtr command(new RedisCommand(HLEN));
    (*command << key).done();
    return command;
}

}
}
}
}
