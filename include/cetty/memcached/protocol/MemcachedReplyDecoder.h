#if !defined(CETTY_MEMCACHED_PROTOCOL_MEMCACHEDREPLYDECODER_H)
#define CETTY_MEMCACHED_PROTOCOL_MEMCACHEDREPLYDECODER_H

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

#include <boost/noncopyable.hpp>
#include <cetty/handler/codec/ReplayingDecoder.h>
#include <cetty/memcached/protocol/MemcachedReplyPtr.h>

namespace cetty {
namespace memcached {
namespace protocol {

using namespace cetty::channel;
using namespace cetty::util;
using namespace cetty::handler::codec;

class MemcachedReplyDecoder : private boost::noncopyable {
public:
    typedef MemcachedReplyDecoder Self;
    typedef ReplayingDecoder<Self, MemcachedReplyPtr> Decoder;

    typedef Decoder::Context Context;
    typedef Decoder::Handler Handler;
    typedef Decoder::HandlerPtr HandlerPtr;

    typedef Decoder::CheckPointInvoker CheckPointInvoker;

public:
    enum State {
        READ_INITIAL,
        READ_DATA,
        READ_VALUE,
        READ_STAT,
    };

public:
    MemcachedReplyDecoder()
        : maxBulkSize_(1024*1024),
          bulkSize_(0),
          multiBulkSize_(0) {
        init();
    }

    MemcachedReplyDecoder(int maxBulkSize)
        : maxBulkSize_(maxBulkSize),
          bulkSize_(0),
          multiBulkSize_(0) {
        init();
    }

    ~MemcachedReplyDecoder() {}


    void registerTo(Context& ctx) {
        decoder_.registerTo(ctx);
    }

private:
    void init() {
        checkPoint_ = decoder_.checkPointInvoker();

        decoder_.setInitialState(READ_INITIAL);
        decoder_.setDecoder(boost::bind(&MemcachedReplyDecoder::decode,
                                        this,
                                        _1,
                                        _2,
                                        _3));
    }

    MemcachedReplyPtr decode(ChannelHandlerContext& ctx,
                         const ReplayingDecoderBufferPtr& buffer,
                         int state);

    void fail(ChannelHandlerContext& ctx, long frameLength);

    /**
     * Returns the number of bytes between the readerIndex of the haystack and
     * the first "\r\n" found in the haystack.  -1 is returned if no "\r\n" is
     * found in the haystack.
     */
    int indexOf(const StringPiece& bytes, int offset);

private:
    int maxBulkSize_;

    int bulkSize_;
    int multiBulkSize_;
    MemcachedReplyPtr reply_;

    Decoder decoder_;
    CheckPointInvoker checkPoint_;
};

}
}
}

#endif //#if !defined(CETTY_MEMCACHED_PROTOCOL_MEMCACHEDREPLYDECODER_H)

// Local Variables:
// mode: c++
// End:
