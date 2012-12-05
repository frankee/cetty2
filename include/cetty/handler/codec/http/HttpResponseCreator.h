#if !defined(CETTY_HANDLER_CODEC_HTTP_HTTPRESPONSECREATOR_H)
#define CETTY_HANDLER_CODEC_HTTP_HTTPRESPONSECREATOR_H

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

#include <cetty/handler/codec/http/HttpPackage.h>
#include <cetty/handler/codec/http/HttpVersion.h>
#include <cetty/handler/codec/http/HttpResponse.h>
#include <cetty/handler/codec/http/HttpResponseStatus.h>

namespace cetty {
namespace handler {
namespace codec {
namespace http {

class HttpResponseCreator {
public:
    HttpResponseCreator()
        : response(new HttpResponse) {
    }

    HttpPackage create(const StringPiece& str1,
                       const StringPiece& str2,
                       const StringPiece& str3) {
        if (response->refcount() == 1) {
            response->clear();
            response->setVersion(HttpVersion::valueOf(str1));
            response->setStatus(HttpResponseStatus(StringUtil::strto32(str2), str3));
        }
        else {
            response.reset(new HttpResponse(
                               HttpVersion::valueOf(str1),
                               HttpResponseStatus(StringUtil::strto32(str2), str3)));
        }

        return response;
    }

private:
    HttpResponsePtr response;
};

}
}
}
}


#endif //#if !defined(CETTY_HANDLER_CODEC_HTTP_HTTPRESPONSECREATOR_H)

// Local Variables:
// mode: c++
// End:
