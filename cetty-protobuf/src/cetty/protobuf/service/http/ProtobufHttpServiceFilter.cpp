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

#include <cetty/protobuf/service/http/ProtobufHttpServiceFilter.h>

#include <cetty/handler/codec/http/HttpRequest.h>
#include <cetty/handler/codec/http/HttpResponse.h>
#include <cetty/config/ConfigCenter.h>
#include <cetty/protobuf/service/ProtobufServiceMessage.h>
#include <cetty/protobuf/service/http/map/ServiceRequestMapper.h>
#include <cetty/protobuf/service/http/map/ServiceResponseMapper.h>

namespace cetty {
namespace protobuf {
namespace service {
namespace http {

using namespace cetty::handler::codec::http;
using namespace cetty::protobuf::service;

ProtobufHttpServiceFilter::ProtobufHttpServiceFilter(const ServiceRequestMapperPtr& requestMapper,
        const ServiceResponseMapperPtr& responseMapper)
    : requestMapper(requestMapper),
      responseMapper(responseMapper),
      http2proto(requestMapper),
      proto2http(responseMapper) {
}

ProtobufHttpServiceFilter::ProtobufHttpServiceFilter() {
    requestMapper = new ServiceRequestMapper();
    responseMapper = new ServiceResponseMapper();

    http2proto.setRequestMapper(requestMapper);
    proto2http.setResponseMapper(responseMapper);
}

ChannelHandlerPtr ProtobufHttpServiceFilter::clone() {
    //    return new ProtobufHttpMessageFilter(requestMapper, responseMapper);
    return NULL;
}

std::string ProtobufHttpServiceFilter::toString() const {
    return "ProtobufHttpMessageFilter";
}

ProtobufServiceMessagePtr ProtobufHttpServiceFilter::filterRequest(ChannelHandlerContext& ctx,
        const HttpRequestPtr& req) {
    return http2proto.getProtobufMessage(req);
}

HttpResponsePtr ProtobufHttpServiceFilter::filterResponse(ChannelHandlerContext& ctx,
        const HttpRequestPtr& req,
        const ProtobufServiceMessagePtr& rep) {

    return proto2http.getHttpResponse(req, rep);
}

}
}
}
}
