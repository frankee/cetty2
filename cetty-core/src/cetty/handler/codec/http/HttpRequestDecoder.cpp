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

#include <cetty/handler/codec/http/HttpRequestDecoder.h>

#include <cetty/util/StringPiece.h>

namespace cetty {
namespace handler {
namespace codec {
namespace http {


HttpRequestDecoder::HttpRequestDecoder(int maxInitialLineLength,
                                       int maxHeaderSize,
                                       int maxChunkSize)
    : HttpMessageDecoder(maxInitialLineLength, maxHeaderSize, maxChunkSize) {

}

HttpRequestDecoder::~HttpRequestDecoder() {

}

ChannelHandlerPtr HttpRequestDecoder::clone() {
    return ChannelHandlerPtr(
               new HttpRequestDecoder(maxInitialLineLength,
                                      maxHeaderSize,
                                      maxChunkSize));
}

std::string HttpRequestDecoder::toString() const {
    return "HttpRequestDecoder";
}

HttpMessagePtr HttpRequestDecoder::createMessage(const StringPiece& str1,
    const StringPiece& str2,
    const StringPiece& str3) {

    BOOST_ASSERT(!str1.empty() && !str2.empty() && !str3.empty()
                 && "createMessage parameters should not be NULL");

    if (!request) { //has not initial ready.
        request = new HttpRequest(HttpVersion::valueOf(str3),
                                  HttpMethod::valueOf(str1),
                                  str2);
    }
    else {
        request->clear();
        request->setProtocolVersion(HttpVersion::valueOf(str3));
        request->setMethod(HttpMethod::valueOf(str1));
        request->setUri(str2);
    }

    return request;
}

bool HttpRequestDecoder::isDecodingRequest() const {
    return true;
}

}
}
}
}
