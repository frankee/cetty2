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
#include <cetty/handler/codec/http/HttpRequest.h>
#include <cetty/handler/codec/http/HttpResponse.h>
#include <cetty/handler/codec/http/HttpChunkTrailer.h>

namespace cetty {
namespace handler {
namespace codec {
namespace http {

class HttpPackage {
public:
    typedef boost::variant<HttpRequestPtr, HttpResponsePtr, HttpChunkPtr, HttpChunkTrailerPtr> Variant;

public:
    Variant variant;

public:
    HttpPackage() {}
    HttpPackage(const HttpChunkPtr& chunk)
        : variant(chunk) {
    }
    HttpPackage(const HttpRequestPtr& request)
        : variant(request) {
    }
    HttpPackage(const HttpResponsePtr& response)
        : variant(response) {
    }
    HttpPackage(const HttpChunkTrailerPtr& chunkTailer)
        : variant(chunkTailer) {
    }

    operator bool() const;

    int contentLength(int defaultValue) const;
    bool is100ContinueExpected() const;
    const HttpTransferEncoding& transferEncoding() const;

    void setTransferEncoding(const HttpTransferEncoding& te);
    void setContent(const ChannelBufferPtr& content);

    HttpHeaders* headers();

    bool isHttpRequest() const;
    bool isHttpResponse() const;
    bool isHttpChunk() const;
    bool isHttpChunkTrailer() const;

    HttpRequestPtr httpRequest() const;
    HttpResponsePtr httpResponse() const;
    HttpChunkPtr httpChunk() const;
    HttpChunkTrailerPtr httpChunkTrailer() const;
};

inline
bool HttpPackage::isHttpRequest() const {
    return variant.which() == 0;
}

inline
bool HttpPackage::isHttpResponse() const {
    return variant.which() == 1;
}

inline
bool HttpPackage::isHttpChunk() const {
    return variant.which() == 2;
}

inline
bool HttpPackage::isHttpChunkTrailer() const {
    return variant.which() == 3;
}

inline
HttpRequestPtr HttpPackage::httpRequest() const {
    if (isHttpRequest()) {
        return boost::get<HttpRequestPtr>(variant);
    }

    return HttpRequestPtr();
}

inline
HttpResponsePtr HttpPackage::httpResponse() const {
    if (isHttpResponse()) {
        return boost::get<HttpResponsePtr>(variant);
    }

    return HttpResponsePtr();
}

inline
HttpChunkPtr HttpPackage::httpChunk() const {
    if (isHttpChunk()) {
        return boost::get<HttpChunkPtr>(variant);
    }

    return HttpChunkPtr();
}

inline
HttpChunkTrailerPtr HttpPackage::httpChunkTrailer() const {
    if (isHttpChunkTrailer()) {
        return boost::get<HttpChunkTrailerPtr>(variant);
    }

    return HttpChunkTrailerPtr();
}

}
}
}
}

#endif //#if !defined(CETTY_HANDLER_CODEC_HTTP_HTTPPACKAGE_H)

// Local Variables:
// mode: c++
// End:
