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
    : version_(httpVersion),
      method_(method),
      uriStr_(uri),
      uri_(uri) {
}

HttpRequest::HttpRequest(const HttpVersion& httpVersion,
                         const HttpMethod& method,
                         const StringPiece& uri)
    : version_(httpVersion),
      method_(method),
      uriStr_(uri.data(), uri.size()),
      uri_(uri) {
}

HttpRequest::~HttpRequest() {

}

const NameValueCollection& HttpRequest::queryParameters() const {
    if (queryParams_.empty()) {
        if (headers_.headerValue(HttpHeaders::Names::CONTENT_TYPE) == HttpHeaders::Values::X_WWW_FORM_URLENCODED) {
            std::string contentStr;
            content_->readBytes(&contentStr);
            std::string decodedStr;
            URI::decode(contentStr, decodedStr);
            QueryStringDecoder::decodeParams(decodedStr, queryParams_);
        }
        else {
            QueryStringDecoder queryStringDecoder(uri_);
            queryStringDecoder.getParameters(&queryParams_);
        }
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
    headers_.clear();

    uri_.clear();
    uriStr_.clear();

    pathSegments_.clear();
    queryParams_.clear();
}

bool HttpRequest::is100ContinueExpected() const {
    // Expect: 100-continue is for requests only.

    //It works only on HTTP/1.1 or later.
    if (version_.compareTo(HttpVersion::HTTP_1_1) < 0) {
        return false;
    }

    // In most cases, there will be one or zero 'Expect' header.
    const std::string& value = headers_.headerValue(HttpHeaders::Names::EXPECT);

    if (value.empty()) {
        return false;
    }

    if (StringUtil::iequals(HttpHeaders::Values::CONTINUE, value)) {
        return true;
    }

    // Multiple 'Expect' headers.  Search through them.
    std::vector<std::string> headers;
    headers_.headerValues(HttpHeaders::Names::EXPECT, &headers);

    for (size_t i = 0; i < headers.size(); ++i) {
        if (StringUtil::iequals(HttpHeaders::Values::CONTINUE, headers[i])) {
            return true;
        }
    }

    return false;
}

void HttpRequest::set100ContinueExpected(bool set) {
    if (set) {
        headers_.setHeader(HttpHeaders::Names::EXPECT,
                           HttpHeaders::Values::CONTINUE);
    }
    else {
        headers_.removeHeader(HttpHeaders::Names::EXPECT);
    }
}

std::string HttpRequest::toString() const {
    std::string buf;
    buf.reserve(2048);
    //
    //     StringUtil::printf(&buf,
    //                           "HttpRequest (TransferEncode: %s)\r\n%s %s %s",
    //                           transferEncoding().toString().c_str(),
    //                           method().toString().c_str(),
    //                           getUriString().c_str(),
    //                           version().getText().c_str());
    //
    //     ConstHeaderIterator end = getLastHeader();
    //
    //     for (ConstHeaderIterator itr = getFirstHeader(); itr != end; ++itr) {
    //         StringUtil::printf(&buf, "\r\n%s: %s", itr->first.c_str(), itr->second.c_str());
    //     }

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
