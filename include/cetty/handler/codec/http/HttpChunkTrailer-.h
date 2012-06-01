#if !defined(CETTY_HANDLER_CODEC_HTTP_HTTPCHUNKTRAILER_H)
#define CETTY_HANDLER_CODEC_HTTP_HTTPCHUNKTRAILER_H

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

#include <cetty/handler/codec/http/HttpChunk.h>

namespace cetty {
namespace handler {
namespace codec {
namespace http {

class HttpHeader;

/**
 * The last {@link HttpChunk} which has trailing headers.
 *
 *
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 * @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
 */

class HttpChunkTrailer : public HttpChunk {
public:
    typedef HttpHeader::IntList       IntList;
    typedef HttpHeader::StringList    StringList;
    typedef HttpHeader::NameValueList NameValueList;

public:
    virtual ~HttpChunkTrailer() {}

    /**
     * Always returns <tt>true</tt>.
     */
    virtual bool isLast() const { return true; }

    /**
     * Returns the trailing http header.
     *
     * @return the HttpHeader.
     */
    virtual HttpHeader& header() = 0;

    /**
     * Returns the trailing http header.
     *
     * @return the HttpHeader.
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
    virtual void getHeaders(const std::string& name, StringList& headers) const = 0;

    /**
     * Returns the all header names and values that this message contains.
     *
     * @return the {@link List} of the header name-value pairs.  An empty list
     *         if there is no header in this message.
     */
    virtual NameValueList getHeaders() const = 0;
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
    virtual void getHeaderNames(StringList& names) const = 0;

    /**
     * Adds a new header with the specified name and value.
     */
    virtual void addHeader(const std::string& name, const std::string& value) = 0;
    virtual void addHeader(const std::string& name, int value) = 0;

    /**
     * Sets a new header with the specified name and value.  If there is an
     * existing header with the same name, the existing header is removed.
     */
    virtual void setHeader(const std::string& name, const std::string& value) = 0;
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
};

typedef boost::intrusive_ptr<HttpChunkTrailer> HttpChunkTrailerPtr;

}
}
}
}

#endif //#if !defined(CETTY_HANDLER_CODEC_HTTP_HTTPCHUNKTRAILER_H)

// Local Variables:
// mode: c++
// End:

