#if !defined(CETTY_PROTOBUF_SERVICE_HANDLER_PROTOBUFSERVICEINBOUNDFILTER_H)
#define CETTY_PROTOBUF_SERVICE_HANDLER_PROTOBUFSERVICEINBOUNDFILTER_H

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

#include <boost/function.hpp>
#include <cetty/channel/ChannelInboundMessageFilter.h>
#include <cetty/protobuf/service/ProtobufServiceMessagePtr.h>

namespace cetty {
namespace protobuf {
namespace service {
namespace handler {

using namespace cetty::channel;
using namespace cetty::protobuf::service;

template<typename H>
class ProtobufServiceInboundFilter : private boost::noncopyable {
public:
    typedef boost::function<void (ChannelHandlerContext&,
                                  ProtobufServiceMessagePtr const&,
                                  bool)> FilterCallback;

    typedef boost::function<void (ChannelHandlerContext&,
                                  ProtobufServiceMessagePtr const&,
                                  FilterCallback const&)> Filter;

    typedef ChannelInboundMessageFilter<H,
            ProtobufServiceMessagePtr,
            ProtobufServiceMessagePtr,
            ProtobufServiceMessagePtr> InboundMessageFilter;

    typedef typename InboundMessageFilter::Context Context;
    typedef typename InboundMessageFilter::InboundContainer InboundContainer;
    typedef typename InboundMessageFilter::InboundQueue InboundQueue;

public:
    ProtobufServiceInboundFilter() {}
    ~ProtobufServiceInboundFilter() {}

public:
    void setFilter(const Filter& filter);

private:
    void messageReceived(ChannelHandlerContext& ctx,
                         const ProtobufServiceMessagePtr& msg) {
    }

    void onFilter(ChannelHandlerContext& ctx,
                  const ProtobufServiceMessagePtr& msg,
                  bool passed) {

    }

private:
    FilterCallback filterCallback_;
};

}
}
}
}

#endif //#if !defined(CETTY_PROTOBUF_SERVICE_HANDLER_PROTOBUFSERVICEINBOUNDFILTER_H)

// Local Variables:
// mode: c++
// End:
