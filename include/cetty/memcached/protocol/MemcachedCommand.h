#if !defined(CETTY_MEMCACHED_PROTOCOL_MEMCACHEDCOMMAND_H)
#define CETTY_MEMCACHED_PROTOCOL_MEMCACHEDCOMMAND_H

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

#include <cetty/util/ReferenceCounter.h>

namespace cetty {
namespace memcached {
namespace protocol {

using namespace cetty::util;

class MemcachedCommand : public ReferenceCounter<MemcachedCommand, int> {
public:
private:
    bool noReply_;

    int flags_;
    int expirationTime_;
    int64_t casUnique_;

    std::string key_;
    ChannelBuffer buffer_;
};

}
}
}


#endif //#if !defined(CETTY_MEMCACHED_PROTOCOL_MEMCACHEDCOMMAND_H)

// Local Variables:
// mode: c++
// End:
