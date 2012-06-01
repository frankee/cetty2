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

#include <cetty/handler/codec/http/HttpMessage.h>
#include <cetty/handler/codec/http/HttpMethod.h>
#include <cetty/handler/codec/http/QueryStringDecoder.h>
#include <cetty/util/URI.h>
#include <cetty/util/NameValueCollection.h>

namespace cetty {
namespace handler {
namespace codec {
namespace http {

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
class HttpRequest : public HttpMessage {
public:
    /**
    * Creates a new instance.
    *
    * @param httpVersion the HTTP version of the request
    * @param method      the HTTP method of the request
    * @param uri         the URI or path of the request
    */
    HttpRequest(const HttpVersion& httpVersion,
        const HttpMethod& method,
        const std::string& uri);

    virtual ~HttpRequest();

    /**
    * Returns the method of this request.
    */

    const HttpMethod& getMethod() const {
        return method;
    }

    /**
    * Sets the method of this request.
    */
    void setMethod(const HttpMethod& method) {
        this->method = method;
    }

    /**
    * Returns the URI (or path) of this request.
    */
    const std::string& getUri() const {
        return uriStr;
    }

    /**
    * Sets the URI (or path) of this request.
    */
    void setUri(const std::string& uri) {
        this->uriStr = uri;
        this->uri = uri;
    }

    void setUri(const char* uri) {
        if (uri) {
            this->uriStr = uri;
            this->uri = uri;
        }
    }

    /**
    * Get the Query parameter of the URI of this request.
    */
    const NameValueCollection& getQueryParameters() const;

    /**
    * Get the path of the URI of this request.
    */
    const std::vector<std::string>& getPathSegments() const;

    virtual std::string toString() const;

    virtual void clear();

private:
    mutable std::vector<std::string> pathSegments;
    mutable NameValueCollection queryParams;

    HttpMethod method;
    std::string uriStr;
    URI uri;
};

}
}
}
}

#endif //#if !defined(CETTY_HANDLER_CODEC_HTTP_HTTPREQUEST_H)

// Local Variables:
// mode: c++
// End:

