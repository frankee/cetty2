#if !defined(CETTY_HANDLER_CODEC_HTTP_HTTPCHUNKAGGREGATOR_H)
#define CETTY_HANDLER_CODEC_HTTP_HTTPCHUNKAGGREGATOR_H

/*
 * Copyright 2009 Red Hat, Inc.
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

#include <cetty/handler/codec/MessageToMessageDecoder.h>
#include <cetty/handler/codec/http/HttpMessageFwd.h>

namespace cetty {
namespace handler {
namespace codec {
namespace http {

using namespace cetty::handler::codec;

/**
 * A {@link ChannelHandler} that aggregates an {@link HttpMessage}
 * and its following {@link HttpChunk}s into a single {@link HttpMessage} with
 * no following {@link HttpChunk}s.  It is useful when you don't want to take
 * care of HTTP messages whose transfer encoding is 'chunked'.  Insert this
 * handler after {@link HttpMessageDecoder} in the {@link ChannelPipeline}:
 * <pre>
 * {@link ChannelPipeline} p = ...;
 * ...
 * p.addLast("decoder", new {@link HttpRequestDecoder}());
 * p.addLast("aggregator", <b>new {@link HttpChunkAggregator}(1048576)</b>);
 * ...
 * p.addLast("encoder", new {@link HttpResponseEncoder}());
 * p.addLast("handler", new HttpRequestHandler());
 * </pre>
 *
 *
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 * @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
 *
 * @apiviz.landmark
 * @apiviz.has org.jboss.netty.handler.codec.http.HttpChunk oneway - - filters out
 */

class HttpChunkAggregator
    : public MessageToMessageDecoder<HttpMessagePtr, HttpMessagePtr> {
public:
    /**
     * Creates a new instance.
     *
     * @param maxContentLength
     *        the maximum length of the aggregated content.
     *        If the length of the aggregated content exceeds this value,
     *        a {@link TooLongFrameException} will be raised.
     */
    HttpChunkAggregator(int maxContentLength);

    virtual ~HttpChunkAggregator() {}

protected:
    virtual HttpMessagePtr decode(ChannelHandlerContext& ctx,
        const HttpMessagePtr& msg);

    void appendToCumulation(const ChannelBufferPtr& input);

private:
    int maxContentLength;
    HttpMessagePtr currentMessage;
};

}
}
}
}

#endif //#if !defined(CETTY_HANDLER_CODEC_HTTP_HTTPCHUNKAGGREGATOR_H)

// Local Variables:
// mode: c++
// End:

