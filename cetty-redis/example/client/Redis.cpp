
/**
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

#include <string>
#include <boost/bind.hpp>
#include "RedisClient.h"

using namespace cetty::redis;

void myCallback(int code, const SimpleString& reply) {
    if (code < 0) {
        printf("the value not exist.\n");
    }
    else {
        printf("the value is %s.\n", reply.c_str());
    }
}

void myArrayCallback(int code, const std::vector<SimpleString>& replies) {
    if (code < 0) {
        printf("the array value not exist.\n");
    }
    else {
        for (std::size_t i = 0; i < replies.size(); ++i) {
            printf("the array value of %d is %s.\n", i, replies[i].c_str());
        }
    }
}

int main(int argc, char** argv) {
    RedisClient client;
    client.addServer("127.0.0.1", 6379);

    std::vector<std::string> keys;

    /*
    std::string buffer(1024* 100, 0xff);

    client.set("test1", buffer);
    client.get("test1", boost::bind(myCallback, _1, _2));


    buffer.clear();
    buffer.append(1024*16, '2');
    client.set("test1", buffer);
    client.get("test1", boost::bind(myCallback, _1, _2));
    */

    /*
    std::vector<std::pair<std::string, std::string> > values;
    values.push_back(std::make_pair("11", "12345"));
    values.push_back(std::make_pair("22", "12345--2"));
    client.hmset("ht", values);
    */

    /*
    keys.push_back("11");
    keys.push_back("22");
    client.hmget("ht", keys, boost::bind(myArrayCallback, _1, _2));
    */

    client.set("mset1", std::string("1---"));
    client.set("mset2", std::string("sdksalsdj888788**&&^^2"));

    keys.clear();
    keys.push_back("mset1");
    keys.push_back("mset2");
    client.mget(keys, boost::bind(myArrayCallback, _1, _2));

    system("PAUSE");

    return 0;
}