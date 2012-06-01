/*
 * Copyright (c) 2010-2011 frankee zhou (frankee.zhou at gmail dot com)
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

#include <cetty/buffer/ChannelBuffer.h>
#include <cetty/handler/codec/http/HttpRequest.h>
#include <cetty/util/StringUtil.h>

namespace cetty {
namespace handler {
namespace codec {
namespace http {

using namespace cetty::buffer;
using namespace cetty::util;

HttpRequest::HttpRequest(const HttpVersion& httpVersion,
    const HttpMethod& method,
    const std::string& uri)
    : HttpMessage(httpVersion),
    method(method),
    uriStr(uri),
    uri(uri) {
}

HttpRequest::~HttpRequest() {

}

const NameValueCollection& HttpRequest::getQueryParameters() const {
    if (queryParams.empty()) {
        QueryStringDecoder queryStringDecoder(uri);
        queryStringDecoder.getParameters(&queryParams);
    }

    return queryParams;
}

const std::vector<std::string>& HttpRequest::getPathSegments() const {
    if (pathSegments.empty()) {
        uri.getPathSegments(pathSegments);
    }

    return pathSegments;
}

void HttpRequest::clear() {
    HttpMessage::clear();

    uri.clear();
    uriStr.clear();

    pathSegments.clear();
    queryParams.clear();
}

std::string HttpRequest::toString() const {
    std::string buf;
    buf.reserve(2048);

    StringUtil::strprintf(&buf,
        "HttpRequest (chunked: %s)\r\n%s %s %s",
        isChunked() ? "true" : "false",
        getMethod().toString().c_str(),
        getUri().c_str(),
        getProtocolVersion().getText().c_str());

    ConstHeaderIterator end = getLastHeader();
    for (ConstHeaderIterator itr = getFirstHeader(); itr != end; ++itr) {
        StringUtil::strprintf(&buf, "\r\n%s: %s", itr->first.c_str(), itr->second.c_str());
    }

    return buf;
}

}
}
}
}