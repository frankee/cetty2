#if !defined(CETTY_HANDLER_CODEC_HTTP_HTTPMESSAGE_H)
#define CETTY_HANDLER_CODEC_HTTP_HTTPMESSAGE_H

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
#include <cetty/buffer/ChannelBuffer.h>
#include <cetty/util/ReferenceCounter.h>
#include <cetty/util/NameValueCollection.h>
#include <cetty/handler/codec/http/Cookie.h>
#include <cetty/handler/codec/http/HttpVersion.h>
#include <cetty/handler/codec/http/HttpMessagePtr.h>
#include <cetty/handler/codec/http/HttpTransferEncoding.h>

namespace cetty {
namespace handler {
namespace codec {
namespace http {

using namespace cetty::buffer;
using namespace cetty::util;

/**
* An HTTP message which provides common properties for {@link HttpRequest} and
* {@link HttpResponse}.
*
*
* @author Andy Taylor (andy.taylor@jboss.org)
* @author <a href="http://gleamynode.net/">Trustin Lee</a>
* @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
*
* @see HttpHeaders
*
* @apiviz.landmark
* @apiviz.has org.jboss.netty.handler.codec.http.HttpChunk oneway - - is followed by
*/
class HttpMessage : public cetty::util::ReferenceCounter<HttpMessage, int> {
public:
    typedef NameValueCollection::ConstIterator ConstHeaderIterator;

public:
    HttpMessage();
    HttpMessage(const HttpVersion& version);
    virtual ~HttpMessage() {}

    virtual void clear() {
        httpHeader.clear();
        content.reset();
        transferEncoding = HttpTransferEncoding::SINGLE;
    }

    bool hasHeader(const std::string& name) const {
        return httpHeader.has(name);
    }

    const std::string& getHeader(const std::string& name) const {
        return httpHeader.get(name);
    }

    /**
    * Get the header values with the specified header name.
    *
    * @param name the specified header name
    * @param header the {@link StringList} of header values.
    *               An empty list if there is no such header.
    *
    */
    void getHeaders(const std::string& name, std::vector<std::string>* headers) const {
        if (headers) {
            httpHeader.get(name, headers);
        }
    }

    /**
    * Get the all header names and values that this message contains.
    *
    * @param nameValues The {@link NameValueList} of the header name-value pairs.
    *                   An empty list if there is no header in this message.
    */
    ConstHeaderIterator getFirstHeader() const {
        return httpHeader.begin();
    }

    ConstHeaderIterator getLastHeader() const {
        return httpHeader.end();
    }

    /**
    * Returns <tt>true</tt> if and only if there is a header with the specified
    * header name.
    */
    bool containsHeader(const std::string& name) const {
        return httpHeader.has(name);
    }

    /**
    * Get the {@link StringList} of all header names that this message contains.
    */
    void getHeaderNames(std::vector<std::string>* names) const {
        httpHeader.getNames(names);
    }

    /**
    * Adds a new header with the specified name and string value.
    */
    void addHeader(const std::string& name, const std::string& value) {
        httpHeader.add(name, value);
    }

    /**
    * Adds a new header with the specified name and int value.
    */
    void setHeader(const std::string& name, const std::string& value) {
        httpHeader.set(name, value);
    }

    void setHeader(const std::string& name, int value);

    /**
    * Sets a new header with the specified name and values.  If there is an
    * existing header with the same name, the existing header is removed.
    */
    void setHeader(const std::string& name, const std::vector<std::string>& values) {
        httpHeader.set(name, values.begin(), values.end());
    }

    /**
    * Removes the header with the specified name.
    */
    void removeHeader(const std::string& name) {
        httpHeader.erase(name);
    }

    /**
    * Removes the header with the specified name and value.
    * If only one value with the name, then move the name item completely,
    * otherwise only remove the value.
    */
    void removeHeader(const std::string& name, const std::string& value) {
        httpHeader.erase(name, value);
    }

    /**
    * Removes all headers from this message.
    */
    void clearHeaders() {
        httpHeader.clear();
    }

    void addCookie(const Cookie& cookie);
    void addCookie(const std::string& name, const std::string& value);

    const std::vector<Cookie>& getCookies() const;

    /**
    * Returns the protocol version of this message.
    */
    const HttpVersion& getProtocolVersion() const {
        return version;
    }

    /**
    * Sets the protocol version of this message.
    */
    void setProtocolVersion(const HttpVersion& version) {
        this->version = version;
    }

    /**
    * Returns the content of this message.  If there is no content or
    * {@link #isChunked()} returns <tt>true</tt>, an
    * {@link ChannelBuffers#EMPTY_BUFFER} is returned.
    */
    const ChannelBufferPtr& getContent() const {
        return content;
    }

    /**
    * Sets the content of this message.  If <tt>null</tt> is specified,
    * the content of this message will be set to {@link ChannelBuffers#EMPTY_BUFFER}.
    */
    void setContent(const ChannelBufferPtr& content);

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
    HttpTransferEncoding getTransferEncoding() const;

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
    void setTransferEncoding(HttpTransferEncoding te);

    virtual std::string toString() const;

private:
    HttpVersion version;
    NameValueCollection httpHeader;

    ChannelBufferPtr content;
    HttpTransferEncoding transferEncoding;

    mutable std::vector<Cookie> cookies;
};

}
}
}
}

#endif //#if !defined(CETTY_HANDLER_CODEC_HTTP_HTTPMESSAGE_H)

// Local Variables:
// mode: c++
// End:
