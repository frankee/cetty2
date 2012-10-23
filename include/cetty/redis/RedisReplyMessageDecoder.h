#if !defined(CETTY_REDIS_REDISREPLYMESSAGEDECODER_H)
#define CETTY_REDIS_REDISREPLYMESSAGEDECODER_H

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

#include <vector>
#include <cetty/util/StringPiece.h>
#include <cetty/handler/codec/ReplayingDecoder.h>
#include <cetty/redis/RedisReplyMessagePtr.h>

namespace cetty {
namespace redis {

using namespace cetty::channel;
using namespace cetty::util;
using namespace cetty::handler::codec;

class RedisReplyMessageDecoder : public ReplayingDecoder<RedisReplyMessagePtr> {
public:
    enum State {
        READ_INITIAL,
        READ_BULK,
        READ_MULTI_BULK
    };

public:
    RedisReplyMessageDecoder()
        : ReplayingDecoder<RedisReplyMessagePtr>(READ_INITIAL, true),
          maxBulkSize(1024*1024),
          bulkSize(0),
          multiBulkSize(0) {}
    RedisReplyMessageDecoder(int maxBulkSize)
        : ReplayingDecoder<RedisReplyMessagePtr>(READ_INITIAL, true),
          maxBulkSize(maxBulkSize),
          bulkSize(0),
          multiBulkSize(0) {}

    virtual ~RedisReplyMessageDecoder() {}

    virtual ChannelHandlerPtr clone();
    virtual std::string toString() const { return "RedisReplyMessageDecoder"; }

protected:
    virtual RedisReplyMessagePtr decode(ChannelHandlerContext& ctx,
                                        const ReplayingDecoderBufferPtr& buffer,
                                        int state);

private:
    RedisReplyMessagePtr readMultiBukls(const ReplayingDecoderBufferPtr& buffer,
                                        const StringPiece& bytes,
                                        std::vector<StringPiece>* bulks);

    bool readMultiBulkElement(const ReplayingDecoderBufferPtr& buffer,
                              const StringPiece& bytes,
                              std::vector<StringPiece>* bulks);

    void fail(ChannelHandlerContext& ctx, long frameLength);

    /**
     * Returns the number of bytes between the readerIndex of the haystack and
     * the first "\r\n" found in the haystack.  -1 is returned if no "\r\n" is
     * found in the haystack.
     */
    int indexOf(const StringPiece& bytes, int offset);

    RedisReplyMessagePtr reset();

private:
    int maxBulkSize;

    int bulkSize;
    int multiBulkSize;
    RedisReplyMessagePtr reply;
};

}
}

#endif //#if !defined(CETTY_REDIS_REDISREPLYMESSAGEDECODER_H)

// Local Variables:
// mode: c++
// End:
