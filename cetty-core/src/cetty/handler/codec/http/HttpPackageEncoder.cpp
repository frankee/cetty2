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

#include <cetty/handler/codec/http/HttpPackageEncoder.h>

#include <cetty/buffer/Unpooled.h>

#include <cetty/channel/Channel.h>
#include <cetty/channel/ChannelConfig.h>
#include <cetty/channel/ChannelHandlerContext.h>
#include <cetty/util/Exception.h>
#include <cetty/util/StringUtil.h>

#include <cetty/handler/codec/http/HttpRequest.h>
#include <cetty/handler/codec/http/HttpResponse.h>
#include <cetty/handler/codec/http/HttpPackage.h>
#include <cetty/handler/codec/http/HttpChunk.h>
#include <cetty/handler/codec/http/HttpChunkTrailer.h>
#include <cetty/handler/codec/http/HttpCodecUtil.h>


namespace cetty {
namespace handler {
namespace codec {
namespace http {

using namespace cetty::channel;
using namespace cetty::buffer;
using namespace cetty::util;

HttpPackageEncoder::HttpPackageEncoder()
    : lastTE_(HttpTransferEncoding::SINGLE) {
}

HttpPackageEncoder::~HttpPackageEncoder() {
}

ChannelBufferPtr HttpPackageEncoder::encode(ChannelHandlerContext& ctx,
        const HttpPackage& msg,
        const ChannelBufferPtr& out) {
    if (msg.isHttpRequest() || msg.isHttpResponse()) {
        HttpHeaders& headers = msg.headers();
        HttpTransferEncoding te = headers.transferEncoding();
        lastTE_ = te;

        // Calling setTransferEncoding() will sanitize the headers and the content.
        // For example, it will remove the cases such as 'Transfer-Encoding' and 'Content-Length'
        // coexist.  It also removes the content if the transferEncoding is not SINGLE.
        headers.setTransferEncoding(te);

        // Encode the message.
        ChannelBufferPtr header = Unpooled::buffer();

        initialLineEncoder_(msg, header);
        encodeHeaders(*header, headers.firstHeader(), headers.lastHeader());
        header->writeByte(HttpCodecUtil::CR);
        header->writeByte(HttpCodecUtil::LF);

        ChannelBufferPtr content /*= value->getContent()*/;

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
    else if (msg.isHttpChunk()) {
        HttpTransferEncoding te = lastTE_;
        HttpChunkPtr chunk = msg.httpChunk();

        if (te == HttpTransferEncoding::SINGLE) {
            //throw IllegalArgumentException(
            //    "The transfer encoding of the last encoded HttpMessage is SINGLE.");
        }
        else if (te == HttpTransferEncoding::STREAMED) {
            //ByteBuf content = chunk.getContent();
            //out.writeBytes(content, content.readerIndex(), content.readableBytes());
        }
        else if (te == HttpTransferEncoding::CHUNKED) {
            if (chunk->isLast()) {
                //encoder.chunked = false;
                return Unpooled::EMPTY_BUFFER;
            }

            ChannelBufferPtr content = chunk->getContent();
            int contentLength = content->readableBytes();
            std::string lengthStr = StringUtil::hextostr(contentLength);
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

        //FIXME
        return Unpooled::EMPTY_BUFFER;
    }

    else if (msg.isHttpChunkTrailer()) {
        HttpChunkTrailerPtr chunkTrailer = msg.httpChunkTrailer();

        if (chunkTrailer->isLast()) {
            //encoder.chunked = false;

            if (chunkTrailer) {
                ChannelBufferPtr trailer = Unpooled::buffer(1024);

                trailer->writeByte('0');
                trailer->writeByte(HttpCodecUtil::CR);
                trailer->writeByte(HttpCodecUtil::LF);

                encodeHeaders(*trailer,
                              chunkTrailer->headers().firstHeader(),
                              chunkTrailer->headers().lastHeader());

                trailer->writeByte(HttpCodecUtil::CR);
                trailer->writeByte(HttpCodecUtil::LF);
                return trailer;
            }
        }

        return ChannelBufferPtr();
    }
}

void HttpPackageEncoder::encodeHeaders(ChannelBuffer& buf,
    const HttpHeaders::ConstHeaderIterator& begin,
    const HttpHeaders::ConstHeaderIterator& end) {

    for (HttpHeaders::ConstHeaderIterator itr = begin; itr != end; ++itr) {
        encodeHeader(buf, itr->first, itr->second);
    }
}

void HttpPackageEncoder::encodeHeader(ChannelBuffer& buf,
                                      const std::string& header,
                                      const std::string& value) {
    buf.writeBytes(header);
    buf.writeByte(HttpCodecUtil::COLON);
    buf.writeByte(HttpCodecUtil::SP);
    buf.writeBytes(value);
    buf.writeByte(HttpCodecUtil::CR);
    buf.writeByte(HttpCodecUtil::LF);
}

bool HttpPackageEncoder::encodeResponseInitialLine(const HttpPackage& package, const ChannelBufferPtr& buf) {
    HttpResponsePtr response = package.httpResponse();

    if (!response) {
        LOG_ERROR << "message must be HttpResponse";
        return false;
    }

    buf->writeBytes(response->version().toString());
    buf->writeByte(HttpCodecUtil::SP);
    buf->writeBytes(StringUtil::numtostr(response->status().getCode()));
    buf->writeByte(HttpCodecUtil::SP);
    buf->writeBytes(response->status().getReasonPhrase());
    buf->writeByte(HttpCodecUtil::CR);
    buf->writeByte(HttpCodecUtil::LF);

    return true;
}

bool HttpPackageEncoder::encodeRequestInitialLine(const HttpPackage& package, const ChannelBufferPtr& buf) {
    HttpRequestPtr request = package.httpRequest();

    if (!request) {
        LOG_ERROR << "message must be HttpRequest";
        return false;
    }

    buf->writeBytes(request->method().toString());
    buf->writeByte(HttpCodecUtil::SP);
    buf->writeBytes(request->getUriString());
    buf->writeByte(HttpCodecUtil::SP);
    buf->writeBytes(request->version().toString());
    buf->writeByte(HttpCodecUtil::CR);
    buf->writeByte(HttpCodecUtil::LF);

    return true;
}

static std::string lastStr("0\r\n\r\n");

}
}
}
}
