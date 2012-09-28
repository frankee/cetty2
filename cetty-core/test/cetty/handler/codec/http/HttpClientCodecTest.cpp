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
#include <cetty/channel/embedded/EmbeddedBufferChannel.h>

static const std::string RESPONSE =
    "HTTP/1.0 200 OK\r\n"
    "Date: Fri, 31 Dec 1999 23:59:59 GMT\r\n"
    "Content-Type: text/html\r\n"
    "Content-Length: 28\r\n"
    "\r\n"
    "<html><body></body></html>\r\n";

static const std::string INCOMPLETE_CHUNKED_RESPONSE =
    "HTTP/1.1 200 OK\r\n"
    "Content-Type: text/plain\r\n"
    "Transfer-Encoding: chunked\r\n"
    "\r\n"
    "5\r\n"
    "first\r\n"
    "6\r\n"
    "second\r\n"
    "0\r\n";

static const std::string CHUNKED_RESPONSE = INCOMPLETE_CHUNKED_RESPONSE + "\r\n";

TEST(HttpClientCodecTest, testFailsNotOnRequestResponse) {
    HttpClientCodec codec = new HttpClientCodec(4096, 8192, 8192, true);
    EmbeddedBufferChannel ch(codec);

    ch.writeOutbound(new DefaultHttpRequest(HttpVersion::HTTP_1_1,
        HttpMethod::GET,
        "http://localhost/"));
    ch.writeInbound(Unpooled::copiedBuffer(RESPONSE));
    ch.finish();
}

TEST(HttpClientCodecTest, testFailsNotOnRequestResponseChunked) {
    HttpClientCodec codec = new HttpClientCodec(4096, 8192, 8192, true);
    EmbeddedByteChannel ch = new EmbeddedByteChannel(codec);

    ch.writeOutbound(new DefaultHttpRequest(HttpVersion.HTTP_1_1, HttpMethod.GET, "http://localhost/"));
    ch.writeInbound(Unpooled.copiedBuffer(CHUNKED_RESPONSE));
    ch.finish();
}

TEST(HttpClientCodecTest, testFailsOnMissingResponse) {
    HttpClientCodec codec = new HttpClientCodec(4096, 8192, 8192, true);
    EmbeddedByteChannel ch = new EmbeddedByteChannel(codec);

    assertTrue(ch.writeOutbound(new DefaultHttpRequest(HttpVersion.HTTP_1_1, HttpMethod.GET, "http://localhost/")));
    assertNotNull(ch.readOutbound());

    try {
        ch.finish();
        fail();
    }
    catch (CodecException e) {
        assertTrue(e instanceof PrematureChannelClosureException);
    }

}

TEST(HttpClientCodecTest, testFailsOnIncompleteChunkedResponse) {
    HttpClientCodec codec = new HttpClientCodec(4096, 8192, 8192, true);
    EmbeddedByteChannel ch = new EmbeddedByteChannel(codec);

    ch.writeOutbound(new DefaultHttpRequest(HttpVersion.HTTP_1_1, HttpMethod.GET, "http://localhost/"));
    ch.writeInbound(Unpooled.copiedBuffer(INCOMPLETE_CHUNKED_RESPONSE, CharsetUtil.ISO_8859_1));

    try {
        ch.finish();
        fail();
    }
    catch (CodecException e) {
        assertTrue(e instanceof PrematureChannelClosureException);
    }

}
