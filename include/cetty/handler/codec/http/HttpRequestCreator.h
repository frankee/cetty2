#if !defined(CETTY_HANDLER_CODEC_HTTP_HTTPREQUESTCREATOR_H)
#define CETTY_HANDLER_CODEC_HTTP_HTTPREQUESTCREATOR_H

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

#include <cetty/handler/codec/http/HttpRequest.h>

namespace cetty {
namespace handler {
namespace codec {
namespace http {

class HttpRequestCreator {
public:
    HttpRequestCreator() {
        request = new HttpRequest;
    }

    HttpRequestPtr create(const StringPiece& str1,
                          const StringPiece& str2,
                          const StringPiece& str3) {

        BOOST_ASSERT(!str1.empty() && !str2.empty() && !str3.empty()
                     && "createMessage parameters should not be NULL");

        if (request->refcount() == 1) { //if no one else use it, reuse it.
            request->clear();
            request->setVersion(HttpVersion::valueOf(str3));
            request->setMethod(HttpMethod::valueOf(str1));
            request->setUri(str2);
        }
        else {
            request.reset(new HttpRequest(HttpVersion::valueOf(str3),
                                          HttpMethod::valueOf(str1),
                                          str2));
        }

        return request;
    }

private:
    HttpRequestPtr request;
};

}
}
}
}

#endif //#if !defined(CETTY_HANDLER_CODEC_HTTP_HTTPREQUESTCREATOR_H)

// Local Variables:
// mode: c++
// End:
