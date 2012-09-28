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

#include <cetty/util/StringUtil.h>

using namespace cetty::buffer;

static ChannelBufferPtr prepareDataChunk(int size) {
    std::string str(size, 'a');
    return Unpooled::copiedBuffer(str);
}

TEST(HttpServerCodecTest, testUnfinishedChunkedHttpRequestIsLastFlag) {
    int maxChunkSize = 2000;
    HttpServerCodec httpServerCodec = new HttpServerCodec(1000, 1000, maxChunkSize);
    EmbeddedByteChannel decoderEmbedder = new EmbeddedByteChannel(httpServerCodec);

    int totalContentLength = maxChunkSize * 5;

    std::string input;
    StringUtil::vstrprintf(&input, "PUT /test HTTP/1.1\r\nContent-Length: %d\r\n\r\n", totalContentLength);
    decoderEmbedder.writeInbound(Unpooled::wrappedBuffer(&input));

    int offeredContentLength = (int)(maxChunkSize * 2.5);
    decoderEmbedder.writeInbound(prepareDataChunk(offeredContentLength));
    decoderEmbedder.finish();

    HttpMessage httpMessage = (HttpMessage) decoderEmbedder.readInbound();
    Assert.assertSame(HttpTransferEncoding.STREAMED, httpMessage.getTransferEncoding());

    boolean empty = true;
    int totalBytesPolled = 0;

    for (;;) {
        HttpChunk httpChunk = (HttpChunk) decoderEmbedder.readInbound();

        if (httpChunk == null) {
            break;
        }

        empty = false;
        totalBytesPolled += httpChunk.getContent().readableBytes();
        Assert.assertFalse(httpChunk.isLast());
    }

    Assert.assertFalse(empty);
    Assert.assertEquals(offeredContentLength, totalBytesPolled);
}


