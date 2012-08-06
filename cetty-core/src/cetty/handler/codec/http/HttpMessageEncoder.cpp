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

#include <cetty/buffer/Array.h>
#include <cetty/buffer/ChannelBuffers.h>
#include <cetty/buffer/ChannelBufferFactory.h>
#include <cetty/channel/ChannelConfig.h>
#include <cetty/channel/Channel.h>
#include <cetty/util/Exception.h>
#include <cetty/util/Integer.h>

#include <cetty/handler/codec/http/HttpMessage.h>
#include <cetty/handler/codec/http/HttpRequest.h>
#include <cetty/handler/codec/http/HttpResponse.h>
#include <cetty/handler/codec/http/HttpChunk.h>
#include <cetty/handler/codec/http/HttpChunkTrailer.h>
#include <cetty/handler/codec/http/HttpCodecUtil.h>
#include <cetty/handler/codec/http/HttpMessageEncoder.h>

namespace cetty {
namespace handler {
namespace codec {
namespace http {

using namespace cetty::channel;
using namespace cetty::buffer;
using namespace cetty::util;

HttpMessageEncoder::~HttpMessageEncoder() {
}

HttpMessageEncoder::HttpMessageEncoder() {
}

class HttpPackageEncodeVisitor : public boost::static_visitor<ChannelBufferPtr> {
public:
    HttpPackageEncodeVisitor(HttpMessageEncoder& encoder,
                             ChannelHandlerContext& ctx,
                             const ChannelBufferPtr& out)
        : encoder(encoder), ctx(ctx), out(out) {

    }

    ChannelBufferPtr operator()(const HttpMessagePtr& value) const {
        bool chunked = encoder.chunked =
                           HttpCodecUtil::isTransferEncodingChunked(*value);

        ChannelBufferPtr header = ChannelBuffers::dynamicBuffer();

        encoder.encodeInitialLine(*header, *value);
        encoder.encodeHeaders(*header, value->getFirstHeader(), value->getLastHeader());
        header->writeByte(HttpCodecUtil::CR);
        header->writeByte(HttpCodecUtil::LF);

        ChannelBufferPtr content = value->getContent();

        if (!content->readable()) {
            return header; // no content
        }
        else if (chunked) {
            throw InvalidArgumentException(
                "HttpMessage.content must be empty if Transfer-Encoding is chunked.");
        }
        else {
            if (content->aheadWritableBytes() >= header->readableBytes()) {
                content->writeBytesAhead(header);
                return content;
            }
            else {
                header->writeBytes(content);
                return header;
            }
        }
    }

    ChannelBufferPtr operator()(const HttpChunkPtr& value) const {
        if (encoder.chunked) {
            ChannelBufferPtr content = value->getContent();
            int contentLength = content->readableBytes();
            std::string lengthStr = Integer::toHexString(contentLength);
            int lengthPartSize = lengthStr.size() + 2;

            if (content->writableBytes() >= 2 && content->aheadWritableBytes() >= lengthPartSize) {
                content->writeByte(HttpCodecUtil::CR);
                content->writeByte(HttpCodecUtil::LF);

                // prepends the length part
                content->writeByteAhead(HttpCodecUtil::LF);
                content->writeByteAhead(HttpCodecUtil::CR);
                content->writeBytesAhead(lengthStr);

                return content;
            }
            else {
                ChannelBufferPtr buffer = ChannelBuffers::buffer(contentLength + lengthPartSize + 2);
                buffer->writeBytes(lengthStr);
                buffer->writeByte(HttpCodecUtil::CR);
                buffer->writeByte(HttpCodecUtil::LF);

                buffer->writeBytes(content);
                buffer->writeByte(HttpCodecUtil::CR);
                buffer->writeByte(HttpCodecUtil::LF);
                return buffer;
            }

            if (value->isLast()) {
                encoder.chunked = false;
                return ChannelBuffers::EMPTY_BUFFER;
            }
        }
        else { // if (this->chunked) {
            if (value->isLast()) {
                return ChannelBufferPtr();
            }
            else {
                return value->getContent();
            }
        }
    }

    ChannelBufferPtr operator()(const HttpChunkTrailerPtr& value) const {
        if (encoder.chunked) {
            encoder.chunked = false;

            if (value) {
                ChannelBufferPtr trailer = ChannelBuffers::dynamicBuffer(1024);

                trailer->writeByte('0');
                trailer->writeByte(HttpCodecUtil::CR);
                trailer->writeByte(HttpCodecUtil::LF);

                encoder.encodeHeaders(*trailer,
                                      value->getFirstHeader(),
                                      value->getLastHeader());

                trailer->writeByte(HttpCodecUtil::CR);
                trailer->writeByte(HttpCodecUtil::LF);
                return trailer;
            }
        }

        return ChannelBufferPtr();
    }

    template<typename U>
    ChannelBufferPtr operator()(const U& value) const {
        return ChannelBufferPtr();
    }

private:
    HttpMessageEncoder& encoder;
    ChannelHandlerContext& ctx;
    const ChannelBufferPtr& out;
};

ChannelBufferPtr HttpMessageEncoder::encode(ChannelHandlerContext& ctx,
        const HttpPackage& msg,
        const ChannelBufferPtr& out) {

    HttpPackageEncodeVisitor visitor(*this, ctx, out);
    return msg.apply_visitor(visitor);
}

void HttpMessageEncoder::encodeHeaders(ChannelBuffer& buf,
                                       const HttpMessage::ConstHeaderIterator& begin,
                                       const HttpMessage::ConstHeaderIterator& end) {

    for (HttpMessage::ConstHeaderIterator itr = begin; itr != end; ++itr) {
        encodeHeader(buf, itr->first, itr->second);
    }
}

void HttpMessageEncoder::encodeHeader(ChannelBuffer& buf,
                                      const std::string& header,
                                      const std::string& value) {
    buf.writeBytes(header);
    buf.writeByte(HttpCodecUtil::COLON);
    buf.writeByte(HttpCodecUtil::SP);
    buf.writeBytes(value);
    buf.writeByte(HttpCodecUtil::CR);
    buf.writeByte(HttpCodecUtil::LF);
}

static std::string lastStr("0\r\n\r\n");
ChannelBufferPtr HttpMessageEncoder::LAST_CHUNK = ChannelBuffers::wrappedBuffer(lastStr);

}
}
}
}
