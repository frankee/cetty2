#if !defined(CETTY_PROTOBUF_SERVICE_HANDLER_PROTOBUFSERVICEMESSAGEHANDLER_H)
#define CETTY_PROTOBUF_SERVICE_HANDLER_PROTOBUFSERVICEMESSAGEHANDLER_H

/*
 * Copyright (c) 2010-2011 frankee zhou (frankee.zhou at gmail dot com)
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

#include <deque>
#include <cetty/Types.h>
#include <cetty/channel/ChannelInboundMessageHandlerAdapter.h>
#include <cetty/protobuf/service/ProtobufServiceFuture.h>
#include <cetty/protobuf/service/ProtobufServiceRegister.h>
#include <cetty/protobuf/service/ProtobufServiceMessagePtr.h>

namespace google {
namespace protobuf {
class MethodDescriptor;
class RpcController;
}
}

namespace cetty {
namespace channel {
class Channel;
}
}

namespace cetty {
namespace protobuf {
namespace proto {
class RpcMessage;
}
}
}

namespace cetty {
namespace protobuf {
namespace service {
namespace handler {
using namespace cetty::channel;

class ProtobufServiceMessageHandler;
typedef boost::intrusive_ptr<ProtobufServiceMessageHandler> ProtobufServiceMessageHandlerPtr;

class ProtobufServiceMessageHandler
    : public ChannelInboundMessageHandlerAdapter<ProtobufServiceMessagePtr,
      VoidChannelMessage,
          ProtobufServiceMessagePtr> {
public:
    ProtobufServiceMessageHandler();
    virtual ~ProtobufServiceMessageHandler();

    virtual ChannelHandlerPtr clone();
    virtual std::string toString() const;

protected:
    virtual void messageReceived(ChannelHandlerContext& ctx,
                                 const ProtobufServiceMessagePtr& msg);

private:
    void doneCallback(const MessagePtr& response,
                      ChannelHandlerContext& ctx,
                      ProtobufServiceMessagePtr req,
                      int64_t id);
};

}
}
}
}

#endif //#if !defined(CETTY_PROTOBUF_SERVICE_HANDLER_PROTOBUFSERVICEMESSAGEHANDLER_H)

// Local Variables:
// mode: c++
// End:
