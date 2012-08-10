#if !defined(CETTY_SHIRO_CODEC_CODECSUPPORT_H)
#define CETTY_SHIRO_CODEC_CODECSUPPORT_H
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

namespace cetty {
namespace shiro {
namespace codec {

/**
 * Base abstract class that provides useful encoding and decoding operations, especially for character data.
 *
 * @since 0.9
 */
class CodecSupport {
public:

    /**
     * Shiro's default preferred character encoding, equal to <b><code>UTF-8</code></b>.
     */
    static const std::string PREFERRED_ENCODING = "UTF-8";
    /**
     * Converts the specified character array into a byte array using the specified character encoding.
     * <p/>
     * This is a convenience method equivalent to calling the {@link #toBytes(String,String)} method with a
     * a wrapping String and the specified encoding, i.e.
     * <p/>
     * <code>toBytes( new String(chars), encoding );</code>
     *
     * @param chars    the character array to be converted to a byte array
     * @param encoding the character encoding to use to when converting to bytes.
     * @return the bytes of the specified character array under the specified encoding.
     * @throws CodecException if the JVM does not support the specified encoding.
     */
    static std::string encode(const std::string &src, std::string encoding);

    /**
     * Converts the specified byte array to a String using the {@link CodecSupport#PREFERRED_ENCODING PREFERRED_ENCODING}.
     *
     * @param bytes the byte array to turn into a String.
     * @return the specified byte array as an encoded String ({@link CodecSupport#PREFERRED_ENCODING PREFERRED_ENCODING}).
     * @see #toString(byte[], String)
     */
    static std::string encode(const std::string &in) {
        return encode(in, PREFERRED_ENCODING);
    }
};
}
}
}
#endif // #if !defined(CETTY_SHIRO_CODEC_CODECSUPPORT_H)
