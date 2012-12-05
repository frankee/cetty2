#if !defined(CETTY_HANDLER_CODEC_HTTP_HTTPREQUESTDECODER_H)
#define CETTY_HANDLER_CODEC_HTTP_HTTPREQUESTDECODER_H

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

#include <cetty/handler/codec/ReplayingDecoder.h>
#include <cetty/handler/codec/http/HttpRequest.h>
#include <cetty/handler/codec/http/HttpRequestCreator.h>
#include <cetty/handler/codec/http/HttpPackageDecoder.h>

namespace cetty {
namespace handler {
namespace codec {
namespace http {

using namespace cetty::util;

/**
 * Decodes {@link ChannelBuffer}s into {@link HttpRequest}s and {@link HttpChunk}s.
 *
 * <h3>Parameters that prevents excessive memory consumption</h3>
 * <table border="1">
 * <tr>
 * <th>Name</th><th>Meaning</th>
 * </tr>
 * <tr>
 * <td><tt>maxInitialLineLength</tt></td>
 * <td>The maximum length of the initial line (e.g. <tt>"GET / HTTP/1.0"</tt>)
 *     If the length of the initial line exceeds this value, a
 *     {@link TooLongFrameException} will be raised.</td>
 * </tr>
 * <tr>
 * <td><tt>maxHeaderSize</tt></td>
 * <td>The maximum length of all headers.  If the sum of the length of each
 *     header exceeds this value, a {@link TooLongFrameException} will be raised.</td>
 * </tr>
 * <tr>
 * <td><tt>maxChunkSize</tt></td>
 * <td>The maximum length of the content or each chunk.  If the content length
 *     exceeds this value, the transfer encoding of the decoded request will be
 *     converted to 'chunked' and the content will be split into multiple
 *     {@link HttpChunk}s.  If the transfer encoding of the HTTP request is
 *     'chunked' already, each chunk will be split into smaller chunks if the
 *     length of the chunk exceeds this value.  If you prefer not to handle
 *     {@link HttpChunk}s in your handler, insert {@link HttpChunkAggregator}
 *     after this decoder in the {@link ChannelPipeline}.</td>
 * </tr>
 * </table>
 *
 *
 * @author Andy Taylor (andy.taylor@jboss.org)
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 * @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
 */

class HttpRequestDecoder : private boost::noncopyable {
public:
    typedef boost::shared_ptr<HttpRequestDecoder> HandlerPtr;

    typedef ChannelMessageHandlerContext<HttpRequestDecoder,
            ChannelBufferPtr,
            HttpPackage,
            VoidMessage,
            VoidMessage,
            ChannelBufferContainer,
            ChannelMessageContainer<HttpPackage, MESSAGE_BLOCK>,
            VoidMessage,
            VoidMessage> Context;

public:
    /**
     * Creates a new instance with the default
     * <tt>maxInitialLineLength (4096)</tt>, <tt>maxHeaderSize (8192)</tt>, and
     * <tt>maxChunkSize (8192)</tt>.
     */
    HttpRequestDecoder()
        : requestDecoder_(HttpPackageDecoder::REQUEST, 4096, 8192, 8192) {
        init();
    }

    /**
     * Creates a new instance with the specified parameters.
     */
    HttpRequestDecoder(int maxInitialLineLength,
                       int maxHeaderSize,
                       int maxChunkSize)
        : requestDecoder_(HttpPackageDecoder::REQUEST,
                          maxInitialLineLength,
                          maxHeaderSize,
                          maxChunkSize) {
        init();
    }

    void registerTo(Context& ctx);

private:
    void init();

    // if has an exception, reply an error message.
    void exceptionCaught(ChannelHandlerContext& ctx,
                         const ChannelException& cause);

private:
    HttpRequestCreator requestCreator_;
    HttpPackageDecoder requestDecoder_;
    ReplayingDecoder<HttpRequestDecoder, HttpPackage, Context> decoder_;
};

}
}
}
}

#endif //#if !defined(CETTY_HANDLER_CODEC_HTTP_HTTPREQUESTDECODER_H)

// Local Variables:
// mode: c++
// End:
