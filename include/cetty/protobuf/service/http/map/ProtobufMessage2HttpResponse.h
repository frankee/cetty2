#if !defined(CETTY_PROTOBUF_SERVICE_HTTP_MAP_PROTOBUFMESSAGE2HTTPRESPONSE_H)
#define CETTY_PROTOBUF_SERVICE_HTTP_MAP_PROTOBUFMESSAGE2HTTPRESPONSE_H

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

#include <cetty/handler/codec/http/HttpMessagePtr.h>
#include <cetty/protobuf/service/ProtobufServiceMessagePtr.h>
#include <cetty/protobuf/service/http/map/ServiceMapperPtr.h>


namespace cetty {
namespace protobuf {
namespace service {
namespace http {
namespace map {

using namespace cetty::handler::codec::http;
using namespace cetty::protobuf::service;

class ProtobufMessage2HttpResponse {
public:
    ProtobufMessage2HttpResponse();
    ProtobufMessage2HttpResponse(const ServiceResponseMapperPtr& mapper);

    void setResponseMapper(const ServiceResponseMapperPtr& mapper);

    HttpResponsePtr getHttpResponse(const HttpRequestPtr& req,
                                    const ProtobufServiceMessagePtr& message);

private:
    ServiceResponseMapperPtr mapper;
};

}
}
}
}
}

#endif //#if !defined(CETTY_PROTOBUF_SERVICE_HTTP_MAP_PROTOBUFMESSAGE2HTTPRESPONSE_H)

// Local Variables:
// mode: c++
// End:
