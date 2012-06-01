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
#include <cetty/handler/codec/http/HttpMessageFwd.h>
#include <cetty/util/NameValueCollection.h>

namespace cetty {
namespace handler {
namespace codec {
namespace http {

    using namespace cetty::util;

/**
 * The last {@link HttpChunk} which has trailing headers.
 *
 *
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 * @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
 */

class HttpChunkTrailer : public HttpChunk {
public:
    typedef NameValueCollection::ConstIterator ConstHeaderIterator;

public:
    HttpChunkTrailer();
    virtual ~HttpChunkTrailer();

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

    /**
     * Always returns <tt>true</tt>.
     */
    virtual bool isLast() const;

    /**
     * do not allow to set the content.
     */
    virtual void setContent(const ChannelBufferPtr& content);

    virtual std::string toString() const;

private:
    NameValueCollection httpHeader;
};

}
}
}
}

#endif //#if !defined(CETTY_HANDLER_CODEC_HTTP_HTTPCHUNKTRAILER_H)

// Local Variables:
// mode: c++
// End:

