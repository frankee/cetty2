#if !defined(CETTY_SERVICE_HTTP_HTTPMESSAGEFILTER_H)
#define CETTY_SERVICE_HTTP_HTTPMESSAGEFILTER_H

/*
 * Copyright (c) 2010-2012 frankee zhou (frankee.zhou at gmail dot com)
 *
 * Distributed under under the Apache License, version 2.0 (the "License").
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 */

#include <cetty/service/ServerFilter.h>
#include <cetty/handler/codec/http/HttpMessagePtr.h>

namespace cetty {
namespace service {
namespace http {

using namespace cetty::service;
using namespace cetty::handler::codec::http;

class HttpMessageFilter
    : public cetty::service::ServiceAdaptor<HttpRequestPtr, HttpResponsePtr> {

protected:
    virtual HttpRequestPtr filterReq(const HttpRequestPtr& req);
    virtual HttpResponsePtr filterRep(const HttpResponsePtr& rep);
};

}
}
}

#endif //#if !defined(CETTY_SERVICE_HTTP_HTTPMESSAGEFILTER_H)

// Local Variables:
// mode: c++
// End:
