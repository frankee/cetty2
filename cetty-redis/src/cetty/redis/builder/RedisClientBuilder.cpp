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

#include <cetty/redis/builder/RedisClientBuilder.h>

#include <cetty/redis/protocol/RedisReply.h>
#include <cetty/redis/protocol/RedisReplyDecoder.h>
#include <cetty/redis/protocol/RedisCommandEncoder.h>

namespace cetty {
namespace redis {
namespace builder {

using namespace cetty::redis;
using namespace cetty::redis::protocol;

RedisClientBuilder::RedisClientBuilder()
    : builder_() {
    init();
}

RedisClientBuilder::RedisClientBuilder(int threadCnt)
    : builder_(threadCnt) {
    init();
}

RedisClientBuilder::RedisClientBuilder(const EventLoopPtr& eventLoop)
    : builder_(eventLoop) {
    init();
}

RedisClientBuilder::RedisClientBuilder(const EventLoopPoolPtr& eventLoopPool)
    : builder_(eventLoopPool) {
    init();
}

bool initializeChannel(const ChannelPtr& channel) {
    ChannelPipeline& pipeline = channel->pipeline();

    pipeline.addLast<RedisCommandEncoder::HandlerPtr>("redisEncoder",
        RedisCommandEncoder::HandlerPtr(new RedisCommandEncoder));

    pipeline.addLast<RedisReplyMessageDecoder::HandlerPtr>("redisDecoder",
        RedisReplyMessageDecoder::HandlerPtr(new RedisReplyMessageDecoder));

    return true;
}

void RedisClientBuilder::init() {
    builder_.setClientInitializer(boost::bind(initializeChannel, _1));
}

}
}
}
