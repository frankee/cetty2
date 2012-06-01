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

#include <cetty/handler/codec/http/HttpChunkTrailer.h>

#include <boost/assert.hpp>
#include <boost/algorithm/string/predicate.hpp>

#include <cetty/handler/codec/http/HttpHeaders.h>
#include <cetty/handler/codec/http/HttpCodecUtil.h>
#include <cetty/buffer/ChannelBuffers.h>
#include <cetty/util/Exception.h>

namespace cetty {
namespace handler {
namespace codec {
namespace http {

using namespace cetty::buffer;
using namespace cetty::util;

static bool validateHeaderName(const std::string& name) {
    if (!HttpCodecUtil::validateHeaderName(name)) {
        return false;
    }

    if (boost::algorithm::iequals(name, HttpHeaders::Names::CONTENT_LENGTH) ||
        boost::algorithm::iequals(name, HttpHeaders::Names::TRANSFER_ENCODING) ||
        boost::algorithm::iequals(name, HttpHeaders::Names::TRAILER)) {
            return false;
            throw InvalidArgumentException(
                std::string("prohibited trailing header: ") + name);
    }

    return true;
}

HttpChunkTrailer::HttpChunkTrailer()
    : HttpChunk(ChannelBuffers::EMPTY_BUFFER) {
    httpHeader.setValidateNameFunctor(validateHeaderName);
}

HttpChunkTrailer::~HttpChunkTrailer() {
}

void HttpChunkTrailer::setContent(const ChannelBufferPtr& content) {
    //throw IllegalStateException("read-only");
}

std::string HttpChunkTrailer::toString() const {
    return "HttpChunkTrailer";
}

bool HttpChunkTrailer::isLast() const {
    return true;
}

}
}
}
}
