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

#include <vector>
#include <string>
#include <cetty/util/NameValueCollection.h>
#include <cetty/handler/codec/http/HttpMessage.h>

namespace cetty {
namespace handler {
namespace codec {
namespace http {

class HttpMethod;
class HttpRequest;

using namespace cetty::util;

typedef boost::intrusive_ptr<HttpRequest> HttpRequestPtr;

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
    virtual ~HttpRequest() {}

    /**
     * Returns the method of this request.
     */
    virtual const HttpMethod& getMethod() const = 0;

    /**
     * Sets the method of this request.
     */
    virtual void setMethod(const HttpMethod& method) = 0;

    /**
     * Returns the URI (or path) of this request.
     */
    virtual const std::string& getUri() const = 0;

    /**
     * Sets the URI (or path) of this request.
     */
    virtual void setUri(const std::string& uri) = 0;

    /**
     * Get the Query parameter of the URI of this request.
     */
    virtual const NameValueCollection& getQueryParameters() const = 0;

    /**
     * Get the path of the URI of this request.
     */
    virtual const std::vector<std::string>& getPathSegments() const = 0;
};

}
}
}
}

#endif //#if !defined(CETTY_HANDLER_CODEC_HTTP_HTTPREQUEST_H)

// Local Variables:
// mode: c++
// End:
