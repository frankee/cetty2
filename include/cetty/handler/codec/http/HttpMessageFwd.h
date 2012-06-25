#if !defined(CETTY_HANDLER_CODEC_HTTP_HTTPMESSAGEFWD_H)
#define CETTY_HANDLER_CODEC_HTTP_HTTPMESSAGEFWD_H

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

#include <boost/intrusive_ptr.hpp>

namespace cetty {
namespace handler {
namespace codec {
namespace http {

class HttpMessage;
class HttpRequest;
class HttpResponse;
class HttpChunk;
class HttpChunkTrailer;

typedef boost::intrusive_ptr<HttpMessage> HttpMessagePtr;
typedef boost::intrusive_ptr<HttpRequest> HttpRequestPtr;
typedef boost::intrusive_ptr<HttpResponse> HttpResponsePtr;
typedef boost::intrusive_ptr<HttpChunk> HttpChunkPtr;
typedef boost::intrusive_ptr<HttpChunkTrailer> HttpChunkTrailerPtr;

}
}
}
}

#endif //#if !defined(CETTY_HANDLER_CODEC_HTTP_HTTPMESSAGEFWD_H)

// Local Variables:
// mode: c++
// End:
