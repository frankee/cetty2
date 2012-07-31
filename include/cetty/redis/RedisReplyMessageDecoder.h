#if !defined(CETTY_REDIS_REDISRESPONSEDECODER_H)
#define CETTY_REDIS_REDISRESPONSEDECODER_H

/*
 * Copyright (c) 2010-2011 frankee zhou (frankee.zhou at gmail dot com)
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

#include <cetty/handler/codec/ReplayingDecoder.h>
#include <cetty/redis/RedisReplyMessagePtr.h>

namespace cetty { namespace redis {

using namespace cetty::channel;
using namespace cetty::util;
using namespace cetty::handler::codec;

class RedisReplyDecoder : public ReplayingDecoder<RedisReplyMessagePtr> {
public:
    RedisReplyDecoder() {}
    RedisReplyDecoder(int maxSize) {}
    virtual ~RedisReplyDecoder() {}

    virtual ChannelHandlerPtr clone();
    virtual std::string toString() const { return "RedisResponseDecoder"; }

protected:
    virtual UserEvent decode(
        ChannelHandlerContext& ctx, Channel& channel, const ChannelBufferPtr& buffer);

private:
    void fail(ChannelHandlerContext& ctx, long frameLength);

    /**
     * Returns the number of bytes between the readerIndex of the haystack and
     * the first "\r\n" found in the haystack.  -1 is returned if no "\r\n" is
     * found in the haystack.
     */
    int indexOf(const Array& arry, int offset);

private:
    int maxFrameLength;
    bool stripDelimiter;
    
    bool discardingTooLongFrame;
    int tooLongFrameLength;
};

}}

#endif //#if !defined(CETTY_REDIS_REDISRESPONSEDECODER_H)

// Local Variables:
// mode: c++
// End:
