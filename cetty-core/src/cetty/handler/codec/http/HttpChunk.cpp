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

#include <cetty/handler/codec/http/HttpChunk.h>

#include <cetty/buffer/ChannelBuffer.h>
#include <cetty/buffer/ChannelBuffers.h>
#include <cetty/util/Exception.h>

namespace cetty {
namespace handler {
namespace codec {
namespace http {

using namespace cetty::buffer;
using namespace cetty::util;

HttpChunkPtr HttpChunk::LAST_CHUNK(new HttpChunk(ChannelBuffers::EMPTY_BUFFER));

HttpChunk::HttpChunk(const ChannelBufferPtr& content) {
    if (content) {
        setContent(content);
    }
    else {
        setContent(ChannelBuffers::EMPTY_BUFFER);
    }
}

HttpChunk::~HttpChunk() {
}

bool HttpChunk::isLast() const {
    return !content->readable();
}

std::string HttpChunk::toString() const {
    return "HttpChunk";
}

const ChannelBufferPtr& HttpChunk::getContent() const {
    return content;
}

void HttpChunk::setContent(const ChannelBufferPtr& content) {
    if (content) {
        this->content = content;
    }
}

bool HttpChunk::followingLastChunk() const {
    return true;
}

void HttpChunk::setFollowLastChunk(bool followLast) {

}

}
}
}
}
