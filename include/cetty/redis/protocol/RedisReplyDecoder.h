#if !defined(CETTY_REDIS_PROTOCOL_REDISREPLYMESSAGEDECODER_H)
#define CETTY_REDIS_PROTOCOL_REDISREPLYMESSAGEDECODER_H

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
#include <cetty/redis/protocol/RedisReplyPtr.h>

namespace cetty {
namespace redis {
namespace protocol {

using namespace cetty::channel;
using namespace cetty::util;
using namespace cetty::handler::codec;

class RedisReplyMessageDecoder : private boost::noncopyable {
public:
    typedef ReplayingDecoder<RedisReplyMessageDecoder, RedisReplyPtr> Decoder;
    typedef Decoder::Context Context;
    typedef Decoder::Handler Handler;
    typedef Decoder::HandlerPtr HandlerPtr;

    typedef Decoder::CheckPointInvoker CheckPointInvoker;

public:
    enum State {
        READ_INITIAL,
        READ_BULK,
        READ_MULTI_BULK
    };

public:
    RedisReplyMessageDecoder()
        : maxBulkSize_(1024*1024),
          bulkSize_(0),
          multiBulkSize_(0) {
        init();
    }

    RedisReplyMessageDecoder(int maxBulkSize)
        : maxBulkSize_(maxBulkSize),
          bulkSize_(0),
          multiBulkSize_(0) {
        init();
    }

    ~RedisReplyMessageDecoder() {}


    void registerTo(Context& ctx) {
        decoder_.registerTo(ctx);
    }

private:
    void init() {
        checkPoint_ = decoder_.checkPointInvoker();

        decoder_.setInitialState(READ_INITIAL);
        decoder_.setDecoder(boost::bind(&RedisReplyMessageDecoder::decode,
                                        this,
                                        _1,
                                        _2,
                                        _3));
    }

    RedisReplyPtr decode(ChannelHandlerContext& ctx,
                         const ReplayingDecoderBufferPtr& buffer,
                         int state);

    RedisReplyPtr readMultiBukls(const ReplayingDecoderBufferPtr& buffer,
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

    RedisReplyPtr reset();

private:
    int maxBulkSize_;

    int bulkSize_;
    int multiBulkSize_;
    RedisReplyPtr reply_;

    Decoder decoder_;
    CheckPointInvoker checkPoint_;
};

}
}
}

#endif //#if !defined(CETTY_REDIS_PROTOCOL_REDISREPLYMESSAGEDECODER_H)

// Local Variables:
// mode: c++
// End:
