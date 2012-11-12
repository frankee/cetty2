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

#include <cetty/handler/codec/http/HttpChunkAggregator.h>

#include <cetty/buffer/Unpooled.h>

#include <cetty/channel/Channel.h>
#include <cetty/channel/ChannelConfig.h>
#include <cetty/channel/ChannelHandlerContext.h>
#include <cetty/util/Exception.h>
#include <cetty/util/StringUtil.h>

#include <cetty/handler/codec/TooLongFrameException.h>
#include <cetty/handler/codec/http/HttpMessage.h>
#include <cetty/handler/codec/http/HttpChunk.h>
#include <cetty/handler/codec/http/HttpChunkTrailer.h>
#include <cetty/handler/codec/http/HttpHeaders.h>

namespace cetty {
namespace handler {
namespace codec {
namespace http {

using namespace cetty::channel;
using namespace cetty::buffer;
using namespace cetty::util;
using namespace cetty::handler::codec;

class HttpPackageVisitor : public boost::static_visitor<HttpMessagePtr> {
public:
    HttpPackageVisitor(HttpChunkAggregator& aggregator,
                       ChannelHandlerContext& ctx)
        : aggregator(aggregator), ctx(ctx) {
    }

    HttpMessagePtr operator()(const HttpMessagePtr& value) const {
        HttpMessagePtr& currentMessage = aggregator.currentMessage;

        BOOST_ASSERT(value);

        // Handle the 'Expect: 100-continue' header if necessary.
        // TODO: Respond with 413 Request Entity Too Large
        //   and discard the traffic or close the connection.
        //       No need to notify the upstream handlers - just log.
        //       If decoding a response, just throw an exception.
        if (HttpHeaders::is100ContinueExpected(*value)) {
            //Channels::write(ctx, Channels::succeededFuture(ctx.getChannel()), CONTINUE.duplicate());
        }

        HttpTransferEncoding te = value->getTransferEncoding();
        if (te == HttpTransferEncoding::SINGLE) {
            currentMessage.reset();
            return value;
        }
        else if (te == HttpTransferEncoding::STREAMED
            || te == HttpTransferEncoding::CHUNKED) {
            // initialize the cumulative buffer, and wait for incoming chunks.
            value->setTransferEncoding(HttpTransferEncoding::SINGLE);
            value->setContent(Unpooled::buffer());

            currentMessage.reset();
            currentMessage = value;
            return HttpMessagePtr();
        }
        else {
            // Error.
            return HttpMessagePtr();
        }
    }

    HttpMessagePtr operator()(const HttpChunkPtr& value) const {
        HttpMessagePtr& currentMessage = aggregator.currentMessage;

        // Sanity check
        if (!currentMessage) {
            throw IllegalStateException("received HttpChunk  without HttpMessage");
        }

        // Merge the received chunk into the content of the current message.
        ChannelBufferPtr content = currentMessage->getContent();

        if (content->readableBytes() > aggregator.maxContentLength - value->getContent()->readableBytes()) {
            // TODO: Respond with 413 Request Entity Too Large
            //   and discard the traffic or close the connection.
            //       No need to notify the upstream handlers - just log.
            //       If decoding a response, just throw an exception.
            std::string msg;
            StringUtil::printf(&msg,
                "HTTP content length exceeded %d bytes.",
                aggregator.maxContentLength);

            LOG_ERROR << msg;

            throw TooLongFrameException(msg);
        }

        // Append the content of the chunk
        aggregator.appendToCumulation(value->getContent());

        if (value->isLast()) {
            currentMessage.reset();

            // Set the 'Content-Length' header.
            currentMessage->setHeader(
                HttpHeaders::Names::CONTENT_LENGTH,
                StringUtil::numtostr(content->readableBytes()));

            // All done - generate the event.
            return currentMessage;
        }
        else {
            return HttpMessagePtr();
        }
    }

    HttpMessagePtr operator()(const HttpChunkTrailerPtr& value) const {
        HttpMessagePtr& currentMessage = aggregator.currentMessage;

        HttpChunkTrailer::ConstHeaderIterator itr = value->getFirstHeader();
        HttpChunkTrailer::ConstHeaderIterator end = value->getLastHeader();

        for (; itr != end; ++itr) {
            currentMessage->setHeader(itr->first, itr->second);
        }

        // Set the 'Content-Length' header.
        currentMessage->setHeader(
            HttpHeaders::Names::CONTENT_LENGTH,
            StringUtil::numtostr(currentMessage->getContent()->readableBytes()));

        return currentMessage;
    }

    template<typename U>
    ChannelBufferPtr operator()(const U& value) const {
        // Neither HttpMessage or HttpChunk
        throw IllegalStateException(
            "Only HttpMessage and HttpChunk are accepted.");

        return HttpMessagePtr();
    }

private:
    HttpChunkAggregator& aggregator;
    ChannelHandlerContext& ctx;
};

HttpChunkAggregator::HttpChunkAggregator(int maxContentLength)
    : maxContentLength(maxContentLength) {
    if (maxContentLength <= 0) {
        throw InvalidArgumentException(
            "maxContentLength must be a positive integer: " +
            maxContentLength);
    }
}

HttpMessagePtr HttpChunkAggregator::decode(ChannelHandlerContext& ctx,
        const HttpPackage& msg) {
    HttpPackageVisitor visitor(*this, ctx);
    return msg.variant.apply_visitor(visitor);
}

void HttpChunkAggregator::appendToCumulation(const ChannelBufferPtr& input) {
    const ChannelBufferPtr& cumulation = currentMessage->getContent();
#if 0

    if (cumulation instanceof CompositeByteBuf) {
        // Make sure the resulting cumulation buffer has no more than 4 components.
        CompositeByteBuf composite = (CompositeByteBuf) cumulation;

        if (composite.numComponents() >= maxCumulationBufferComponents) {
            currentMessage.setContent(Unpooled.wrappedBuffer(composite.copy(), input));
        }
        else {
            List<ByteBuf> decomposed = composite.decompose(0, composite.readableBytes());
            ByteBuf[] buffers = decomposed.toArray(new ByteBuf[decomposed.size() + 1]);
            buffers[buffers.length - 1] = input;

            currentMessage.setContent(Unpooled.wrappedBuffer(buffers));
        }
    }
    else {
        currentMessage.setContent(Unpooled.wrappedBuffer(cumulation, input));
    }

#endif
    currentMessage->setContent(Unpooled::wrappedBuffer(cumulation, input));
}

ChannelHandlerPtr HttpChunkAggregator::clone() {
    return new HttpChunkAggregator(maxContentLength);
}

std::string HttpChunkAggregator::toString() const {
    return "HttpChunkAggregator";
}

}
}
}
}
