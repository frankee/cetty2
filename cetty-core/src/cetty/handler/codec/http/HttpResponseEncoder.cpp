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

#include <cetty/handler/codec/http/HttpResponseEncoder.h>

#include <cetty/buffer/ChannelBuffer.h>
#include <cetty/util/Integer.h>
#include <cetty/util/Exception.h>
#include <cetty/handler/codec/http/HttpMethod.h>
#include <cetty/handler/codec/http/HttpVersion.h>
#include <cetty/handler/codec/http/HttpResponse.h>
#include <cetty/handler/codec/http/HttpResponseStatus.h>
#include <cetty/handler/codec/http/HttpCodecUtil.h>

namespace cetty {
namespace handler {
namespace codec {
namespace http {

using namespace cetty::buffer;
using namespace cetty::util;

void HttpResponseEncoder::encodeInitialLine(ChannelBuffer& buf, const HttpMessage& message) {
    const HttpResponse* response =
        dynamic_cast<const HttpResponse*>(&message);

    if (NULL == response) {
        throw RuntimeException("message must be HttpResponse");
    }

    buf.writeBytes(response->getProtocolVersion().toString());
    buf.writeByte(HttpCodecUtil::SP);
    buf.writeBytes(Integer::toString(response->getStatus().getCode()));
    buf.writeByte(HttpCodecUtil::SP);
    buf.writeBytes(response->getStatus().getReasonPhrase());
    buf.writeByte(HttpCodecUtil::CR);
    buf.writeByte(HttpCodecUtil::LF);
}

}
}
}
}