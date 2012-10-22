/*
 * Copyright 2012 The Netty Project
 *
 * The Netty Project licenses this file to you under the Apache License,
 * version 2.0 (the "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at:
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations
 * under the License.
 */

#include <gtest/gtest.h>
#include <cetty/buffer/Unpooled.h>
#include <cetty/buffer/ChannelBuffer.h>

#include <cetty/channel/embedded/EmbeddedBufferChannel.h>
#include <cetty/handler/codec/http/HttpServerCodec.h>
#include <cetty/handler/codec/http/HttpChunkAggregator.h>

#include <cetty/util/StringUtil.h>

using namespace cetty::buffer;
using namespace cetty::channel::embedded;
using namespace cetty::handler::codec::http;

static ChannelBufferPtr prepareDataChunk(int size) {
    std::string str(size, 'a');
    return Unpooled::copiedBuffer(str);
}

TEST(HttpServerCodecTest, testMultiChannelBufferInput) {
    EmbeddedBufferChannel<HttpMessagePtr> decoderEmbedder(
        new HttpRequestDecoder(),
        new HttpChunkAggregator(1048576));

    std::string inputPart1 = "GET /api/echo.json?p=ssss HTTP/1.1\r\n"
                        "Host: 127.0.0.1:8080\r\n"
                        "User-Agent: Mozilla/5.0 (Windows NT 6.1; WOW64; rv:15.0) Gecko/20100101 Firefox/15.0.1\r\n";

    std::string inputPart2 = "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n"
                        "Accept-Language: en-us,en;q=0.5\r\n"
                        "Accept-Encoding: gzip, deflate\r\n"
                        "Connection: keep-alive\r\n\r\n";

    decoderEmbedder.writeInbound(Unpooled::wrappedBuffer(&inputPart1));
    decoderEmbedder.writeInbound(Unpooled::wrappedBuffer(&inputPart2));
    decoderEmbedder.finish();

    HttpMessagePtr message;
    decoderEmbedder.readInbound(&message);
}

TEST(HttpServerCodecTest, testUnfinishedChunkedHttpRequestIsLastFlag) {
    int maxChunkSize = 2000;

    EmbeddedBufferChannel<HttpPackage> decoderEmbedder(
        new HttpServerCodec(1000, 1000, maxChunkSize));

    int totalContentLength = maxChunkSize * 5;

    std::string input;
    StringUtil::strprintf(&input, "PUT /test HTTP/1.1\r\nContent-Length: %d\r\n\r\n", totalContentLength);
    decoderEmbedder.writeInbound(Unpooled::wrappedBuffer(&input));

    int offeredContentLength = (int)(maxChunkSize * 2.5);
    decoderEmbedder.writeInbound(prepareDataChunk(offeredContentLength));
    decoderEmbedder.finish();

    HttpPackage message;
    decoderEmbedder.readInbound(&message);
    //Assert.assertSame(HttpTransferEncoding.STREAMED, httpMessage.getTransferEncoding());

    bool empty = true;
    int totalBytesPolled = 0;

    for (;;) {
        HttpPackage httpChunk;
        decoderEmbedder.readInbound(&httpChunk);

        if (!httpChunk) {
            break;
        }

        empty = false;
        //totalBytesPolled += httpChunk.getContent().readableBytes();
        //Assert.assertFalse(httpChunk.isLast());
    }

    ASSERT_FALSE(empty);
    ASSERT_EQ(offeredContentLength, totalBytesPolled);
};
