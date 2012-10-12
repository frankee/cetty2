/*
 * Copyright 2012 The Netty Project
 *
 * The Netty Project licenses this file to you under the Apache License,
 * version 2.0 (the "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at:
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations
 * under the License.
 */

#include <string>
#include <vector>

namespace cetty {
namespace handler {
namespace codec {
namespace http {

class Cookie;

/**
 * Decodes an HTTP header value into {@link Cookie}s.  This decoder can decode
 * the HTTP cookie version 0, 1, and 2.
 *
 * <pre>
 * {@link HttpRequest} req = ...;
 * String value = req.getHeader("Cookie");
 * Set&lt;{@link Cookie}&gt; cookies = new {@link CookieDecoder}().decode(value);
 * </pre>
 *
 * @see ClientCookieEncoder
 * @see ServerCookieEncoder
 *
 * @apiviz.stereotype utility
 * @apiviz.has        io.netty.handler.codec.http.Cookie oneway - - decodes
 */
class CookieDecoder {
public:
    /**
     * Decodes the specified HTTP header value into {@link Cookie}s.
     *
     * @return the decoded {@link Cookie}s
     */
    static void decode(const std::string& header, std::vector<Cookie>* cookies);

private:

private:
    static void extractKeyValuePairs(const std::string& header,
                                     std::vector<std::string>* names,
                                     std::vector<std::string>* values);

    CookieDecoder() {}
};

}
}
}
}
