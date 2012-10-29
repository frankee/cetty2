#if !defined(CETTY_UTIL_BASE64_H)
#define CETTY_UTIL_BASE64_H

/*
 * Copyright (C) 2010 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
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

namespace cetty {
namespace util {

class Base64 {
public:
    /**
    * Default values for encoder/decoder flags.
    */
    static const int DEFAULT = 0;

    /**
     * Encoder flag bit to omit the padding '=' characters at the end
     * of the output (if any).
     */
    static const int NO_PADDING = 1;

    /**
     * Encoder flag bit to omit all line terminators (i.e., the output
     * will be on one long line).
     */
    static const int NO_WRAP = 2;

    /**
     * Encoder flag bit to indicate lines should be terminated with a
     * CRLF pair instead of just an LF.  Has no effect if {@code
     * NO_WRAP} is specified as well.
     */
    static const int CRLF = 4;

    /**
     * Encoder/decoder flag bit to indicate using the "URL and
     * filename safe" variant of Base64 (see RFC 3548 section 4) where
     * {@code -} and {@code _} are used in place of {@code +} and
     * {@code /}.
     */
    static const int URL_SAFE = 8;

    /**
     * Decode the Base64-encoded data in input and return the data in
     * a new byte array.
     *
     * <p>The padding '=' characters at the end are considered optional, but
     * if any are present, there must be the correct number of them.
     *
     * @param str    the input String to decode, which is converted to
     *               bytes using the default charset
     * @param flags  controls certain features of the decoded output.
     *               Pass {@code DEFAULT} to decode standard Base64.
     *
     * @throws IllegalArgumentException if the input contains
     * incorrect padding
     */
    static bool decode(const std::string& str, std::string* bytes) {
        return decode(str.c_str(), str.size(), DEFAULT, bytes);
    }
    static bool decode(const std::string& str, int flags, std::string* bytes) {
        return decode(str.c_str(), str.size(), flags, bytes);
    }

    static bool decode(const char* str, int size, std::string* bytes) {
        return decode(str, size, DEFAULT, bytes);
    }
    static bool decode(const char* str, int size, int flags, std::string* bytes);

    // todo Encoded result looks like error
    static bool encode(const std::string& bytes, std::string* str) {
        return encode(bytes, DEFAULT, str);
    }
    static bool encode(const std::string& bytes, int flags, std::string* str) {
        return encode(bytes.c_str(), bytes.size(), flags, str);
    }

    static bool encode(const char* bytes, int size, std::string* str) {
        return encode(bytes, size, DEFAULT, str);
    }

    /**
     * Base64-encode the given data and return a newly allocated
     * byte[] with the result.
     *
     * @param input  the data to encode
     * @param offset the position within the input array at which to
     *               start
     * @param len    the number of bytes of input to encode
     * @param flags  controls certain features of the encoded output.
     *               Passing {@code DEFAULT} results in output that
     *               adheres to RFC 2045.
     */
    static bool encode(const char* bytes, int size, int flags, std::string* str);

private:
    Base64();

};

}
}

#endif //#if !defined(CETTY_UTIL_BASE64_H)

// Local Variables:
// mode: c++
// End:
