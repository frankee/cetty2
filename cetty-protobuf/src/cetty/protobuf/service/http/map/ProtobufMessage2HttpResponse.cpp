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
#include <cetty/handler/codec/http/HttpRequest.h>
#include <cetty/protobuf/service/ProtobufServiceMessage.h>
#include <cetty/protobuf/service/http/map/ServiceResponseMapper.h>

namespace cetty {
namespace protobuf {
namespace service {
namespace http {
namespace map {

using namespace cetty::handler::codec::http;
using namespace cetty::protobuf::service;

ProtobufMessage2HttpResponse::ProtobufMessage2HttpResponse() {

}

ProtobufMessage2HttpResponse::ProtobufMessage2HttpResponse(
    const ServiceResponseMapperPtr& mapper)
    : mapper(mapper) {

}

void ProtobufMessage2HttpResponse::setResponseMapper(
    const ServiceResponseMapperPtr& mapper) {
    this->mapper = mapper;
}

HttpResponsePtr ProtobufMessage2HttpResponse::getHttpResponse(
    const HttpRequestPtr& req,
    const ProtobufServiceMessagePtr& message) {
    const ServiceResponseMapper::MapValue* v = mapper->match(message->getService(), message->getMethod());

    if (v) {
        HttpResponsePtr response(new HttpResponse);
        ServiceResponseMapper::setHttpHeaders(*v, response);

        const std::string& format = req->getLabel();

        return response;
    }

    return HttpResponsePtr();
}

}
}
}
}
}
