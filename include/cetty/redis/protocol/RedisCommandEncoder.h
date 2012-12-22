#if !defined(CETTY_REDIS_PROTOCOL_REDISCOMMANDENCODER_H)
#define CETTY_REDIS_PROTOCOL_REDISCOMMANDENCODER_H

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

#include <cetty/handler/codec/MessageToBufferEncoder.h>
#include <cetty/redis/protocol/RedisCommand.h>

namespace cetty {
namespace redis {
namespace protocol {

using namespace cetty::channel;
using namespace cetty::handler::codec;

class RedisCommandEncoder : private boost::noncopyable {
public:
    typedef MessageToBufferEncoder<RedisCommandEncoder, RedisCommandPtr> Encoder;
    typedef Encoder::Context Context;
    typedef Encoder::Handler Handler;
    typedef Encoder::HandlerPtr HandlerPtr;

public:
    RedisCommandEncoder() {
        encoder_.setEncoder(boost::bind(&RedisCommandEncoder::encode,
                                        this,
                                        _1,
                                        _2,
                                        _3));
    }

    ~RedisCommandEncoder() {}

    void registerTo(Context& ctx) {
        encoder_.registerTo(ctx);
    }

private:
    ChannelBufferPtr encode(ChannelHandlerContext& ctx,
                            const RedisCommandPtr& msg,
                            const ChannelBufferPtr& out) {
        return msg->getBuffer();
    }

private:
    Encoder encoder_;
};

}
}
}

#endif //#if !defined(CETTY_REDIS_PROTOCOL_REDISCOMMANDENCODER_H)

// Local Variables:
// mode: c++
// End:
