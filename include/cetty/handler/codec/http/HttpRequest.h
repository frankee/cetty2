#if !defined(CETTY_HANDLER_CODEC_HTTP_HTTPREQUEST_H)
#define CETTY_HANDLER_CODEC_HTTP_HTTPREQUEST_H

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

#include <cetty/buffer/Unpooled.h>

#include <cetty/handler/codec/http/HttpMethod.h>
#include <cetty/handler/codec/http/HttpVersion.h>
#include <cetty/handler/codec/http/HttpHeaders.h>
#include <cetty/handler/codec/http/HttpRequestPtr.h>
#include <cetty/handler/codec/http/QueryStringDecoder.h>

#include <cetty/util/URI.h>
#include <cetty/util/ReferenceCounter.h>
#include <cetty/util/NameValueCollection.h>

namespace cetty {
namespace util {
class StringPiece;
}
}

namespace cetty {
namespace handler {
namespace codec {
namespace http {

using namespace cetty::buffer;
using namespace cetty::util;

/**
 * An HTTP request.
 *
 * <h3>Accessing Query Parameters and Cookie</h3>
 * <p>
 * Unlike the Servlet API, a query string is constructed and decomposed by
 * {@link QueryStringEncoder} and {@link QueryStringDecoder}.  {@link Cookie}
 * support is also provided separately via {@link CookieEncoder} and
 * {@link CookieDecoder}.
 *
 *
 * @author Andy Taylor (andy.taylor@jboss.org)
 * @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
 *
 * @see HttpResponse
 * @see CookieEncoder
 * @see CookieDecoder
 */
class HttpRequest : public cetty::util::ReferenceCounter<HttpRequest, int> {
public:
    HttpRequest()
        : method_(HttpMethod::GET),
          version_(HttpVersion::HTTP_1_1) {
    }

    /**
    * Creates a new instance.
    *
    * @param httpVersion the HTTP version of the request
    * @param method      the HTTP method of the request
    * @param uri         the URI or path of the request
    */
    explicit HttpRequest(const HttpVersion& version,
                         const HttpMethod& method,
                         const std::string& uri);

    explicit HttpRequest(const HttpVersion& version,
                         const HttpMethod& method,
                         const StringPiece& uri);

    ~HttpRequest();

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
    * Returns the method of this request.
    */
    const HttpMethod& method() const {
        return method_;
    }

    /**
    * Sets the method of this request.
    */
    void setMethod(const HttpMethod& method) {
        method_ = method;
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

    /**
    * Returns the URI (or path) of this request.
    */
    const URI& uri() const {
        return uri_;
    }

    const std::string& getUriString() const {
        return uriStr_;
    }

    /**
    * Sets the URI (or path) of this request.
    */
    void setUri(const std::string& uri) {
        this->uriStr_ = uri;
        this->uri_ = uri;
    }

    void setUri(const StringPiece& uri);

    /**
    * Get the Query parameter of the URI of this request.
    */
    const NameValueCollection& queryParameters() const;

    /**
    * Get the path of the URI of this request.
    */
    const std::vector<std::string>& pathSegments() const;


    void setLabel(const std::string& label);

    const std::string& label() const;

    /**
     * Returns <tt>true</tt> if and only if the specified message contains the
     * <tt>"Expect: 100-continue"</tt> header.
     */
    bool is100ContinueExpected() const;

    /**
     * Sets the <tt>"Expect: 100-continue"</tt> header to the specified message.
     * If there is any existing <tt>"Expect"</tt> header, they are replaced with
     * the new one.
     */
    void set100ContinueExpected() {
        set100ContinueExpected(true);
    }

    /**
     * Sets or removes the <tt>"Expect: 100-continue"</tt> header to / from the
     * specified message.  If the specified <tt>value</tt> is <tt>true</tt>,
     * the <tt>"Expect: 100-continue"</tt> header is set and all other previous
     * <tt>"Expect"</tt> headers are removed.  Otherwise, all <tt>"Expect"</tt>
     * headers are removed completely.
     */
    void set100ContinueExpected(bool set);

    const HttpTransferEncoding& transferEncoding() const {
        return headers_.transferEncoding();
    }

    void setTransferEncoding(const HttpTransferEncoding& te) {
        headers_.setTransferEncoding(te);

        if (te.multiple()) {
            setContent(Unpooled::EMPTY_BUFFER);
        }
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
            if (HttpMethod::GET == method_ &&
                headers_.containsHeader(HttpHeaders::Names::SEC_WEBSOCKET_KEY1) &&
                headers_.containsHeader(HttpHeaders::Names::SEC_WEBSOCKET_KEY2)) {
                    return 8;
            }
        }

        if (length >= 0) {
            return length;
        }
        else {
            return defaultValue;
        }
    }

    void setContentLength(int length) {
        headers_.setContentLength(length);
    }

    bool keepAlive() const {
        return headers_.keepAlive(version_);
    }

    void setKeepAlive(bool keepAlive) {
        headers_.setKeepAlive(keepAlive, version_);
    }

    std::string toString() const;

    void clear();

private:
    URI uri_;
    std::string uriStr_;

    HttpMethod method_;
    HttpVersion version_;

    HttpHeaders headers_;

    ChannelBufferPtr content_;

    mutable std::string label_;
    mutable NameValueCollection queryParams_;
    mutable std::vector<std::string> pathSegments_;
};

}
}
}
}

#endif //#if !defined(CETTY_HANDLER_CODEC_HTTP_HTTPREQUEST_H)

// Local Variables:
// mode: c++
// End:
