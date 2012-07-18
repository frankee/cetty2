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

#include <cetty/handler/codec/http/HttpRequestEncoder.h>

#include <cetty/buffer/ChannelBuffer.h>
#include <cetty/util/Exception.h>
#include <cetty/handler/codec/http/HttpMethod.h>
#include <cetty/handler/codec/http/HttpVersion.h>
#include <cetty/handler/codec/http/HttpRequest.h>
#include <cetty/handler/codec/http/HttpCodecUtil.h>

namespace cetty {
namespace handler {
namespace codec {
namespace http {

using namespace cetty::buffer;
using namespace cetty::util;

void HttpRequestEncoder::encodeInitialLine(ChannelBuffer& buf, const HttpMessage& message) {
    const HttpRequest* request =
        dynamic_cast<const HttpRequest*>(&message);

    if (NULL == request) {
        throw RuntimeException("message must be HttpRequest");
    }

    buf.writeBytes(request->getMethod().toString());
    buf.writeByte(HttpCodecUtil::SP);
    buf.writeBytes(request->getUri());
    buf.writeByte(HttpCodecUtil::SP);
    buf.writeBytes(request->getProtocolVersion().toString());
    buf.writeByte(HttpCodecUtil::CR);
    buf.writeByte(HttpCodecUtil::LF);
}

}
}
}
}
