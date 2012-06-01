
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

#include <cetty/handler/codec/http/HttpResponseDecoder.h>
#include <cetty/handler/codec/http/HttpResponse.h>
#include <cetty/util/Integer.h>

namespace cetty { namespace handler { namespace codec { namespace http { 

    using namespace cetty::util;

HttpResponseDecoder::HttpResponseDecoder()
    : response(new HttpResponse),
    HttpMessageDecoder(MAX_INITIAL_LINE_LENGTH, MAX_HEADER_SIZE, MAX_CHUNK_SIZE) {
}

HttpResponseDecoder::HttpResponseDecoder(int maxInitialLineLength,
    int maxHeaderSize,
    int maxChunkSize)
    : response(new HttpResponse),
    HttpMessageDecoder(maxInitialLineLength,
    maxHeaderSize,
    maxChunkSize) {
}

HttpResponseDecoder::~HttpResponseDecoder() {

}

cetty::channel::ChannelHandlerPtr HttpResponseDecoder::clone() {
    return ChannelHandlerPtr(
        new HttpResponseDecoder(maxInitialLineLength,
        maxHeaderSize,
        maxChunkSize));
}

std::string HttpResponseDecoder::toString() const {
    return "HttpResponseDecoder";
}

cetty::handler::codec::http::HttpMessagePtr HttpResponseDecoder::createMessage(const char* str1, const char* str2, const char* str3) {
    response->clear();
    response->setProtocolVersion(HttpVersion::valueOf(str1));
    response->setStatus(HttpResponseStatus(Integer::parse(str2), str3));
    return response;
}

bool HttpResponseDecoder::isDecodingRequest() const {
    return false;
}

}}}}
