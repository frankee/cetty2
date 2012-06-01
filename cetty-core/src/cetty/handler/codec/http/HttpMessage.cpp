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

#include <cetty/buffer/ChannelBuffers.h>
#include <cetty/util/StringUtil.h>
#include <cetty/util/internal/ConversionUtil.h>
#include <cetty/util/Exception.h>
#include <cetty/handler/codec/http/HttpVersion.h>
#include <cetty/handler/codec/http/HttpCodecUtil.h>
#include <cetty/handler/codec/http/HttpHeaders.h>

namespace cetty {
namespace handler {
namespace codec {
namespace http {

using namespace cetty::buffer;
using namespace cetty::util;

HttpMessage::HttpMessage(const HttpVersion& version)
    : chunked(false),
      version(version),
      content(ChannelBuffers::EMPTY_BUFFER) {
    httpHeader.setValidateNameFunctor(HttpCodecUtil::validateHeaderName);
}

HttpMessage::HttpMessage()
    : chunked(false),
      version(HttpVersion::HTTP_1_1),
      content(ChannelBuffers::EMPTY_BUFFER) {
            httpHeader.setValidateNameFunctor(HttpCodecUtil::validateHeaderName);
}

bool HttpMessage::isChunked() const {
    if (chunked) {
        return true;
    }
    else {
        return HttpCodecUtil::isTransferEncodingChunked(*this);
    }
}

void HttpMessage::setChunked(bool chunked) {
    this->chunked = chunked;

    if (chunked) {
        setContent(ChannelBuffers::EMPTY_BUFFER);
    }
}

void HttpMessage::setContent(const ChannelBufferPtr& content) {
    if (!content) {
        this->content = ChannelBuffers::EMPTY_BUFFER;
    }

    if (content->readable() && isChunked()) {
        throw InvalidArgumentException(
            "non-empty content disallowed if this.chunked == true");
    }

    this->content = content;
}

std::string HttpMessage::toString() const {
    std::string buf;
    buf.reserve(2048);

    StringUtil::strprintf(&buf,
        "HttpMessage (version: %d, keepAlive: %s,  chunked: %s)",
        getProtocolVersion().getText().c_str(),
        HttpHeaders::isKeepAlive(*this) ? "true" : "false",
        isChunked() ? "true" : "false");

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
