#if !defined(CETTY_UTIL_BASE64_H)
#define CETTY_UTIL_BASE64_H

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
     * Flag to pass to {@link Base64OutputStream} to indicate that it
     * should not close the output stream it is wrapping when it
     * itself is closed.
     */
    static const int NO_CLOSE = 16;


    static void decode(const std::string& str, std::string* bytes) {
        decode(str.c_str(), str.size(), 0, bytes);
    }
    static void decode(const std::string& str, int flags, std::string* bytes) {
        decode(str.c_str(), str.size(), flags, bytes);
    }

    static void decode(const char* str, int size, std::string* bytes) {
        decode(str, size, 0, bytes);
    }
    static void decode(const char* str, int size, int flags, std::string* bytes);


    static void encode(const std::string& bytes, std::string* str) {
        encode(bytes, 0, str);
    }
    static void encode(const std::string& bytes, int flags, std::string* str);

    static void encode(const char* bytes, int size, std::string* str) {
        encode(bytes, size, 0, str);
    }

    static void encode(const char* bytes, int size, int flags, std::string* str);

private:
    Base64();

};

}
}

#endif //#if !defined(CETTY_UTIL_BASE64_H)

// Local Variables:
// mode: c++
// End:
