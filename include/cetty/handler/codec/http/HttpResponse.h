#if !defined(CETTY_HANDLER_CODEC_HTTP_HTTPRESPONSE_H)
#define CETTY_HANDLER_CODEC_HTTP_HTTPRESPONSE_H

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

#include <vector>
#include <cetty/buffer/Unpooled.h>

#include <cetty/handler/codec/http/Cookie.h>
#include <cetty/handler/codec/http/HttpHeaders.h>
#include <cetty/handler/codec/http/HttpVersion.h>
#include <cetty/handler/codec/http/HttpResponsePtr.h>
#include <cetty/handler/codec/http/HttpResponseStatus.h>

namespace cetty {
namespace handler {
namespace codec {
namespace http {

    using namespace cetty::buffer;

/**
 * An HTTP response.
 *
 * <h3>Accessing Cookie</h3>
 * <p>
 * Unlike the Servlet API, {@link Cookie} support is provided separately via
 * {@link CookieEncoder} and {@link CookieDecoder}.
 *
 *
 * @author Andy Taylor (andy.taylor@jboss.org)
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 * @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
 *
 * @see HttpRequest
 * @see CookieEncoder
 * @see CookieDecoder
 */
class HttpResponse : public cetty::util::ReferenceCounter<HttpResponse, int> {
public:
    HttpResponse();

    /**
    * Creates a new instance.
    *
    * @param version the HTTP version of this response
    * @param status  the status of this response
    */
    HttpResponse(const HttpVersion& version, const HttpResponseStatus& status);

    /**
    * Returns the protocol version of this message.
    */
    const HttpVersion& version() const {
        return version_;
    }

    /**
    * Sets the protocol version of this message.
    */
    void setVersion(const HttpVersion& version) {
        version_ = version;
    }

    /**
    * Returns the status of this response.
    */
    const HttpResponseStatus& status() const {
        return status_;
    }

    /**
    * Sets the status of this response.
    */
    void setStatus(const HttpResponseStatus& status) {
        status_ = status;
    }

    HttpHeaders& headers() {
        return headers_;
    }

    const HttpHeaders& headers() const {
        return headers_;
    }

    /**
    * Returns the content of this message.  If there is no content or
    * {@link #isChunked()} returns <tt>true</tt>, an
    * {@link ChannelBuffers#EMPTY_BUFFER} is returned.
    */
    const ChannelBufferPtr& content() const {
        return content_;
    }

    /**
    * Sets the content of this message.  If <tt>null</tt> is specified,
    * the content of this message will be set to {@link ChannelBuffers#EMPTY_BUFFER}.
    */
    void setContent(const ChannelBufferPtr& content) {
        if (!content) {
            content_ = Unpooled::EMPTY_BUFFER;
            return;
        }

        if (content->readable() && headers_.transferEncoding().multiple()) {
            //TODO
            //throw InvalidArgumentException(
            //    "non-empty content disallowed if this.chunked == true");
        }

        content_ = content;
    }

    const HttpTransferEncoding& transferEncoding() const {
        return headers_.transferEncoding();
    }

    void setTransferEncoding(const HttpTransferEncoding& te) {
        headers_.setTransferEncoding(te);

        if (te.multiple()) {
            setContent(Unpooled::EMPTY_BUFFER);
        }
    }

    bool keepAlive() const {
        return headers_.keepAlive(version_);
    }

    void setKeepAlive(bool keepAlive) {
        headers_.setKeepAlive(keepAlive, version_);
    }

    int contentLength() {
        return headers_.contentLength();
    }

    /**
     * Returns the length of the content.  Please note that this value is
     * not retrieved from {@link HttpMessage#getContent()} but from the
     * <tt>"Content-Length"</tt> header, and thus they are independent from each
     * other.
     *
     * @return the content length or <tt>defaultValue</tt> if this message does
     *         not have the <tt>"Content-Length"</tt> header
     */
    int contentLength(int defaultValue) const {
        int length = headers_.contentLength();

        if (!length) {
            // WebSockset messages have constant content-lengths.
            if (status_.code() == 101 &&
                headers_.containsHeader(HttpHeaders::Names::SEC_WEBSOCKET_ORIGIN) &&
                headers_.containsHeader(HttpHeaders::Names::SEC_WEBSOCKET_LOCATION)) {
                    return 16;
            }
        }

        return defaultValue;
    }

    void setContentLength(int length) {
        headers_.setContentLength(length);
    }

    std::string toString() const;

    void clear();

private:
    HttpVersion version_;
    HttpResponseStatus status_;

    HttpHeaders headers_;
    ChannelBufferPtr content_;
};

}
}
}
}

#endif //#if !defined(CETTY_HANDLER_CODEC_HTTP_HTTPRESPONSE_H)

// Local Variables:
// mode: c++
// End:
