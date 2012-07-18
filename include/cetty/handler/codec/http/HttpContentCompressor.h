#if !defined(CETTY_HANDLER_CODEC_HTTP_HTTPCONTENTCOMPRESSOR_H)
#define CETTY_HANDLER_CODEC_HTTP_HTTPCONTENTCOMPRESSOR_H

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

#include <cetty/handler/codec/http/HttpContentEncoder.h>

namespace cetty {
namespace handler {
namespace codec {
namespace http {

/**
 * Compresses an {@link HttpMessage} and an {@link HttpChunk} in <tt>gzip</tt> or
 * <tt>deflate</tt> encoding while respecting the <tt>"Accept-Encoding"</tt> header.
 * If there is no matching encoding, no compression is done.  For more
 * information on how this handler modifies the message, please refer to
 * {@link HttpContentEncoder}.
 *
 *
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 * @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
 */

class HttpContentCompressor : public HttpContentEncoder {
public:
    /**
     * Creates a new handler with the default compression level (<tt>6</tt>).
     */
    HttpContentCompressor() : compressionLevel(6) {
    }

    /**
     * Creates a new handler with the specified compression level.
     *
     * @param compressionLevel
     *        <tt>1</tt> yields the fastest compression and <tt>9</tt> yields the
     *        best compression.  <tt>0</tt> means no compression.  The default
     *        compression level is <tt>6</tt>.
     */
    HttpContentCompressor(int compressionLevel) : compressionLevel(compressionLevel) {
        if (compressionLevel < 0 || compressionLevel > 9) {
            throw InvalidArgumentException(
                std::string("compressionLevel: ") +
                Integer::toString(compressionLevel) +
                std::string(" (expected: 0-9)"));
        }
    }

    virtual ~HttpContentCompressor() {}

protected:
    EncoderEmbedder<ChannelBuffer> newContentEncoder(std::string acceptEncoding) {
        ZlibWrapper wrapper = determineWrapper(acceptEncoding);

        if (wrapper == null) {
            return null;
        }

        return new EncoderEmbedder<ChannelBuffer>(new ZlibEncoder(wrapper, compressionLevel));
    }

    std::string getTargetContentEncoding(std::string acceptEncoding) {
        ZlibWrapper wrapper = determineWrapper(acceptEncoding);

        if (wrapper == null) {
            return null;
        }

        switch (wrapper) {
        case GZIP:
            return "gzip";

        case ZLIB:
            return "deflate";

        default:
            throw new Error();
        }
    }

private:
    ZlibWrapper determineWrapper(std::string acceptEncoding) {
        // FIXME: Use the Q value.
        if (acceptEncoding.indexOf("gzip") >= 0) {
            return ZlibWrapper.GZIP;
        }

        if (acceptEncoding.indexOf("deflate") >= 0) {
            return ZlibWrapper.ZLIB;
        }

        return null;
    }

private:
    int compressionLevel;
}


}
}
}
}

#endif //#if !defined(CETTY_HANDLER_CODEC_HTTP_HTTPCONTENTCOMPRESSOR_H)

// Local Variables:
// mode: c++
// End:
