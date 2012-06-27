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

#include <cetty/protobuf/service/http/map/ProtobufMessage2HttpResponse.h>
#include <cetty/handler/codec/http/HttpResponse.h>
#include <cetty/protobuf/service/ProtobufServiceMessage.h>

namespace cetty {
namespace protobuf {
namespace service {
namespace http {
namespace map {

using namespace cetty::handler::codec::http;
using namespace cetty::protobuf::service;

HttpResponsePtr ProtobufMessage2HttpResponse::getHttpResponse(const ProtobufServiceMessagePtr& message) {
    const ServiceReponseMapper::MapValue* v = mapper.match(message->getService(), message->getMethod());

    if (v) {
        HttpResponsePtr response(new HttpResponse);
        ServiceReponseMapper::setHttpHeaders(*v, response);
        ServiceReponseMapper::setHttpContent(*v, response);
        return response;
    }
}

}
}
}
}
}