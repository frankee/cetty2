#if !defined(CETTY_HANDLER_CODEC_HTTP_HTTPCONTENTDECOMPRESSOR_H)
#define CETTY_HANDLER_CODEC_HTTP_HTTPCONTENTDECOMPRESSOR_H

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

#include <cetty/handler/codec/http/HttpContentDecoder.h>

namespace cetty {
namespace handler {
namespace codec {
namespace http {

/**
 * Decompresses an {@link HttpMessage} and an {@link HttpChunk} compressed in
 * <tt>gzip</tt> or <tt>deflate</tt> encoding.  For more information on how this
 * handler modifies the message, please refer to {@link HttpContentDecoder}.
 *
 *
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 * @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
 */
class HttpContentDecompressor : public HttpContentDecoder {
public:
    HttpContentDecompressor() {}
    virtual ~HttpContentDecompressor() {}

protected:
    virtual DecoderEmbedder<ChannelBuffer> newContentDecoder(std::string contentEncoding) throws Exception {
        if ("gzip".equalsIgnoreCase(contentEncoding) || "x-gzip".equalsIgnoreCase(contentEncoding)) {
            return new DecoderEmbedder<ChannelBuffer>(new ZlibDecoder(ZlibWrapper.GZIP));
        }
        else if ("deflate".equalsIgnoreCase(contentEncoding) || "x-deflate".equalsIgnoreCase(contentEncoding)) {
            return new DecoderEmbedder<ChannelBuffer>(new ZlibDecoder(ZlibWrapper.ZLIB));
        }

        // 'identity' or unsupported
        return null;
    }
};

}
}
}
}

#endif //#if !defined(CETTY_HANDLER_CODEC_HTTP_HTTPCONTENTDECOMPRESSOR_H)

// Local Variables:
// mode: c++
// End:
