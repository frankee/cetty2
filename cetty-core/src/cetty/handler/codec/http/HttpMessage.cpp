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

namespace cetty {
namespace handler {
namespace codec {
namespace http {

using namespace cetty::buffer;
using namespace cetty::util;

HttpMessage::HttpMessage(const HttpVersion& version)
    : version(version),
      content(Unpooled::EMPTY_BUFFER),
      transferEncoding(HttpTransferEncoding::SINGLE) {
    httpHeader.setValidateNameFunctor(HttpCodecUtil::validateHeaderName);
}

HttpMessage::HttpMessage()
    : version(HttpVersion::HTTP_1_1),
      content(Unpooled::EMPTY_BUFFER),
      transferEncoding(HttpTransferEncoding::SINGLE) {
    httpHeader.setValidateNameFunctor(HttpCodecUtil::validateHeaderName);
}

void HttpMessage::setContent(const ChannelBufferPtr& content) {
    if (!content) {
        this->content = Unpooled::EMPTY_BUFFER;
        return;
    }

    if (content->readable() && transferEncoding.isMultiple()) {
        throw InvalidArgumentException(
            "non-empty content disallowed if this.chunked == true");
    }

    this->content = content;
}

std::string HttpMessage::toString() const {
    std::string buf;
    buf.reserve(2048);

    StringUtil::strprintf(&buf,
                          "HttpMessage (version: %d, keepAlive: %s,  transferEncoding: %s)",
                          getProtocolVersion().getText().c_str(),
                          HttpHeaders::isKeepAlive(*this) ? "true" : "false",
                          transferEncoding.toString().c_str());

    ConstHeaderIterator end = getLastHeader();

    for (ConstHeaderIterator itr = getFirstHeader(); itr != end; ++itr) {
        StringUtil::strprintf(&buf, "\r\n%s: %s", itr->first.c_str(), itr->second.c_str());
    }

    return buf;
}

void HttpMessage::setHeader(const std::string& name, int value) {
    httpHeader.set(name, StringUtil::strprintf("%d", value));
}

HttpTransferEncoding HttpMessage::getTransferEncoding() const {
    return transferEncoding;
}

void HttpMessage::setTransferEncoding(HttpTransferEncoding te) {
    this->transferEncoding = te;
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

}
}
}
}
