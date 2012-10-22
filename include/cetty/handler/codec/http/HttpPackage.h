#if !defined(CETTY_HANDLER_CODEC_HTTP_HTTPPACKAGE_H)
#define CETTY_HANDLER_CODEC_HTTP_HTTPPACKAGE_H

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

#include <boost/variant.hpp>

#include <cetty/handler/codec/http/HttpChunk.h>
#include <cetty/handler/codec/http/HttpMessage.h>
#include <cetty/handler/codec/http/HttpChunkTrailer.h>

namespace cetty {
namespace handler {
namespace codec {
namespace http {

class HttpPackage {
public:
    typedef boost::variant<HttpMessagePtr, HttpChunkPtr, HttpChunkTrailerPtr> Variant;

public:
    Variant variant;

public:
    HttpPackage() {}
    HttpPackage(const HttpChunkPtr& chunk) : variant(chunk) {}
    HttpPackage(const HttpMessagePtr& message) : variant(message) {}
    HttpPackage(const HttpChunkTrailerPtr& chunkTailer) : variant(chunkTailer) {}

    operator bool() const;
};

}
}
}
}

#endif //#if !defined(CETTY_HANDLER_CODEC_HTTP_HTTPPACKAGE_H)

// Local Variables:
// mode: c++
// End:
