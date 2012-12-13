#if !defined(CETTY_CRAFT_HTTP_HTTPSERVICEFILTER_H)
#define CETTY_CRAFT_HTTP_HTTPSERVICEFILTER_H

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
#include <cetty/handler/codec/http/HttpPackage.h>
#include <cetty/protobuf/service/ProtobufServiceMessagePtr.h>

#include <cetty/craft/http/ServiceMapperPtr.h>
#include <cetty/craft/http/HttpRequest2ProtobufMessage.h>
#include <cetty/craft/http/ProtobufMessage2HttpResponse.h>

namespace cetty {
namespace craft {
namespace http {

using namespace cetty::channel;
using namespace cetty::handler::codec::http;
using namespace cetty::protobuf::service;

class HttpServiceFilter : private boost::noncopyable {
public:
    typedef cetty::service::ServiceFilter<HttpServiceFilter,
            HttpPackage,
            ProtobufServiceMessagePtr,
            ProtobufServiceMessagePtr,
            HttpPackage> Filter;

    typedef Filter::Context Context;

    typedef Filter::Context::Handler Handler;
    typedef Filter::Context::HandlerPtr HandlerPtr;

public:
    HttpServiceFilter();

    HttpServiceFilter(const ServiceRequestMapperPtr& requestMapper,
                      const ServiceResponseMapperPtr& responseMap);

    void registerTo(Context& ctx) {
        filter_.registerTo(ctx);

        ctx.setExceptionCallback(boost::bind(&HttpServiceFilter::exceptionCaught,
                                             this,
                                             _1,
                                             _2));
    }

    void exceptionCaught(ChannelHandlerContext& ctx,
                         const ChannelException& cause);

private:
    ProtobufServiceMessagePtr filterRequest(ChannelHandlerContext& ctx,
                                            const HttpPackage& req);

    HttpPackage filterResponse(ChannelHandlerContext& ctx,
                               const HttpPackage& req,
                               const ProtobufServiceMessagePtr& rep,
                               const ChannelFuturePtr& future);

private:
    Filter filter_;

    ServiceRequestMapperPtr requestMapper_;
    ServiceResponseMapperPtr responseMapper_;

    HttpRequest2ProtobufMessage http2proto_;
    ProtobufMessage2HttpResponse proto2http_;
};

}
}
}

#endif //#if !defined(CETTY_CRAFT_HTTP_HTTPSERVICEFILTER_H)

// Local Variables:
// mode: c++
// End:
