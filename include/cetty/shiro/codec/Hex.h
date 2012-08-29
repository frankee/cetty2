#if !defined(CETTY_SHIRO_CODEC_HEX_H)
#define CETTY_SHIRO_CODEC_HEX_H
/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */
#include <string>
#include <cstring>

namespace cetty {
namespace shiro {
namespace codec {

/**
 * <a href="http://en.wikipedia.org/wiki/Hexadecimal">Hexadecimal</a> encoder and decoder.
 * <p/>
 * This class was borrowed from Apache Commons Codec SVN repository (rev. {@code 560660}) with modifications
 * to enable Hex conversion without a full dependency on Commons Codec.  We didn't want to reinvent the wheel of
 * great work they've done, but also didn't want to force every Shiro user to depend on the commons-codec.jar
 * <p/>
 * As per the Apache 2.0 license, the original copyright notice and all author and copyright information have
 * remained in tact.
 *
 * @see <a href="http://en.wikipedia.org/wiki/Hexadecimal">Wikipedia: Hexadecimal</a>
 * @since 0.9
 */
class Hex {

public:
    /**
     * Encodes the specifed string to a Hex string and then returns that hex String.
     *
     * @param in the binary array to Hex-encode.
     * @return A String representation of the resultant hex-encoded char array.
     */
    static std::string encode(const std::string &in);

    /**
     * Converts an array of character bytes representing hexidecimal values into an
     * array of bytes of those same values. The returned array will be half the
     * length of the passed array, as it takes two characters to represent any
     * given byte.
     *
     * @param array An array of character bytes containing hexidecimal digits
     * @return A byte array containing binary data decoded from
     *         the supplied byte array (representing characters).
     * @throws IllegalArgumentException Thrown if an odd number of characters is supplied
     *                                  to this function
     * @see #decode(char[])
     */
    static std::string decode(const std::string &in);

    /**
     * Converts a hexadecimal character to an integer.
     *
     * @param ch    A character to convert to an integer digit
     * @param index The index of the character in the source
     * @return An integer
     * @throws IllegalArgumentException if ch is an illegal hex character
     */
    static int toDigit(char ch, int index){ return 0;};
};
}
}
}

#endif //#if !defined(CETTY_SHIRO_CODEC_HEX_H)
