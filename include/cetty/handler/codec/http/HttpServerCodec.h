#if !defined(CETTY_HANDLER_CODEC_HTTP_HTTPSERVERCODEC_H)
#define CETTY_HANDLER_CODEC_HTTP_HTTPSERVERCODEC_H

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

#include <cetty/handler/codec/ReplayingDecoder.h>
#include <cetty/handler/codec/MessageToBufferEncoder.h>
#include <cetty/handler/codec/http/HttpRequestDecoder.h>
#include <cetty/handler/codec/http/HttpResponseEncoder.h>

namespace cetty {
namespace handler {
namespace codec {
namespace http {

/**
 * A combination of {@link HttpRequestDecoder} and {@link HttpResponseEncoder}
 * which enables easier server side HTTP implementation.
 *
 *
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 * @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
 *
 * @see HttpClientCodec
 *
 * @apiviz.has org.jboss.netty.handler.codec.http.HttpRequestDecoder
 * @apiviz.has org.jboss.netty.handler.codec.http.HttpResponseEncoder
 */

class HttpServerCodec {
public:
    typedef ChannelMessageHandlerContext<HttpServerCodec,
        ChannelBufferPtr,
        HttpPackage,
        HttpPackage,
        ChannelBufferPtr,
        ChannelBufferContainer,
        ChannelMessageContainer<HttpPackage, MESSAGE_BLOCK>,
        ChannelMessageContainer<HttpPackage, MESSAGE_BLOCK>,
        ChannelBufferContainer> Context;

    typedef boost::shared_ptr<HttpServerCodec> HandlerPtr;

public:
    /**
     * Creates a new instance with the default decoder options
     * (<tt>maxInitialLineLength (4096}</tt>, <tt>maxHeaderSize (8192)</tt>, and
     * <tt>maxChunkSize (8192)</tt>).
     */
    HttpServerCodec()
        :  decoderImpl_(4096, 8192, 8192),
           decoder_(boost::bind(&HttpRequestDecoderImpl::decode, &decoderImpl_, _1, _2, _3)),
           encoder_(boost::bind(&HttpResponseEncoderImpl::encode, &encoderImpl_, _1, _2)) {
    }

    /**
     * Creates a new instance with the specified decoder options.
     */
    HttpServerCodec(int maxInitialLineLength, int maxHeaderSize, int maxChunkSize)
        : decoderImpl_(maxInitialLineLength, maxHeaderSize, maxChunkSize) {
    }

    void registerTo(Context& ctx) {
        decoder_.registerTo(ctx);
        encoder_.registerTo(ctx);
    }

private:
    HttpRequestDecoderImpl decoderImpl_;
    HttpResponseEncoderImpl encoderImpl_;

    ReplayingDecoder<HttpServerCodec, HttpPackage, Context> decoder_;
    MessageToBufferEncoder<HttpServerCodec, HttpPackage, Context> encoder_;
};

}
}
}
}

#endif //#if !defined(CETTY_HANDLER_CODEC_HTTP_HTTPSERVERCODEC_H)

// Local Variables:
// mode: c++
// End:
