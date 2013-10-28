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

#include <cetty/buffer/ByteOrder.h>

namespace cetty {
namespace buffer {

const ByteOrder ByteOrder::BIG(1, "BIG_ENDIAN");
const ByteOrder ByteOrder::LITTLE(2, "LITTLE_ENDIAN");

ByteOrder::ByteOrder(int value, const char* name)
    : cetty::util::Enum<ByteOrder>(value, name) {
}

ByteOrder ByteOrder::nativeOrder() {
    static const char tmp[2] = {0x01, 0x02};

    if (*(reinterpret_cast<const short*>(tmp)) == 0x0102) {
        return BIG;
    }
    else {
        return LITTLE;
    }
}

int64_t ByteOrder::swap(int64_t value) {
    const char* buffer = reinterpret_cast<const char*>(&value);
    return ((static_cast<int64_t>(buffer[0]) & 0xff) << 56) |
           ((static_cast<int64_t>(buffer[1]) & 0xff) << 48) |
           ((static_cast<int64_t>(buffer[2]) & 0xff) << 40) |
           ((static_cast<int64_t>(buffer[3]) & 0xff) << 32) |
           ((static_cast<int64_t>(buffer[4]) & 0xff) << 24) |
           ((static_cast<int64_t>(buffer[5]) & 0xff) << 16) |
           ((static_cast<int64_t>(buffer[6]) & 0xff) <<  8) |
           ((static_cast<int64_t>(buffer[7]) & 0xff) <<  0);

}

int32_t ByteOrder::swap(int32_t value) {
    const char* buffer = reinterpret_cast<const char*>(&value);
    return ((static_cast<int32_t>(buffer[0]) & 0xff) << 24) |
           ((static_cast<int32_t>(buffer[1]) & 0xff) << 16) |
           ((static_cast<int32_t>(buffer[2]) & 0xff) << 8) |
           ((static_cast<int32_t>(buffer[3]) & 0xff) << 0);
}

int16_t ByteOrder::swap(int16_t value) {
    const char* buffer = reinterpret_cast<const char*>(&value);
    return ((static_cast<int16_t>(buffer[0]) & 0xff) <<  8) |
           ((static_cast<int16_t>(buffer[1]) & 0xff) <<  0);
}

}
}
