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

#include <cetty/channel/ChannelFutureListener.h>
#include <cetty/handler/codec/http/HttpHeaders.h>
#include <cetty/handler/codec/http/HttpRequest.h>
#include <cetty/handler/codec/http/HttpResponse.h>
#include <cetty/protobuf/service/ProtobufServiceMessage.h>

#include <cetty/craft/http/ServiceRequestMapping.h>
#include <cetty/craft/http/ServiceResponseMapping.h>

namespace cetty {
namespace craft {
namespace http {

using namespace cetty::channel;
using namespace cetty::handler::codec::http;
using namespace cetty::protobuf::service;

HttpServiceFilter::HttpServiceFilter() {
    filter_.setRequestFilter(boost::bind(&HttpServiceFilter::filterRequest,
        this,
        _1,
        _2));

    filter_.setResponseFilter(boost::bind(&HttpServiceFilter::filterResponse,
        this,
        _1,
        _2,
        _3,
        _4));
}

ProtobufServiceMessagePtr HttpServiceFilter::filterRequest(
    ChannelHandlerContext& ctx,
        const HttpPackage& req) {
    HttpRequestPtr request = req.httpRequest();
    ProtobufServiceMessagePtr msg =
        ServiceRequestMapping::instance().toProtobufMessage(request);

    if (!msg) {
        HttpResponsePtr response = new HttpResponse(
            request->version(),
            HttpResponseStatus::BAD_REQUEST);
    }

    return msg;
}

HttpPackage HttpServiceFilter::filterResponse(ChannelHandlerContext& ctx,
        const HttpPackage& req,
        const ProtobufServiceMessagePtr& rep,
        const ChannelFuturePtr& future) {
    HttpRequestPtr request = req.httpRequest();
    HttpResponsePtr response =
        ServiceResponseMapping::instance().toHttpResponse(request, rep);

    if (!response) {
        LOG_WARN << "HttpServiceFilter filterResponse has an error.";
        response = HttpResponsePtr(new HttpResponse(
                                       request->version(),
                                       HttpResponseStatus::BAD_REQUEST));
    }

    if (!request->keepAlive()) {
        LOG_DEBUG << "not keep alive mode, close the channel after writer completed.";
        future->addListener(ChannelFutureListener::CLOSE);
    }

    return response;
}

void HttpServiceFilter::exceptionCaught(ChannelHandlerContext& ctx,
                                        const ChannelException& cause) {
}

}
}
}
