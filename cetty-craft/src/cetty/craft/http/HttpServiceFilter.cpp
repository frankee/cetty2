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

#include <cetty/craft/http/HttpServiceFilter.h>

#include <cetty/config/ConfigCenter.h>
#include <cetty/channel/ChannelFutureListener.h>
#include <cetty/handler/codec/http/HttpHeaders.h>
#include <cetty/handler/codec/http/HttpRequest.h>
#include <cetty/handler/codec/http/HttpResponse.h>
#include <cetty/protobuf/service/ProtobufServiceMessage.h>
#include <cetty/craft/http/ServiceRequestMapper.h>
#include <cetty/craft/http/ServiceResponseMapper.h>

namespace cetty {
namespace craft {
namespace http {

using namespace cetty::channel;
using namespace cetty::handler::codec::http;
using namespace cetty::protobuf::service;

HttpServiceFilter::HttpServiceFilter(const ServiceRequestMapperPtr& requestMapper,
                                     const ServiceResponseMapperPtr& responseMapper)
    : requestMapper(requestMapper),
      responseMapper(responseMapper),
      http2proto(requestMapper),
      proto2http(responseMapper) {
}

HttpServiceFilter::HttpServiceFilter() {
    requestMapper = new ServiceRequestMapper();
    responseMapper = new ServiceResponseMapper();

    http2proto.setRequestMapper(requestMapper);
    proto2http.setResponseMapper(responseMapper);
}

ChannelHandlerPtr HttpServiceFilter::clone() {
    return new HttpServiceFilter(requestMapper, responseMapper);
}

std::string HttpServiceFilter::toString() const {
    return "ProtobufHttpMessageFilter";
}

ProtobufServiceMessagePtr HttpServiceFilter::filterRequest(ChannelHandlerContext& ctx,
        const HttpMessagePtr& req) {
    HttpRequestPtr request = boost::static_pointer_cast<HttpRequest>(req);
    ProtobufServiceMessagePtr msg = http2proto.getProtobufMessage(request);

    if (!msg) {
        HttpResponsePtr response = new HttpResponse(
            request->getProtocolVersion(),
            HttpResponseStatus::BAD_REQUEST);

        outboundTransfer.unfoldAndAdd(
            boost::static_pointer_cast<HttpMessage>(response));
        ctx.flush();
    }

    return msg;
}

HttpPackage HttpServiceFilter::filterResponse(ChannelHandlerContext& ctx,
        const HttpMessagePtr& req,
        const ProtobufServiceMessagePtr& rep,
        const ChannelFuturePtr& future) {
    HttpRequestPtr request = boost::static_pointer_cast<HttpRequest>(req);

    HttpResponsePtr response = proto2http.getHttpResponse(request, rep);

    if (!response) {
        LOG_WARN << "HttpServiceFilter filterResponse has an error.";
        response = HttpResponsePtr(new HttpResponse(
                                       request->getProtocolVersion(),
                                       HttpResponseStatus::BAD_REQUEST));
    }

    if (!HttpHeaders::isKeepAlive(*req)) {
        LOG_DEBUG << "not keep alive mode, close the channel after writer completed.";
        future->addListener(ChannelFutureListener::CLOSE);
    }

    return boost::static_pointer_cast<HttpMessage>(response);
}

void HttpServiceFilter::exceptionCaught(ChannelHandlerContext& ctx,
                                        const ChannelException& cause) {
    ctx.fireExceptionCaught(ctx, cause);
}

}
}
}
