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

const ByteOrder ByteOrder::BO_BIG_ENDIAN = 0;
const ByteOrder ByteOrder::BO_LITTLE_ENDIAN = 1;

ByteOrder ByteOrder::nativeOrder() {
    static const char tmp[2] = {0x01, 0x02};

    if (*((const short*)tmp) == 0x0102) {
        return BO_BIG_ENDIAN;
    }

    return BO_LITTLE_ENDIAN;
}

std::string ByteOrder::toString() const {
    if (v == BO_BIG_ENDIAN.v) {
        return "BIG_ENDIAN";
    }

    return "LITTLE_ENDIAN";
}

}
}
