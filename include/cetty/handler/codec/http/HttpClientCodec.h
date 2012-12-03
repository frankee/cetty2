#if !defined(CETTY_HANDLER_CODEC_HTTP_HTTPCLIENTCODEC_H)
#define CETTY_HANDLER_CODEC_HTTP_HTTPCLIENTCODEC_H

/*
 * Copyright 2010 Red Hat, Inc.
 *
 * Red Hat licenses this file to you under the Apache License, version 2.0
 * (the "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at:
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 */
/*
 * Copyright (c) 2010-2011 frankee zhou (frankee.zhou at gmail dot com)
 * Distributed under under the Apache License, version 2.0 (the "License").
 */

#include <deque>

#include <cetty/handler/codec/http/HttpRequestEncoder.h>
#include <cetty/handler/codec/http/HttpResponseDecoder.h>
#include <cetty/handler/codec/http/HttpRequest.h>
#include <cetty/handler/codec/http/HttpResponse.h>

#include <cetty/util/StringUtil.h>

namespace cetty {
namespace handler {
namespace codec {
namespace http {

using namespace cetty::util;

/**
 * A combination of {@link HttpRequestEncoder} and {@link HttpResponseDecoder}
 * which enables easier client side HTTP implementation. {@link HttpClientCodec}
 * provides additional state management for <tt>HEAD</tt> and <tt>CONNECT</tt>
 * requests, which {@link HttpResponseDecoder} lacks.  Please refer to
 * {@link HttpResponseDecoder} to learn what additional state management needs
 * to be done for <tt>HEAD</tt> and <tt>CONNECT</tt> and why
 * {@link HttpResponseDecoder} can not handle it by itself.
 *
 *
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 * @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
 *
 * @see HttpServerCodec
 *
 * @apiviz.has org.jboss.netty.handler.codec.http.HttpResponseDecoder
 * @apiviz.has org.jboss.netty.handler.codec.http.HttpRequestEncoder
 */

class HttpClientCodec {
public:
    /**
     * Creates a new instance with the default decoder options
     * (<tt>maxInitialLineLength (4096}</tt>, <tt>maxHeaderSize (8192)</tt>, and
     * <tt>maxChunkSize (8192)</tt>).
     */
    HttpClientCodec()
        : done(false),
          failOnMissingResponse(false),
          maxInitialLineLength(4096),
          maxHeaderSize(8192),
          maxChunkSize(8192) {
        init(new Decoder(maxInitialLineLength, maxHeaderSize, maxChunkSize, this),
             new Encoder(this));
    }

    /**
     * Creates a new instance with the specified decoder options.
     */
    HttpClientCodec(int maxInitialLineLength,
                    int maxHeaderSize,
                    int maxChunkSize)
        : done(false),
          failOnMissingResponse(false),
          maxInitialLineLength(maxInitialLineLength),
          maxHeaderSize(maxHeaderSize),
          maxChunkSize(maxChunkSize) {
        init(new Decoder(maxInitialLineLength, maxHeaderSize, maxChunkSize, this),
             new Encoder(this));
    }

    HttpClientCodec(int maxInitialLineLength,
                    int maxHeaderSize,
                    int maxChunkSize,
                    bool failOnMissingResponse)
        : done(false),
          failOnMissingResponse(failOnMissingResponse),
          maxInitialLineLength(maxInitialLineLength),
          maxHeaderSize(maxHeaderSize),
          maxChunkSize(maxChunkSize) {
        init(new Decoder(maxInitialLineLength, maxHeaderSize, maxChunkSize, this),
             new Encoder(this));
    }

    virtual ChannelHandlerPtr clone() {
        return new HttpClientCodec(maxInitialLineLength,
                                   maxHeaderSize,
                                   maxChunkSize);
    }

    virtual std::string toString() const { return "HttpClientCodec"; }

private:
    class Encoder : public HttpRequestEncoder {
    public:
        Encoder(HttpClientCodec* clientCodec) : codec(clientCodec) {}
        virtual ~Encoder() {}

        virtual ChannelHandlerPtr clone() {
            return ChannelHandlerPtr(new Encoder(codec));
        }
        virtual std::string toString() const {
            return "HttpClientCodec::Encoder";
        }

    protected:
        virtual ChannelBufferPtr encode(ChannelHandlerContext& ctx,
                                        const HttpPackage& msg,
                                        const ChannelBufferPtr& out) {
            if (msg.isHttpMessage() && !codec->done) {
                HttpRequestPtr request
                    = boost::dynamic_pointer_cast<HttpRequest>(msg.httpMessage());

                if (request) {
                    codec->queue.push_back(request->getMethod());
                }
                else {

                }
            }

            ChannelBufferPtr buffer =
                HttpRequestEncoder::encode(ctx, msg, out);

            if (codec->failOnMissingResponse) {
                // check if the request is chunked if so do not increment
                if (msg.isHttpMessage() /*HttpRequest*/
                        && msg.httpMessage()->getTransferEncoding().isSingle()) {
                    ++ codec->requestResponseCounter;
                }
                else if (msg.isHttpChunk() && msg.httpChunk()->isLast()) {
                    // increment as its the last chunk
                    ++ codec->requestResponseCounter;
                }
            }

            return buffer;
        }

    private:
        HttpClientCodec* codec;
    };

    class Decoder : public HttpResponseDecoder {
    public:
        Decoder(int maxInitialLineLength,
                int maxHeaderSize,
                int maxChunkSize,
                HttpClientCodec* clientCodec)
            : HttpResponseDecoder(maxInitialLineLength, maxHeaderSize, maxChunkSize),
              codec(clientCodec) {
        }
        virtual ~Decoder() {}

        virtual ChannelHandlerPtr clone() {
            return ChannelHandlerPtr(new Decoder(maxInitialLineLength,
                                                 maxHeaderSize,
                                                 maxChunkSize,
                                                 codec));
        }

        virtual std::string toString() const {
            return "HttpClientCodec::Decoder";
        }

        virtual void channelInactive(ChannelHandlerContext& ctx) {
            HttpResponseDecoder::channelInactive(ctx);

            if (codec->failOnMissingResponse) {
                int missingResponses = codec->requestResponseCounter;

                if (missingResponses > 0) {
                    ctx.fireExceptionCaught(
                        ChannelException(StringUtil::printf(
                                             "channel gone inactive with %d missing response(s)",
                                             missingResponses)));
                }
            }
        }

    protected:
        virtual HttpPackage decode(ChannelHandlerContext& ctx,
                                   const ReplayingDecoderBufferPtr& buffer,
                                   int state) {
            if (codec->done) {
                return HttpPackage()/*buffer->readBytes(actualReadableBytes())*/;
            }
            else {
                HttpPackage msg = HttpResponseDecoder::decode(ctx, buffer, state);

                if (codec->failOnMissingResponse) {
                    decrement(msg);
                }

                return msg;
            }
        }

        virtual bool isContentAlwaysEmpty(const HttpMessagePtr& msg) {
            HttpResponsePtr response =
                boost::dynamic_pointer_cast<HttpResponse>(msg);
            BOOST_ASSERT(response);

            int statusCode = response->getStatus().getCode();

            if (statusCode == 100) {
                // 100-continue response should be excluded from paired comparison.
                return true;
            }

            // Get the method of the HTTP request that corresponds to the
            // current response.
            HttpMethod method = codec->queue.front();
            codec->queue.pop_front();

            char firstChar = method.getName().at(0);

            switch (firstChar) {
            case 'H':

                // According to 4.3, RFC2616:
                // All responses to the HEAD request method MUST NOT include a
                // message-body, even though the presence of entity-header fields
                // might lead one to believe they do.
                if (HttpMethod::HEAD == method) {
                    return true;

                    // The following code was inserted to work around the servers
                    // that behave incorrectly.  It has been commented out
                    // because it does not work with well behaving servers.
                    // Please note, even if the 'Transfer-Encoding: chunked'
                    // header exists in the HEAD response, the response should
                    // have absolutely no content.
                    //
                    //// Interesting edge case:
                    //// Some poorly implemented servers will send a zero-byte
                    //// chunk if Transfer-Encoding of the response is 'chunked'.
                    ////
                    //// return !msg.isChunked();
                }

                break;

            case 'C':

                // Successful CONNECT request results in a response with empty body.
                if (statusCode == 200) {
                    if (HttpMethod::CONNECT == method) {
                        // Proxy connection established - Not HTTP anymore.
                        codec->done = true;
                        codec->queue.clear();
                        return true;
                    }
                }

                break;
            }

            return HttpResponseDecoder::isContentAlwaysEmpty(msg);
        }

    private:
        void decrement(const HttpPackage& msg) {
            if (!msg) {
                return;
            }

            // check if it's an HttpMessage and its transfer encoding is SINGLE.
            if (msg.isHttpMessage() && msg.httpMessage()->getTransferEncoding().isSingle()) {
                -- codec->requestResponseCounter;
            }
            else if (msg.isHttpChunk() && msg.httpChunk()->isLast()) {
                -- codec->requestResponseCounter;
            }

            //else if (msg instanceof Object[]) {
            // we just decrement it here as we only use this if the end of the chunk is reached
            // It would be more safe to check all the objects in the array but would also be slower
            //    requestResponseCounter.decrementAndGet();
            //}
        }

    private:
        HttpClientCodec* codec;
    };

    friend class Encoder;
    friend class Decoder;

private:
    /** If true, decoding stops (i.e. pass-through) */
    bool done;

    bool failOnMissingResponse;

    int requestResponseCounter;

    int maxInitialLineLength;
    int maxHeaderSize;
    int maxChunkSize;

    /** A queue that is used for correlating a request and a response. */
    std::deque<HttpMethod> queue;
};


}
}
}
}

#endif //#if !defined(CETTY_HANDLER_CODEC_HTTP_HTTPCLIENTCODEC_H)

// Local Variables:
// mode: c++
// End:
