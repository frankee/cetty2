#if !defined(CETTY_REDIS_BUILDER_REDISCLIENTBUILDER_H)
#define CETTY_REDIS_BUILDER_REDISCLIENTBUILDER_H

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

#include <cetty/redis/protocol/RedisReply.h>
#include <cetty/redis/protocol/RedisCommand.h>
#include <cetty/service/builder/ClientBuilder.h>

namespace cetty {
namespace redis {
namespace builder {

using namespace cetty::service::builder;
using namespace cetty::redis::protocol;

class RedisClientBuilder
        : public ClientBuilder<RedisCommandPtr, RedisReplyPtr> {
public:
    typedef ClientBuilder<RedisCommandPtr, RedisReplyPtr> ClientBuilderType;

public:
    RedisClientBuilder();
    RedisClientBuilder(int threadCnt);
    RedisClientBuilder(const EventLoopPtr& eventLoop);
    RedisClientBuilder(const EventLoopPoolPtr& eventLoopPool);

private:
    void init();

private:
    ChannelPipelinePtr pipeline;
};

}
}
}

#endif //#if !defined(CETTY_REDIS_BUILDER_REDISCLIENTBUILDER_H)

// Local Variables:
// mode: c++
// End:
