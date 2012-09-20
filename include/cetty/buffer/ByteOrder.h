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
    static const ByteOrder BO_BIG_ENDIAN;
    static const ByteOrder BO_LITTLE_ENDIAN;

    /**
     * Retrieves the native byte order of the underlying platform.
     *
     * @return The native byte order of the hardware.
     */
    static ByteOrder nativeOrder();

    /**
     * Constructs a string describing this enum.
     * This method returns the string "BIG_ENDIAN" for BIG_ENDIAN and
     * "LITTLE_ENDIAN" for LITTLE_ENDIAN.
     *
     * @return The specified string
     */
    std::string toString() const;

private:
    ByteOrder(int value) : cetty::util::Enum<ByteOrder>(value) {}
};

}
}

#endif //#if !defined(CETTY_BUFFER_BYTEORDER_H)

// Local Variables:
// mode: c++
// End:
