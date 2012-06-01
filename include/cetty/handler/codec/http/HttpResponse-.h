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

#include <cetty/handler/codec/http/HttpMessage.h>

namespace cetty {
namespace handler {
namespace codec {
namespace http {

class HttpResponseStatus;

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

class HttpResponse : public HttpMessage {
public:
    virtual ~HttpResponse() {}

    /**
     * Returns the status of this response.
     */
    virtual const HttpResponseStatus& getStatus() const = 0;

    /**
     * Sets the status of this response.
     */
    virtual void setStatus(const HttpResponseStatus& status) = 0;
};

typedef boost::intrusive_ptr<HttpResponse> HttpResponsePtr;

}
}
}
}

#endif //#if !defined(CETTY_HANDLER_CODEC_HTTP_HTTPRESPONSE_H)

// Local Variables:
// mode: c++
// End:

