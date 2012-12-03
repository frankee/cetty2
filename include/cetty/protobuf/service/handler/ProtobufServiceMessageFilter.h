#if !defined(CETTY_PROTOBUF_SERVICE_HANDLER_PROTOBUFSERVICEMESSAGEFILTER_H)
#define CETTY_PROTOBUF_SERVICE_HANDLER_PROTOBUFSERVICEMESSAGEFILTER_H

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

#include <cetty/channel/ChannelInboundMessageHandlerAdapter.h>
#include <cetty/protobuf/service/ProtobufServiceMessagePtr.h>

namespace cetty {
namespace protobuf {
namespace service {
namespace handler {

using namespace cetty::channel;
using namespace cetty::protobuf::service;

class ProtobufServiceMessageFilter
    : public ChannelInboundMessageHandlerAdapter<ProtobufServiceMessagePtr,
      ProtobufServiceMessagePtr,
          ProtobufServiceMessagePtr> {
public:
    ProtobufServiceMessageFilter() {}
    virtual ~ProtobufServiceMessageFilter() {}

    typedef boost::function<void (ChannelHandlerContext&, ProtobufServiceMessagePtr const&, bool)> FilterCallback;

protected:

    virtual void filter(ChannelHandlerContext& ctx,
                        const ProtobufServiceMessagePtr& msg) = 0;

private:
    virtual void messageReceived(ChannelHandlerContext& ctx,
                                 const ProtobufServiceMessagePtr& msg) {
        filter(ctx, msg);
    }
};

}
}
}
}

#endif //#if !defined(CETTY_PROTOBUF_SERVICE_HANDLER_PROTOBUFSERVICEMESSAGEFILTER_H)

// Local Variables:
// mode: c++
// End:
