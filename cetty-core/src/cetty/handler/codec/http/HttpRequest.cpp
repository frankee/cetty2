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
#include <cetty/util/StringPiece.h>

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
      uriStr_(uri),
      uri(uri) {
}

HttpRequest::HttpRequest(const HttpVersion& httpVersion,
                         const HttpMethod& method,
                         const StringPiece& uri)
    : HttpMessage(httpVersion),
      method(method),
      uriStr_(uri.data(), uri.size()),
      uri(uri) {

}

HttpRequest::~HttpRequest() {

}

const NameValueCollection& HttpRequest::queryParameters() const {
    if (queryParams_.empty()) {
        QueryStringDecoder queryStringDecoder(uri_);
        queryStringDecoder.getParameters(&queryParams_);
    }

    return queryParams_;
}

const std::vector<std::string>& HttpRequest::pathSegments() const {
    if (pathSegments_.empty()) {
        uri_.getPathSegments(pathSegments_);

        std::string& lastSegment = pathSegments_.back();
        std::string::size_type pos = lastSegment.find('.');

        if (pos != lastSegment.npos) {
            label_ = lastSegment.substr(pos + 1);
            lastSegment = lastSegment.substr(0, pos);
        }
    }

    return pathSegments_;
}

void HttpRequest::clear() {
    HttpMessage::clear();

    uri_.clear();
    uriStr_.clear();

    pathSegments_.clear();
    queryParams_.clear();
}

std::string HttpRequest::toString() const {
    std::string buf;
    buf.reserve(2048);

    StringUtil::printf(&buf,
                          "HttpRequest (TransferEncode: %s)\r\n%s %s %s",
                          transferEncoding().toString().c_str(),
                          method().toString().c_str(),
                          getUriString().c_str(),
                          version().getText().c_str());

    ConstHeaderIterator end = getLastHeader();

    for (ConstHeaderIterator itr = getFirstHeader(); itr != end; ++itr) {
        StringUtil::printf(&buf, "\r\n%s: %s", itr->first.c_str(), itr->second.c_str());
    }

    return buf;
}

const std::string& HttpRequest::label() const {
    return label_;
}

void HttpRequest::setUri(const StringPiece& uri) {
    if (!uri.empty()) {
        this->uriStr_.assign(uri.data(), uri.size());
        this->uri_ = this->uriStr_;
    }
}

void HttpRequest::setLabel(const std::string& label) {
    this->label_ = label;
}

}
}
}
}
