#if !defined(CETTY_MEMCACHED_PROTOCOL_MEMCACHEDCOMMANDDECODER_H)
#define CETTY_MEMCACHED_PROTOCOL_MEMCACHEDCOMMANDDECODER_H

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
#include <cetty/handler/codec/BufferToMessageDecoder.h>
#include <cetty/memcached/protocol/MemcachedCommandPtr.h>

namespace cetty {
namespace memcached {
namespace protocol {

using namespace cetty::channel;
using namespace cetty::util;
using namespace cetty::handler::codec;

class MemcachedCommondDecoder : private boost::noncopyable {
public:
    typedef MemcachedCommondDecoder Self;
    typedef BufferToMessageDecoder<Self, MemcachedCommandPtr> Decoder;

    typedef Decoder::Context Context;
    typedef Decoder::Handler Handler;
    typedef Decoder::HandlerPtr HandlerPtr;

public:
    MemcachedCommondDecoder()
        : maxBulkSize_(1024*1024),
          bulkSize_(0),
          multiBulkSize_(0) {
    }

    ~MemcachedCommondDecoder() {}


    void registerTo(Context& ctx) {
        decoder_.registerTo(ctx);
    }

private:
    void init() {

        decoder_.setInitialState(READ_INITIAL);
        decoder_.setDecoder(boost::bind(&MemcachedReplyMessageDecoder::decode,
                                        this,
                                        _1,
                                        _2,
                                        _3));
    }


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

    Decoder decoder_;
};

}
}
}

#endif //#if !defined(CETTY_MEMCACHED_PROTOCOL_MEMCACHEDCOMMANDDECODER_H)

// Local Variables:
// mode: c++
// End:
