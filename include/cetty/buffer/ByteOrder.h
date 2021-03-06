#if !defined(CETTY_BUFFER_BYTEORDER_H)
#define CETTY_BUFFER_BYTEORDER_H

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

#include <string>
#include <cetty/util/Enum.h>

namespace cetty {
namespace buffer {

class ByteOrder : public cetty::util::Enum<ByteOrder> {
public:
    /**
     * BIG_ENDIAN, which name is "BIG_ENDIAN"
     */
    static const ByteOrder BIG;

    /**
     * LITTLE_ENDIAN, which name is "LITTLE_ENDIAN"
     */
    static const ByteOrder LITTLE;

    /**
     * Retrieves the native byte order of the underlying platform.
     *
     * @return The native byte order of the hardware.
     */
    static ByteOrder nativeOrder();

    static int64_t swap(int64_t value);

    static int32_t swap(int32_t value);

    static int16_t swap(int16_t value);

private:
    ByteOrder(int value, const char* name);
};

}
}

#endif //#if !defined(CETTY_BUFFER_BYTEORDER_H)

// Local Variables:
// mode: c++
// End:
