#if !defined(CETTY_HANDLER_CODEC_HTTP_HTTPHEADERS_H)
#define CETTY_HANDLER_CODEC_HTTP_HTTPHEADERS_H

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

#include <string>
#include <vector>
#include <cetty/util/NameValueCollection.h>
#include <cetty/handler/codec/http/Cookie.h>
#include <cetty/handler/codec/http/HttpVersion.h>
#include <cetty/handler/codec/http/HttpTransferEncoding.h>

namespace cetty {
namespace handler {
namespace codec {
namespace http {

using namespace cetty::util;

/**
 * Provides the constants for the standard HTTP header names and values and
 * commonly used utility methods that accesses an {@link HttpMessage}.
 *
 *
 * @author Andy Taylor (andy.taylor@jboss.org)
 * @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
 *
 * @apiviz.landmark
 * @apiviz.stereotype static
 */

class HttpHeaders {
public:
    /**
     * Standard HTTP header names.
     *
     *
     * @author Andy Taylor (andy.taylor@jboss.org)
     * @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
     *
     * @apiviz.stereotype static
     */
    class Names {
    public:
        /**
         * <tt>"Accept"</tt>
         */
        static const std::string ACCEPT;
        /**
         * <tt>"Accept-Charset"</tt>
         */
        static const std::string ACCEPT_CHARSET;
        /**
         * <tt>"Accept-Encoding"</tt>
         */
        static const std::string ACCEPT_ENCODING;
        /**
         * <tt>"Accept-Language"</tt>
         */
        static const std::string ACCEPT_LANGUAGE;
        /**
         * <tt>"Accept-Ranges"</tt>
         */
        static const std::string ACCEPT_RANGES;
        /**
         * <tt>"Accept-Patch"</tt>
         */
        static const std::string ACCEPT_PATCH;
        /**
         * <tt>"Age"</tt>
         */
        static const std::string AGE;
        /**
         * <tt>"Allow"</tt>
         */
        static const std::string ALLOW;
        /**
         * <tt>"Authorization"</tt>
         */
        static const std::string AUTHORIZATION;
        /**
         * <tt>"Cache-Control"</tt>
         */
        static const std::string CACHE_CONTROL;
        /**
         * <tt>"Connection"</tt>
         */
        static const std::string CONNECTION;
        /**
         * <tt>"Content-Base"</tt>
         */
        static const std::string CONTENT_BASE;
        /**
         * <tt>"Content-Encoding"</tt>
         */
        static const std::string CONTENT_ENCODING;
        /**
         * <tt>"Content-Language"</tt>
         */
        static const std::string CONTENT_LANGUAGE;
        /**
         * <tt>"Content-Length"</tt>
         */
        static const std::string CONTENT_LENGTH;
        /**
         * <tt>"Content-Location"</tt>
         */
        static const std::string CONTENT_LOCATION;
        /**
         * <tt>"Content-Transfer-Encoding"</tt>
         */
        static const std::string CONTENT_TRANSFER_ENCODING;
        /**
         * <tt>"Content-MD5"</tt>
         */
        static const std::string CONTENT_MD5;
        /**
         * <tt>"Content-Range"</tt>
         */
        static const std::string CONTENT_RANGE;
        /**
         * <tt>"Content-Type"</tt>
         */
        static const std::string CONTENT_TYPE;
        /**
         * <tt>"Cookie"</tt>
         */
        static const std::string COOKIE;
        /**
         * <tt>"Date"</tt>
         */
        static const std::string DATE;
        /**
         * <tt>"ETag"</tt>
         */
        static const std::string ETAG;
        /**
         * <tt>"Expect"</tt>
         */
        static const std::string EXPECT;
        /**
         * <tt>"Expires"</tt>
         */
        static const std::string EXPIRES;
        /**
         * <tt>"From"</tt>
         */
        static const std::string FROM;
        /**
         * <tt>"Host"</tt>
         */
        static const std::string HOST;
        /**
         * <tt>"If-Match"</tt>
         */
        static const std::string IF_MATCH;
        /**
         * <tt>"If-Modified-Since"</tt>
         */
        static const std::string IF_MODIFIED_SINCE;
        /**
         * <tt>"If-None-Match"</tt>
         */
        static const std::string IF_NONE_MATCH;
        /**
         * <tt>"If-Range"</tt>
         */
        static const std::string IF_RANGE;
        /**
         * <tt>"If-Unmodified-Since"</tt>
         */
        static const std::string IF_UNMODIFIED_SINCE;
        /**
         * <tt>"Last-Modified"</tt>
         */
        static const std::string LAST_MODIFIED;
        /**
         * <tt>"Location"</tt>
         */
        static const std::string LOCATION;
        /**
         * <tt>"Max-Forwards"</tt>
         */
        static const std::string MAX_FORWARDS;
        /**
         * <tt>"Origin"</tt>
         */
        static const std::string ORIGIN;
        /**
         * <tt>"Pragma"</tt>
         */
        static const std::string PRAGMA;
        /**
         * <tt>"Proxy-Authenticate"</tt>
         */
        static const std::string PROXY_AUTHENTICATE;
        /**
         * <tt>"Proxy-Authorization"</tt>
         */
        static const std::string PROXY_AUTHORIZATION;
        /**
         * <tt>"Range"</tt>
         */
        static const std::string RANGE;
        /**
         * <tt>"Referer"</tt>
         */
        static const std::string REFERER;
        /**
         * <tt>"Retry-After"</tt>
         */
        static const std::string RETRY_AFTER;
        /**
         * <tt>"Sec-WebSocket-Key1"</tt>
         */
        static const std::string SEC_WEBSOCKET_KEY1;
        /**
         * <tt>"Sec-WebSocket-Key2"</tt>
         */
        static const std::string SEC_WEBSOCKET_KEY2;
        /**
         * <tt>"Sec-WebSocket-Location"</tt>
         */
        static const std::string SEC_WEBSOCKET_LOCATION;
        /**
         * <tt>"Sec-WebSocket-Origin"</tt>
         */
        static const std::string SEC_WEBSOCKET_ORIGIN;
        /**
         * <tt>"Sec-WebSocket-Protocol"</tt>
         */
        static const std::string SEC_WEBSOCKET_PROTOCOL;
        /**
         * {@code "Sec-WebSocket-Version"}
         */
        static const std::string SEC_WEBSOCKET_VERSION;
        /**
         * {@code "Sec-WebSocket-Key"}
         */
        static const std::string SEC_WEBSOCKET_KEY;
        /**
         * {@code "Sec-WebSocket-Accept"}
         */
        static const std::string SEC_WEBSOCKET_ACCEPT;
        /**
         * <tt>"Server"</tt>
         */
        static const std::string SERVER;
        /**
         * <tt>"Set-Cookie"</tt>
         */
        static const std::string SET_COOKIE;
        /**
         * <tt>"Set-Cookie2"</tt>
         */
        static const std::string SET_COOKIE2;
        /**
         * <tt>"TE"</tt>
         */
        static const std::string TE;
        /**
         * <tt>"Trailer"</tt>
         */
        static const std::string TRAILER;
        /**
         * <tt>"Transfer-Encoding"</tt>
         */
        static const std::string TRANSFER_ENCODING;
        /**
         * <tt>"Upgrade"</tt>
         */
        static const std::string UPGRADE;
        /**
         * <tt>"User-Agent"</tt>
         */
        static const std::string USER_AGENT;
        /**
         * <tt>"Vary"</tt>
         */
        static const std::string VARY;
        /**
         * <tt>"Via"</tt>
         */
        static const std::string VIA;
        /**
         * <tt>"Warning"</tt>
         */
        static const std::string WARNING;
        /**
         * <tt>"WebSocket-Location"</tt>
         */
        static const std::string WEBSOCKET_LOCATION;
        /**
         * <tt>"WebSocket-Origin"</tt>
         */
        static const std::string WEBSOCKET_ORIGIN;
        /**
         * <tt>"WebSocket-Protocol"</tt>
         */
        static const std::string WEBSOCKET_PROTOCOL;
        /**
         * <tt>"WWW-Authenticate"</tt>
         */
        static const std::string WWW_AUTHENTICATE;
    };

    /**
     * Standard HTTP header values.
     *
     *
     * @author Andy Taylor (andy.taylor@jboss.org)
     * @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
     *
     * @apiviz.stereotype static
     */
    class Values {
    public:
        /**
         * <tt>"base64"</tt>
         */
        static const std::string BASE64;
        /**
         * <tt>"binary"</tt>
         */
        static const std::string BINARY;
        /**
         * <tt>"bytes"</tt>
         */
        static const std::string BYTES;
        /**
         * <tt>"charset"</tt>
         */
        static const std::string CHARSET;
        /**
         * <tt>"chunked"</tt>
         */
        static const std::string CHUNKED;
        /**
         * <tt>"close"</tt>
         */
        static const std::string CLOSE;
        /**
         * <tt>"compress"</tt>
         */
        static const std::string COMPRESS;
        /**
         * <tt>"100-continue"</tt>
         */
        static const std::string CONTINUE;
        /**
         * <tt>"deflate"</tt>
         */
        static const std::string DEFLATE;
        /**
         * <tt>"gzip"</tt>
         */
        static const std::string GZIP;
        /**
         * <tt>"identity"</tt>
         */
        static const std::string IDENTITY;
        /**
         * <tt>"keep-alive"</tt>
         */
        static const std::string KEEP_ALIVE;
        /**
         * <tt>"max-age"</tt>
         */
        static const std::string MAX_AGE;
        /**
         * <tt>"max-stale"</tt>
         */
        static const std::string MAX_STALE;
        /**
         * <tt>"min-fresh"</tt>
         */
        static const std::string MIN_FRESH;
        /**
         * <tt>"must-revalidate"</tt>
         */
        static const std::string MUST_REVALIDATE;
        /**
         * <tt>"no-cache"</tt>
         */
        static const std::string NO_CACHE;
        /**
         * <tt>"no-store"</tt>
         */
        static const std::string NO_STORE;
        /**
         * <tt>"no-transform"</tt>
         */
        static const std::string NO_TRANSFORM;
        /**
         * <tt>"none"</tt>
         */
        static const std::string NONE;
        /**
         * <tt>"only-if-cached"</tt>
         */
        static const std::string ONLY_IF_CACHED;
        /**
         * <tt>"private"</tt>
         */
        static const std::string PRIVATE;
        /**
         * <tt>"proxy-revalidate"</tt>
         */
        static const std::string PROXY_REVALIDATE;
        /**
         * <tt>"public"</tt>
         */
        static const std::string PUBLIC;
        /**
         * <tt>"quoted-printable"</tt>
         */
        static const std::string QUOTED_PRINTABLE;
        /**
         * <tt>"s-maxage"</tt>
         */
        static const std::string S_MAXAGE;
        /**
         * <tt>"trailers"</tt>
         */
        static const std::string TRAILERS;
        /**
         * <tt>"Upgrade"</tt>
         */
        static const std::string UPGRADE;
        /**
         * <tt>"WebSocket"</tt>
         */
        static const std::string WEBSOCKET;
    };

public:
    typedef NameValueCollection::ConstIterator ConstHeaderIterator;

public:
    HttpHeaders()
        : transferEncoding_(HttpTransferEncoding::SINGLE) {}

    bool hasHeader(const std::string& name) const {
        return headers_.has(name);
    }

    /**
     * Returns the header value with the specified header name.  If there are
     * more than one header value for the specified header name, the first
     * value is returned.
     *
     * @return the header value or <tt>null</tt> if there is no such header
     */
    const std::string& headerValue(const std::string& name) const {
        return headers_.get(name);
    }

    /**
     * Returns the header value with the specified header name.  If there are
     * more than one header value for the specified header name, the first
     * value is returned.
     *
     * @return the header value or the <tt>defaultValue</tt> if there is no such
     *         header
     */
    const std::string& headerValue(const std::string& name,
                                   const std::string& defaultValue) const {
        return headers_.get(name, defaultValue);
    }

    /**
     * Returns the integer header value with the specified header name.  If
     * there are more than one header value for the specified header name, the
     * first value is returned.
     *
     * @return the header value
     * @throws NumberFormatException
     *         if there is no such header or the header value is not a number
     */
    int headerIntValue(const std::string& name) {
        const std::string& value = headers_.get(name);

        if (!value.empty()) {
            return StringUtil::strto32(value);
        }
        else {
            return 0;
        }
    }

    /**
     * Returns the integer header value with the specified header name.  If
     * there are more than one header value for the specified header name, the
     * first value is returned.
     *
     * @return the header value or the <tt>defaultValue</tt> if there is no such
     *         header or the header value is not a number
     */
    int headerIntValue(const std::string& name, int defaultValue) {
        const std::string& value = headers_.get(name);

        if (!value.empty() && StringUtil::isDigits(value)) {
            return StringUtil::strto32(value);
        }
        else {
            return defaultValue;
        }
    }

    /**
    * Get the header values with the specified header name.
    *
    * @param name the specified header name
    * @param header the {@link StringList} of header values.
    *               An empty list if there is no such header.
    *
    */
    void headerValues(const std::string& name, std::vector<std::string>* headers) const {
        if (headers) {
            headers_.get(name, headers);
        }
    }

    /**
    * Get the all header names and values that this message contains.
    *
    * @param nameValues The {@link NameValueList} of the header name-value pairs.
    *                   An empty list if there is no header in this message.
    */
    ConstHeaderIterator firstHeader() const {
        return headers_.begin();
    }

    ConstHeaderIterator lastHeader() const {
        return headers_.end();
    }

    /**
    * Returns <tt>true</tt> if and only if there is a header with the specified
    * header name.
    */
    bool containsHeader(const std::string& name) const {
        return headers_.has(name);
    }

    /**
    * Get the {@link StringList} of all header names that this message contains.
    */
    void headerNames(std::vector<std::string>* names) const {
        headers_.getNames(names);
    }

    /**
    * Adds a new header with the specified name and string value.
    */
    void addHeader(const std::string& name, int value) {
        headers_.add(name, StringUtil::numtostr(value));
    }

    /**
    * Adds a new header with the specified name and string value.
    */
    void addHeader(const std::string& name, const std::string& value) {
        headers_.add(name, value);
    }

    /**
    * Adds a new header with the specified name and int value.
    */
    void setHeader(const std::string& name, int value) {
        headers_.set(name, StringUtil::numtostr(value));
    }

    void setHeader(const std::string& name, const std::string& value) {
        headers_.set(name, value);
    }

    /**
    * Sets a new header with the specified name and values.  If there is an
    * existing header with the same name, the existing header is removed.
    */
    void setHeader(const std::string& name, const std::vector<std::string>& values) {
        headers_.set(name, values.begin(), values.end());
    }

    void setHeader(const std::string& name, const std::vector<int>& values);

    /**
    * Removes the header with the specified name.
    */
    void removeHeader(const std::string& name) {
        headers_.erase(name);
    }

    /**
    * Removes the header with the specified name and value.
    * If only one value with the name, then move the name item completely,
    * otherwise only remove the value.
    */
    void removeHeader(const std::string& name, const std::string& value) {
        headers_.erase(name, value);
    }

    /**
    * Removes all headers from this message.
    */
    void clear() {
        headers_.clear();
        cookies_.clear();
    }

    /**
     * Returns <tt>true</tt> if and only if the connection can remain open and
     * thus 'kept alive'.  This methods respects the value of the
     * <tt>"Connection"</tt> header first and then the return value of
     * {@link HttpVersion#isKeepAliveDefault()}.
     */
    bool keepAlive(const HttpVersion& version = HttpVersion::HTTP_1_1) const;

    /**
     * Sets the value of the <tt>"Connection"</tt> header depending on the
     * protocol version of the specified message.  This method sets or removes
     * the <tt>"Connection"</tt> header depending on what the default keep alive
     * mode of the message's protocol version is, as specified by
     * {@link HttpVersion#isKeepAliveDefault()}.
     * <ul>
     * <li>If the connection is kept alive by default:
     *     <ul>
     *     <li>set to <tt>"close"</tt> if <tt>keepAlive</tt> is <tt>false</tt>.</li>
     *     <li>remove otherwise.</li>
     *     </ul></li>
     * <li>If the connection is closed by default:
     *     <ul>
     *     <li>set to <tt>"keep-alive"</tt> if <tt>keepAlive</tt> is <tt>true</tt>.</li>
     *     <li>remove otherwise.</li>
     *     </ul></li>
     * </ul>
     */
    void setKeepAlive(bool keepAlive,
                      const HttpVersion& version = HttpVersion::HTTP_1_1);

    /**
     * Returns the length of the content.  Please note that this value is
     * not retrieved from {@link HttpMessage#getContent()} but from the
     * <tt>"Content-Length"</tt> header, and thus they are independent from each
     * other.
     *
     * @return the content length or <tt>0</tt> if this message does not have
     *         the <tt>"Content-Length"</tt> header
     */
    int contentLength() const;

    /**
     * Sets the <tt>"Content-Length"</tt> header.
     */
    void setContentLength(int length);

    /**
     * Returns the value of the <tt>"Host"</tt> header.
     */
    const std::string& host() const;

    /**
     * Returns the value of the <tt>"Host"</tt> header.  If there is no such
     * header, the <tt>defaultValue</tt> is returned.
     */
    const std::string& host(const std::string& defaultValue) const;

    /**
     * Sets the <tt>"Host"</tt> header.
     */
    void setHost(const std::string& value);

    /**
     * Returns the transfer encoding of this {@link HttpMessage}.
     * <ul>
     * <li>{@link HttpTransferEncoding#CHUNKED} - an HTTP message whose {@code "Transfer-Encoding"}
     * is {@code "chunked"}.</li>
     * <li>{@link HttpTransferEncoding#STREAMED} - an HTTP message which is not chunked, but
     *     is followed by {@link HttpChunk}s that represent its content.  {@link #getContent()}
     *     returns an empty buffer.</li>
     * <li>{@link HttpTransferEncoding#SINGLE} - a self-contained HTTP message which is not chunked
     *     and {@link #getContent()} returns the full content.</li>
     * </ul>
     */
    const HttpTransferEncoding& transferEncoding() const;

    /**
     * Sets the transfer encoding of this {@link HttpMessage}.
     * <ul>
     * <li>If set to {@link HttpTransferEncoding#CHUNKED}, the {@code "Transfer-Encoding: chunked"}
     * header is set and the {@code "Content-Length"} header and the content of this message are
     * removed automatically.</li>
     * <li>If set to {@link HttpTransferEncoding#STREAMED}, the {@code "Transfer-Encoding: chunked"}
     * header and the content of this message are removed automatically.</li>
     * <li>If set to {@link HttpTransferEncoding#SINGLE}, the {@code "Transfer-Encoding: chunked"}
     * header is removed automatically.</li>
     * </ul>
     * For more information about what {@link HttpTransferEncoding} means, see {@link #getTransferEncoding()}.
     */
    void setTransferEncoding(const HttpTransferEncoding& te);

    void addCookie(const Cookie& cookie);
    void addCookie(const std::string& name, const std::string& value);

    const std::vector<Cookie>& cookies() const;

    void toString(std::string* str) const;

private:
    NameValueCollection headers_;
    HttpTransferEncoding transferEncoding_;

    mutable std::vector<Cookie> cookies_;
};

}
}
}
}

#endif //#if !defined(CETTY_HANDLER_CODEC_HTTP_HTTPHEADERS_H)

// Local Variables:
// mode: c++
// End:
