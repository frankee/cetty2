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

#include <cetty/channel/ChannelUpstreamHandler.h>
#include <cetty/channel/ChannelDownstreamHandler.h>
#include <cetty/handler/codec/http/HttpRequestEncoder.h>
#include <cetty/handler/codec/http/HttpResponseDecoder.h>

namespace cetty {
namespace handler {
namespace codec {
namespace http {

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

class HttpClientCodec : public cetty::channel::ChannelUpstreamHandler,
    public cetty::channel::ChannelDownstreamHandler {
public:
    /**
     * Creates a new instance with the default decoder options
     * (<tt>maxInitialLineLength (4096}</tt>, <tt>maxHeaderSize (8192)</tt>, and
     * <tt>maxChunkSize (8192)</tt>).
     */
    HttpClientCodec()
        : decoder(4096, 8192, 8192) {
        decoder.setHttpClientCodec(this);
        encoder.setHttpClientCodec(this);
    }

    /**
     * Creates a new instance with the specified decoder options.
     */
    HttpClientCodec(
        int maxInitialLineLength, int maxHeaderSize, int maxChunkSize)
        : decoder(maxInitialLineLength, maxHeaderSize, maxChunkSize) {
        decoder.setHttpClientCodec(this);
        encoder.setHttpClientCodec(this);
    }

    virtual void handleUpstream(ChannelHandlerContext& ctx, const ChannelEvent& e) {
        decoder.handleUpstream(ctx, e);
    }
    virtual void messageReceived(ChannelHandlerContext& ctx, const MessageEvent& e) {
        decoder.messageReceived(ctx, e);
    }
    virtual void exceptionCaught(ChannelHandlerContext& ctx, const ExceptionEvent& e) {
        decoder.exceptionCaught(ctx, e);
    }
    virtual void writeCompleted(ChannelHandlerContext& ctx, const WriteCompletionEvent& e) {
        decoder.writeCompleted(ctx, e);
    }
    virtual void channelStateChanged(ChannelHandlerContext& ctx, const ChannelStateEvent& e) {
        decoder.channelStateChanged(ctx, e);
    }
    virtual void childChannelStateChanged(ChannelHandlerContext& ctx, const ChildChannelStateEvent& e) {
        decoder.childChannelStateChanged(ctx, e);
    }

    virtual void handleDownstream(ChannelHandlerContext& ctx, const ChannelEvent& e) {
        encoder.handleDownstream(ctx, e);
    }
    virtual void writeRequested(ChannelHandlerContext& ctx, const MessageEvent& e) {
        encoder.writeRequested(ctx, e);
    }
    virtual void stateChangeRequested(ChannelHandlerContext& ctx, const ChannelStateEvent& e) {
        encoder.stateChangeRequested(ctx, e);
    }

    virtual ChannelHandlerPtr clone() {
        return ChannelHandlerPtr(
                   dynamic_cast<ChannelHandler*>(
                       new HttpClientCodec(decoder.getMaxInitialLineLength(),
                                           decoder.getMaxHeaderSize(),
                                           decoder.getMaxChunkSize())));
    }
    virtual std::string toString() const { return "HttpClientCodec"; }

    bool isDone() const { return done; }

private:
    class Encoder : public HttpRequestEncoder {
    public:
        Encoder() {}
        virtual ~Encoder() {}

        void setHttpClientCodec(HttpClientCodec* clientCodec) {
            BOOST_ASSERT(clientCodec);
            codec = clientCodec;
        }

        virtual ChannelHandlerPtr clone() {
            return ChannelHandlerPtr(new Encoder);
        }
        virtual std::string toString() const {
            return "HttpClientCodec::Encoder";
        }

    protected:
        virtual ChannelMessage encode(
            ChannelHandlerContext& ctx, const ChannelPtr& channel, ChannelMessage& msg) {
            HttpRequestPtr request = msg.smartPointer<HttpRequest, HttpMessage>();

            if (request && !codec->isDone()) {
                codec->queue.push_front(request->getMethod());
            }

            return HttpRequestEncoder::encode(ctx, channel, msg);
        }

    private:
        HttpClientCodec* codec;
    };

    class Decoder : public HttpResponseDecoder {
    public:
        Decoder(int maxInitialLineLength, int maxHeaderSize, int maxChunkSize)
            : HttpResponseDecoder(maxInitialLineLength, maxHeaderSize, maxChunkSize) {
        }
        virtual ~Decoder() {}

        void setHttpClientCodec(HttpClientCodec* clientCodec) {
            BOOST_ASSERT(clientCodec);
            codec = clientCodec;
        }

        virtual ChannelHandlerPtr clone() {
            return ChannelHandlerPtr(new Decoder(maxInitialLineLength,
                                                 maxHeaderSize,
                                                 maxChunkSize));
        }
        virtual std::string toString() const {
            return "HttpClientCodec::Decoder";
        }

        int getMaxInitialLineLength() const { return maxInitialLineLength; }
        int getMaxHeaderSize() const { return maxHeaderSize; }
        int getMaxChunkSize() const { return maxChunkSize; }

    protected:
        virtual ChannelMessage decode(ChannelHandlerContext& ctx,
                                      const ChannelPtr& channel,
                                      const ReplayingDecoderBufferPtr& buffer,
                                      const int& state) {
            if (codec->isDone()) {
                return buffer->readBytes(actualReadableBytes());
            }
            else {
                return HttpResponseDecoder::decode(ctx, channel, buffer, state);
            }
        }

        virtual bool isContentAlwaysEmpty(const HttpMessage& msg) const {
            const HttpResponse* response =
                dynamic_cast<const HttpResponse*>(&msg);
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
                if (HttpMethod::HEAD.equals(method)) {
                    // Interesting edge case:
                    // Zero-byte chunk will appear if Transfer-Encoding of the
                    // response is 'chunked'.  This is probably because of the
                    // trailing headers.
                    return !msg.isChunked();
                }

                break;

            case 'C':

                // Successful CONNECT request results in a response with empty body.
                if (statusCode == 200) {
                    if (HttpMethod::CONNECT.equals(method)) {
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
        HttpClientCodec* codec;
    };

private:
    /** A queue that is used for correlating a request and a response. */
    std::deque<HttpMethod> queue;

    /** If true, decoding stops (i.e. pass-through) */
    bool done;

    Encoder encoder;
    Decoder decoder;
};


}
}
}
}

#endif //#if !defined(CETTY_HANDLER_CODEC_HTTP_HTTPCLIENTCODEC_H)

// Local Variables:
// mode: c++
// End:
