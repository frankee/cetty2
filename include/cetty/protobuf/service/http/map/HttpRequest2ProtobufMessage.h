#if !defined(CETTY_PROTOBUF_SERVICE_HTTP_MAP_HTTPREQUEST2PROTOBUFMESSAGE_H)
#define CETTY_PROTOBUF_SERVICE_HTTP_MAP_HTTPREQUEST2PROTOBUFMESSAGE_H

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

#include <cetty/handler/codec/http/HttpRequest.h>
#include <cetty/protobuf/service/ProtobufServiceMessagePtr.h>
#include <cetty/protobuf/service/http/map/ServiceRequestMapper.h>

namespace google {
namespace protobuf {
class Message;
class FieldDescriptor;
}
}

namespace cetty {
namespace protobuf {
namespace service {
namespace http {
namespace map {

using namespace cetty::handler::codec::http;
using namespace cetty::protobuf::service;
using google::protobuf::Message;
using google::protobuf::FieldDescriptor;

class HttpRequest2ProtobufMessage {
public:
    HttpRequest2ProtobufMessage(const ServiceRequestMapperPtr& serviceTemplates);

    //1, method uri
    //2, method uri content (format) header(cookie)
    ProtobufServiceMessagePtr getProtobufMessage(const HttpRequestPtr& request);

private:
    bool parseMessage(const HttpServiceTemplate& tmpl,
                      const HttpRequestPtr& request,
                      Message* message);

    bool parseField(const HttpServiceTemplate& tmpl,
                    const HttpRequestPtr& request,
                    const FieldDescriptor* field,
                    Message* message);

private:
    ServiceRequestMapperPtr templates;
};

}
}
}
}
}

#endif //#if !defined(CETTY_PROTOBUF_SERVICE_HTTP_MAP_HTTPREQUEST2PROTOBUFMESSAGE_H)

// Local Variables:
// mode: c++
// End:
