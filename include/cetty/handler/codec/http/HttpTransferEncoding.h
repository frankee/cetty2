#if !defined(CETTY_HANDLER_CODEC_HTTP_HTTPTRANSFERENCODING_H)
#define CETTY_HANDLER_CODEC_HTTP_HTTPTRANSFERENCODING_H

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
/*
 * Copyright (c) 2010-2012 frankee zhou (frankee.zhou at gmail dot com)
 *
 * Distributed under under the Apache License, version 2.0 (the "License").
 *
 */

#include <string>
#include <cetty/util/Enum.h>

namespace cetty {
namespace handler {
namespace codec {
namespace http {

/**
 * Represents how an HTTP request or an HTTP response is represented as an {@link HttpMessage}
 * and zero or more {@link HttpChunk}s.
 */
class HttpTransferEncoding : public cetty::util::Enum<HttpTransferEncoding> {
public:
    /**
     * An HTTP message whose transfer encoding is {@code chunked} as defined in
     * <a href="http://www.w3.org/Protocols/rfc2616/rfc2616-sec3.html#sec3.6">the section 3.6 of
     * RFC2616</a> so that the content is split into multiple chunks.  A complete HTTP message is
     * composed of the following:
     * <ol>
     * <li>{@link HttpRequest} or {@link HttpResponse} with empty content</li>
     * <li>A list of {@link HttpChunk}s whose content are not empty</li>
     * <li>{@link HttpChunkTrailer}</li>
     * </ol>
     */
    static const HttpTransferEncoding CHUNKED;

    /**
     * An HTTP message whose transfer encoding is <strong>not</strong> {@code chunked}, but
     * the length of its content is large enough so that the content is split into multiple
     * chunks.  A complete HTTP message is composted of the following.
     * <ol>
     * <li>{@link HttpRequest} or {@link HttpResponse} with empty content</li>
     * <li>A list of {@link HttpChunk}s whose content are not empty</li>
     * <li>{@link HttpChunkTrailer}</li>
     * </ol>
     * The difference from {@link #CHUNKED} is that the transfer encoding of the streamed content
     * is <strong>not</strong> {@code chunked}, and thus {@link HttpMessageEncoder} will
     * encode the content as-is, rather than prepending HTTP chunk headers as defined in
     * <a href="http://www.w3.org/Protocols/rfc2616/rfc2616-sec3.html#sec3.6">the section 3.6 of
     * RFC2616</a>.
     */
    static const HttpTransferEncoding STREAMED;

    /**
     * A self-contained HTTP message which is not followed by any {@link HttpChunk}s.
     * A user can set the content of the message via {@link HttpMessage#setContent(io.netty.buffer.ByteBuf)}.
     */
    static const HttpTransferEncoding SINGLE;

public:
    HttpTransferEncoding(int value, bool single)
        : cetty::util::Enum<HttpTransferEncoding>(value),
          single(single) {}

    HttpTransferEncoding(const HttpTransferEncoding& rhs)
        : cetty::util::Enum<HttpTransferEncoding>(rhs),
          single(rhs.single) {}

    HttpTransferEncoding& operator=(const HttpTransferEncoding& rhs) {
        v = rhs.v;
        single = rhs.single;
        return *this;
    }

    /**
     * Returns {@code true} if and only if a complete HTTP message is composed of an
     * {@link HttpMessage} and one or more {@link HttpChunk}s.
     */
    bool isMultiple() const {
        return !single;
    }

    /**
     * Returns {@code true} if and only if a single {@link HttpMessage} represents a complete
     * HTTP message, not followed by any {@link HttpChunk}s.
     */
    bool isSingle() const {
        return single;
    }

    std::string toString() const;

private:
    bool single;
};

}
}
}
}

#endif //#if !defined(CETTY_HANDLER_CODEC_HTTP_HTTPTRANSFERENCODING_H)

// Local Variables:
// mode: c++
// End:
