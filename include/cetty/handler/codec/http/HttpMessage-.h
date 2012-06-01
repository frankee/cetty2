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

#include <cetty/util/ReferenceCounter.h>
#include <cetty/buffer/ChannelBuffer.h>
#include <cetty/handler/codec/http/HttpHeader.h>

namespace cetty {
namespace handler {
namespace codec {
namespace http {

using namespace cetty::buffer;

class HttpHeader;
class HttpVersion;

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

class HttpMessage : public cetty::util::ReferenceCounter<HttpMessage> {
public:
    typedef HttpHeader::IntList       IntList;
    typedef HttpHeader::StringList    StringList;
    typedef HttpHeader::NameValueList NameValueList;

public:
    virtual ~HttpMessage() {}

    /**
     * Returns the http Header of this message.
     */
    virtual HttpHeader& header() = 0;

    /**
     * Returns the http Header of this message.
     */
    virtual const HttpHeader& header() const = 0;

    /**
     * Returns the header value with the specified header name.  If there are
     * more than one header value for the specified header name, the first
     * value is returned.
     *
     * @return the header value or <tt>null</tt> if there is no such header
     *
     */
    virtual const std::string& getHeader(const std::string& name) const = 0;

    /**
     * Returns the header values with the specified header name.
     *
     * @return the {@link List} of header values.  An empty list if there is no
     *         such header.
     */
    virtual StringList getHeaders(const std::string& name) const = 0;

    /**
     * Get the header values with the specified header name.
     *
     * @param name the specified header name
     * @param header the {@link StringList} of header values.
     *               An empty list if there is no such header.
     *
     */
    virtual void getHeaders(const std::string& name, StringList& headers) const = 0;

    /**
     * Returns the all header names and values that this message contains.
     *
     * @return the {@link List} of the header name-value pairs.  An empty list
     *         if there is no header in this message.
     */
    virtual NameValueList getHeaders() const = 0;

    /**
     * Get the all header names and values that this message contains.
     *
     * @param nameValues The {@link NameValueList} of the header name-value pairs.
     *                   An empty list if there is no header in this message.
     */
    virtual void getHeaders(NameValueList& nameValues) const = 0;

    /**
     * Returns <tt>true</tt> if and only if there is a header with the specified
     * header name.
     */
    virtual bool containsHeader(const std::string& name) const = 0;

    /**
     * Returns the {@link Set} of all header names that this message contains.
     */
    virtual StringList getHeaderNames() const = 0;

    /**
     * Get the {@link StringList} of all header names that this message contains.
     */
    virtual void getHeaderNames(StringList& names) const = 0;

    /**
     * Adds a new header with the specified name and string value.
     */
    virtual void addHeader(const std::string& name, const std::string& value) = 0;

    /**
     * Adds a new header with the specified name and int value.
     */
    virtual void addHeader(const std::string& name, int value) = 0;

    /**
     * Sets a new header with the specified name and value.  If there is an
     * existing header with the same name, the existing header is removed.
     */
    virtual void setHeader(const std::string& name, const std::string& value) = 0;

    /**
     * Sets a new header with the specified name and value.  If there is an
     * existing header with the same name, the existing header is removed.
     */
    virtual void setHeader(const std::string& name, int value) = 0;

    /**
     * Sets a new header with the specified name and values.  If there is an
     * existing header with the same name, the existing header is removed.
     */
    virtual void setHeader(const std::string& name, const StringList& values) = 0;

    
    virtual void setHeader(const std::string& name, const IntList& values) = 0;

    /**
     * Removes the header with the specified name.
     */
    virtual void removeHeader(const std::string& name) = 0;

    /**
     * Removes the header with the specified name and value.
     * If only one value with the name, then move the name item completely,
     * otherwise only remove the value.
     */
    virtual void removeHeader(const std::string& name, const std::string& value) = 0;

    /**
     * Removes all headers from this message.
     */
    virtual void clearHeaders() = 0;

    /**
     * Returns the protocol version of this message.
     */
    virtual const HttpVersion& getProtocolVersion() const = 0;

    /**
     * Sets the protocol version of this message.
     */
    virtual void setProtocolVersion(const HttpVersion& version) = 0;

    /**
     * Returns the content of this message.  If there is no content or
     * {@link #isChunked()} returns <tt>true</tt>, an
     * {@link ChannelBuffers#EMPTY_BUFFER} is returned.
     */
    virtual const ChannelBufferPtr& getContent() const = 0;

    /**
     * Sets the content of this message.  If <tt>null</tt> is specified,
     * the content of this message will be set to {@link ChannelBuffers#EMPTY_BUFFER}.
     */
    virtual void setContent(const ChannelBufferPtr& content) = 0;

    /**
     * Returns <tt>true</tt> if and only if this message does not have any
     * content but the {@link HttpChunk}s, which is generated by
     * {@link HttpMessageDecoder} consecutively, contain the actual content.
     * <p>
     * Please note that this method will keep returning <tt>true</tt> if the
     * <tt>"Transfer-Encoding"</tt> of this message is <tt>"chunked"</tt>, even if
     * you attempt to override this property by calling {@link #setChunked(bool)}
     * with <tt>false</tt>.
     */
    virtual bool isChunked() const = 0;

    /**
     * Sets if this message does not have any content but the
     * {@link HttpChunk}s, which is generated by {@link HttpMessageDecoder}
     * consecutively, contain the actual content.
     * <p>
     * If this method is called with <tt>true</tt>, the content of this message
     * becomes {@link ChannelBuffers#EMPTY_BUFFER}.
     * <p>
     * Even if this method is called with <tt>false</tt>, {@link #isChunked()}
     * will keep returning <tt>true</tt> if the <tt>"Transfer-Encoding"</tt> of
     * this message is <tt>"chunked"</tt>.
     */
    virtual void setChunked(bool chunked) = 0;


    virtual std::string toString() const = 0;
};

typedef boost::intrusive_ptr<HttpMessage> HttpMessagePtr;

}
}
}
}

#endif //#if !defined(CETTY_HANDLER_CODEC_HTTP_HTTPMESSAGE_H)

// Local Variables:
// mode: c++
// End:

