#if !defined(CETTY_SHIRO_CODEC_BASE64_H)
#define CETTY_SHIRO_CODEC_BASE64_H
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
 * Provides <a href="http://en.wikipedia.org/wiki/Base64">Base 64</a> encoding and decoding as defined by
 * <a href="http://www.ietf.org/rfc/rfc2045.txt">RFC 2045</a>.
 * <p/>
 * This class implements section <cite>6.8. Base64 Content-Transfer-Encoding</cite> from RFC 2045 <cite>Multipurpose
 * Internet Mail Extensions (MIME) Part One: Format of Internet Message Bodies</cite> by Freed and Borenstein.
 * <p/>
 * This class was borrowed from Apache Commons Codec SVN repository (rev. 618419) with modifications
 * to enable Base64 conversion without a full dependecny on Commons Codec.  We didn't want to reinvent the wheel of
 * great work they've done, but also didn't want to force every Shiro user to depend on the commons-codec.jar
 * <p/>
 * As per the Apache 2.0 license, the original copyright notice and all author and copyright information have
 * remained in tact.
 *
 * @see <a href="http://en.wikipedia.org/wiki/Base64">Wikipedia: Base 64</a>
 * @see <a href="http://www.ietf.org/rfc/rfc2045.txt">RFC 2045</a>
 * @since 0.9
 */
class Base64 {
public:
    /**
     * Encodes binary data using the base64 algorithm and chunks the encoded output into 76 character blocks
     *
     * @param binaryData binary data to encodeToChars
     * @return Base64 characters chunked in 76 character blocks
     */
    static std::string encodeChunked(const std::string &binary) {
        return encode(binary, true);
    }

    /**
     * Encodes a byte[] containing binary data, into a byte[] containing characters in the Base64 alphabet.
     *
     * @param pArray a byte array containing binary data
     * @return A byte array containing only Base64 character data
     */
    static std::string encode(const std::string &pArray) {
        return encode(pArray, false);
    }

    /**
     * Encodes binary data using the base64 algorithm, optionally chunking the output into 76 character blocks.
     *
     * @param binaryData Array containing binary data to encodeToChars.
     * @param isChunked  if <code>true</code> this encoder will chunk the base64 output into 76 character blocks
     * @return Base64-encoded data.
     */
    static std::string encode(const std::string &in, bool isChunked);

    /**
     * Converts the specified UTF-8 Base64 encoded String and decodes it to a raw Base64 decoded byte array.
     *
     * @param base64Encoded a UTF-8 Base64 encoded String
     * @return the raw Base64 decoded byte array.
     */
    static std::string decode(const std::string &in);
};
}
}
}

#endif //#if !defined(CETTY_SHIRO_CODEC_BASE64_H)
