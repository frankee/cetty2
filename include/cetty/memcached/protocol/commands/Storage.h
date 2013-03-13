#if !defined(CETTY_MEMCACHED_PROTOCOL_COMMANDS_STORAGE_H)
#define CETTY_MEMCACHED_PROTOCOL_COMMANDS_STORAGE_H

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

#include <string>
#include <cetty/Types.h>
#include <cetty/util/StringPiece.h>
#include <cetty/memcached/protocol/MemcachedCommandPtr.h>

namespace cetty {
namespace memcached {
namespace protocol {
namespace commands {

using namespace cetty::util;

MemcachedCommandPtr set(const std::string& key,
                        int flags,
                        int expiry,
                        const StringPiece& value);

MemcachedCommandPtr add(const std::string& key,
                        int flags,
                        int expiry,
                        const StringPiece& value);

MemcachedCommandPtr replace(const std::string& key,
                            int flags,
                            int expiry,
                            const StringPiece& value);

MemcachedCommandPtr append(const std::string& key,
                           int flags,
                           int expiry,
                           const StringPiece& value);

MemcachedCommandPtr prepend(const std::string& key,
                            int flags,
                            int expiry,
                            const StringPiece& value);

MemcachedCommandPtr cas(const std::string& key,
                        int flags,
                        int expiry,
                        int64_t casUnique,
                        const StringPiece& value);

}
}
}
}

#endif //#if !defined(CETTY_MEMCACHED_PROTOCOL_COMMANDS_STORAGE_H)

// Local Variables:
// mode: c++
// End:
