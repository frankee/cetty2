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
#include <cetty/handler/codec/http/Cookie.h>

namespace cetty {
namespace handler {
namespace codec {
namespace http {

/**
 * Encodes client-side {@link Cookie}s into an HTTP header value.  This encoder can encode
 * the HTTP cookie version 0, 1, and 2.
 * <pre>
 * // Example
 * {@link HttpRequest} req = ...;
 * res.setHeader("Cookie", {@link ClientCookieEncoder}.encode("JSESSIONID", "1234"));
 * </pre>
 *
 * @see CookieDecoder
 *
 * @apiviz.stereotype utility
 * @apiviz.has        io.netty.handler.codec.http.Cookie oneway - - encodes
 */
class ClientCookieEncoder {
public:
    /**
     * Encodes the specified cookie into an HTTP header value.
     */
    static void encode(const std::string& name,
                       const std::string& value,
                       std::string* out);

    static void encode(const Cookie& cookie, std::string* out);

private:
    ClientCookieEncoder() {}
};

inline
void ClientCookieEncoder::encode(const std::string& name,
                                 const std::string& value,
                                 std::string* out) {
    Cookie cookie(name, value);
    encode(cookie, out);
}

}
}
}
}
