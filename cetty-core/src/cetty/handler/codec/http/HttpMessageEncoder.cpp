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
#include <cetty/channel/Channels.h>
#include <cetty/channel/ChannelMessage.h>
#include <cetty/util/Exception.h>
#include <cetty/util/Integer.h>

#include <cetty/handler/codec/http/HttpHeader.h>
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

ChannelMessage HttpMessageEncoder::encode(ChannelHandlerContext& ctx,
        const ChannelPtr& channel,
        const ChannelMessage& msg) {
    HttpMessagePtr message = msg.smartPointer<HttpMessage>();

    if (!message) {
        message = msg.smartPointer<HttpMessage, HttpRequest>();

        if (!message) {
            message = msg.smartPointer<HttpMessage, HttpResponse>();
        }
    }

    if (message) {
        bool chunked = this->chunked =
                           HttpCodecUtil::isTransferEncodingChunked(*message);

        const ChannelBufferFactoryPtr& factory = channel->getConfig().getBufferFactory();
        BOOST_ASSERT(factory);
        ChannelBufferPtr header = ChannelBuffers::dynamicBuffer(*factory);

        encodeInitialLine(*header, *message);
        encodeHeaders(*header, message->getFirstHeader(), message->getLastHeader());
        header->writeByte(HttpCodecUtil::CR);
        header->writeByte(HttpCodecUtil::LF);

        ChannelBufferPtr content = message->getContent();

        if (!content->readable()) {
            return ChannelMessage(header); // no content
        }
        else if (chunked) {
            throw InvalidArgumentException(
                "HttpMessage.content must be empty if Transfer-Encoding is chunked.");
        }
        else {
            if (content->aheadWritableBytes() >= header->readableBytes()) {
                content->writeBytesAhead(header);
                return ChannelMessage(content);
            }
            else {
                header->writeBytes(content);
                return ChannelMessage(header);
            }
        }
    }

    HttpChunkPtr chunk = msg.smartPointer<HttpChunk>();

    if (!chunk) {
        // Unknown message type.
        return msg;
    }

    if (this->chunked) {
        if (chunk->isLast()) {
            this->chunked = false;
            HttpChunkTrailerPtr chunkTrailer =
                boost::dynamic_pointer_cast<HttpChunkTrailer>(chunk);

            if (chunkTrailer) {
                const ChannelBufferFactoryPtr& factory = channel->getConfig().getBufferFactory();
                BOOST_ASSERT(factory);
                ChannelBufferPtr trailer = ChannelBuffers::dynamicBuffer(1024, *factory);

                trailer->writeByte('0');
                trailer->writeByte(HttpCodecUtil::CR);
                trailer->writeByte(HttpCodecUtil::LF);

                encodeHeaders(*trailer,
                              chunkTrailer->getFirstHeader(),
                              chunkTrailer->getLastHeader());

                trailer->writeByte(HttpCodecUtil::CR);
                trailer->writeByte(HttpCodecUtil::LF);
                return ChannelMessage(trailer);
            }
            else {
                return ChannelMessage(LAST_CHUNK);
            }
        }
        else {
            ChannelBufferPtr content = chunk->getContent();
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

                return ChannelMessage(content);
            }
            else {
                ChannelBufferPtr buffer = ChannelBuffers::buffer(contentLength + lengthPartSize + 2);
                buffer->writeBytes(lengthStr);
                buffer->writeByte(HttpCodecUtil::CR);
                buffer->writeByte(HttpCodecUtil::LF);

                buffer->writeBytes(content);
                buffer->writeByte(HttpCodecUtil::CR);
                buffer->writeByte(HttpCodecUtil::LF);
                return ChannelMessage(buffer);
            }
        }
    }
    else { // if (this->chunked) {
        if (chunk->isLast()) {
            return ChannelMessage::EMPTY_MESSAGE;
        }
        else {
            return ChannelMessage(chunk->getContent());
        }
    }
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
