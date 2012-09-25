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

#include <cetty/buffer/Unpooled.h>
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

HttpMessageEncoder::HttpMessageEncoder()
    : lastTE(HttpTransferEncoding::SINGLE) {
}

class HttpPackageEncodeVisitor : public boost::static_visitor<ChannelBufferPtr> {
public:
    HttpPackageEncodeVisitor(HttpMessageEncoder& encoder,
                             ChannelHandlerContext& ctx,
                             const ChannelBufferPtr& out)
        : encoder(encoder), ctx(ctx), out(out) {

    }

    ChannelBufferPtr operator()(const HttpMessagePtr& value) const {
        HttpTransferEncoding te = value->getTransferEncoding();
        encoder.lastTE = te;

        // Calling setTransferEncoding() will sanitize the headers and the content.
        // For example, it will remove the cases such as 'Transfer-Encoding' and 'Content-Length'
        // coexist.  It also removes the content if the transferEncoding is not SINGLE.
        value->setTransferEncoding(te);

        // Encode the message.
        ChannelBufferPtr header = Unpooled::buffer();

        encoder.encodeInitialLine(*header, *value);
        encoder.encodeHeaders(*header, value->getFirstHeader(), value->getLastHeader());
        header->writeByte(HttpCodecUtil::CR);
        header->writeByte(HttpCodecUtil::LF);

        ChannelBufferPtr content = value->getContent();

        if (!content->readable()) {
            return header; // no content
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
        HttpTransferEncoding te = encoder.lastTE;

        if (te == HttpTransferEncoding::SINGLE) {
            //throw IllegalArgumentException(
            //    "The transfer encoding of the last encoded HttpMessage is SINGLE.");
        }
        else if (te == HttpTransferEncoding::STREAMED) {
            //ByteBuf content = chunk.getContent();
            //out.writeBytes(content, content.readerIndex(), content.readableBytes());
        }
        else if (te == HttpTransferEncoding::CHUNKED) {
            if (value->isLast()) {
                //encoder.chunked = false;
                return Unpooled::EMPTY_BUFFER;
            }

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
                ChannelBufferPtr buffer = Unpooled::buffer(contentLength + lengthPartSize + 2);
                buffer->writeBytes(lengthStr);
                buffer->writeByte(HttpCodecUtil::CR);
                buffer->writeByte(HttpCodecUtil::LF);

                buffer->writeBytes(content);
                buffer->writeByte(HttpCodecUtil::CR);
                buffer->writeByte(HttpCodecUtil::LF);
                return buffer;
            }
        }
        else {
            // error.
        }
    }

    ChannelBufferPtr operator()(const HttpChunkTrailerPtr& value) const {
        if (value->isLast()) {
            //encoder.chunked = false;

            if (value) {
                ChannelBufferPtr trailer = Unpooled::buffer(1024);

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
ChannelBufferPtr HttpMessageEncoder::LAST_CHUNK = Unpooled::wrappedBuffer(&lastStr);

}
}
}
}
