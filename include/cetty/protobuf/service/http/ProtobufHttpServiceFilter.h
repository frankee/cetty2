#if !defined(CETTY_PROTOBUF_SERVICE_HTTP_PROTOBUFHTTPSERVICEFILTER_H)
#define CETTY_PROTOBUF_SERVICE_HTTP_PROTOBUFHTTPSERVICEFILTER_H

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

#include <cetty/service/ServiceFilter.h>
#include <cetty/handler/codec/http/HttpMessagePtr.h>
#include <cetty/protobuf/service/ProtobufServiceMessagePtr.h>
#include <cetty/protobuf/service/http/map/HttpRequest2ProtobufMessage.h>
#include <cetty/protobuf/service/http/map/ProtobufMessage2HttpResponse.h>

namespace cetty {
namespace config {
class ConfigCenter;
}
}

namespace cetty {
namespace protobuf {
namespace service {
namespace http {

using namespace cetty::channel;
using namespace cetty::config;
using namespace cetty::handler::codec::http;
using namespace cetty::protobuf::service;
using namespace cetty::protobuf::service::http::map;

class ProtobufHttpServiceFilter
        : public cetty::service::ServiceFilter<HttpRequestPtr,
        ProtobufServiceMessagePtr,
        ProtobufServiceMessagePtr,
        HttpResponsePtr> {

public:
    ProtobufHttpServiceFilter();

    ProtobufHttpServiceFilter(const ServiceRequestMapperPtr& requestMapper,
                              const ServiceResponseMapperPtr& responseMap);

    virtual ChannelHandlerPtr clone();
    virtual std::string toString() const;

protected:
    virtual ProtobufServiceMessagePtr filterRequest(ChannelHandlerContext& ctx,
        const HttpRequestPtr& req);
    virtual HttpResponsePtr filterResponse(ChannelHandlerContext& ctx,
        const HttpRequestPtr& req,
        const ProtobufServiceMessagePtr& rep);

private:
    ServiceRequestMapperPtr requestMapper;
    ServiceResponseMapperPtr responseMapper;

    HttpRequest2ProtobufMessage http2proto;
    ProtobufMessage2HttpResponse proto2http;
};

}
}
}
}

#endif //#if !defined(CETTY_PROTOBUF_SERVICE_HTTP_PROTOBUFHTTPSERVICEFILTER_H)

// Local Variables:
// mode: c++
// End:
