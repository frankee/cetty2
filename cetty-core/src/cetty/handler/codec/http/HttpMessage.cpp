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
#include <cetty/handler/codec/http/HttpMessage.h>

#include <cetty/buffer/Unpooled.h>
#include <cetty/util/StringUtil.h>
#include <cetty/util/Exception.h>
#include <cetty/handler/codec/http/HttpVersion.h>
#include <cetty/handler/codec/http/HttpCodecUtil.h>
#include <cetty/handler/codec/http/HttpHeaders.h>
#include <cetty/handler/codec/http/Cookie.h>

namespace cetty {
namespace handler {
namespace codec {
namespace http {

using namespace cetty::buffer;
using namespace cetty::util;

HttpMessage::HttpMessage(const HttpVersion& version)
    : version(version),
      content_(Unpooled::EMPTY_BUFFER),
      transferEncoding_(HttpTransferEncoding::SINGLE) {
    httpHeader.setValidateNameFunctor(HttpCodecUtil::validateHeaderName);
}

HttpMessage::HttpMessage()
    : version_(HttpVersion::HTTP_1_1),
      content_(Unpooled::EMPTY_BUFFER),
      transferEncoding_(HttpTransferEncoding::SINGLE) {
    httpHeader.setValidateNameFunctor(HttpCodecUtil::validateHeaderName);
}

void HttpMessage::setContent(const ChannelBufferPtr& content) {
    if (!content) {
        this->content_ = Unpooled::EMPTY_BUFFER;
        return;
    }

    if (content->readable() && transferEncoding_.isMultiple()) {
        throw InvalidArgumentException(
            "non-empty content disallowed if this.chunked == true");
    }

    this->content_ = content;
}

std::string HttpMessage::toString() const {
    std::string buf;
    buf.reserve(2048);

    StringUtil::printf(&buf,
                          "HttpMessage (version: %d, keepAlive: %s,  transferEncoding: %s)",
                          getProtocolVersion().getText().c_str(),
                          HttpHeaders::keepAlive(*this) ? "true" : "false",
                          transferEncoding_.toString().c_str());

    ConstHeaderIterator end = getLastHeader();

    for (ConstHeaderIterator itr = getFirstHeader(); itr != end; ++itr) {
        StringUtil::printf(&buf, "\r\n%s: %s", itr->first.c_str(), itr->second.c_str());
    }

    return buf;
}

void HttpMessage::setHeader(const std::string& name, int value) {
    httpHeader.set(name, StringUtil::printf("%d", value));
}

HttpTransferEncoding HttpMessage::getTransferEncoding() const {
    return transferEncoding_;
}

void HttpMessage::setTransferEncoding(HttpTransferEncoding te) {
    this->transferEncoding_ = te;
    const std::string& transferEnocodingStr = HttpHeaders::Names::TRANSFER_ENCODING;
    const std::string& chunkedStr = HttpHeaders::Values::CHUNKED;

    if (te == HttpTransferEncoding::SINGLE) {
        httpHeader.erase(transferEnocodingStr, chunkedStr);
    }
    else if (te == HttpTransferEncoding::STREAMED) {
        httpHeader.erase(transferEnocodingStr, chunkedStr);
        setContent(Unpooled::EMPTY_BUFFER);
    }
    else if (te == HttpTransferEncoding::CHUNKED) {
        if (!httpHeader.has(transferEnocodingStr, chunkedStr)) {
            addHeader(transferEnocodingStr, chunkedStr);
        }

        removeHeader(HttpHeaders::Names::CONTENT_LENGTH);
        setContent(Unpooled::EMPTY_BUFFER);
    }
}

void HttpMessage::addCookie(const Cookie& cookie) {
    cookies_.push_back(cookie);
}

void HttpMessage::addCookie(const std::string& name, const std::string& value) {
    Cookie cookie(name, value);
    cookies_.push_back(cookie);
}

const std::vector<Cookie>& HttpMessage::getCookies() const {
    if (cookies_.empty()) {
        const std::string& header = getHeader(HttpHeaders::Names::COOKIE);
        if (!header.empty()) {


            return cookies_;
        }
        
        NameValueCollection::ConstIterator lower = httpHeader.lowerBound(HttpHeaders::Names::SET_COOKIE);
        if (lower != httpHeader.end()) {
            NameValueCollection::ConstIterator up =
                httpHeader.upperBound(HttpHeaders::Names::SET_COOKIE);

            for (NameValueCollection::ConstIterator itr = lower; itr != up; ++itr) {

            }
        }
    }

    return cookies_;
}

}
}
}
}
