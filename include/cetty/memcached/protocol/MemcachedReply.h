#if !defined(CETTY_MEMCACHED_PROTOCOL_MEMCACHEDREPLY_H)
#define CETTY_MEMCACHED_PROTOCOL_MEMCACHEDREPLY_H

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

#include <cetty/Types.h>
#include <cetty/util/StringPiece.h>
#include <cetty/util/ReferenceCounter.h>
#include <cetty/buffer/ChannelBufferPtr.h>
#include <cetty/memcached/protocol/MemcachedReply.h>

namespace cetty {
namespace memcached {
namespace protocol {

    using namespace cetty::util;
    using namespace cetty::buffer;

class MemcachedReply : public ReferenceCounter<MemcachedReply, int> {
public:
    class Value {
    public:
    private:
        int flags_;
        int64_t casUnique_;

        StringPiece key_;
        StringPiece data_;
    };

    class Stat {
    public:
    private:
        std::string& name;
        union {

        } value;
    };

private:
    int code_;
    std::string message_;

    ChannelBufferPtr buffer_;
};

}
}
}


#endif //#if !defined(CETTY_MEMCACHED_PROTOCOL_MEMCACHEDREPLY_H)

// Local Variables:
// mode: c++
// End:
