#if !defined(CETTY_SERVICE_CLIENTSERVICEMESSAGEHANDLER_H)
#define CETTY_SERVICE_CLIENTSERVICEMESSAGEHANDLER_H

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
#include <cetty/Types.h>
#include <cetty/channel/ChannelMessageHandlerAdapter.h>
#include <cetty/channel/ChannelInboundMessageHandler.h>
#include <cetty/channel/ChannelOutboundMessageHandler.h>

#include <cetty/channel/VoidChannelMessage.h>
#include <cetty/channel/ChannelHandlerContext.h>
#include <cetty/service/ServiceFuture.h>
#include <cetty/service/OutstandingCall.h>

namespace cetty {
namespace service {

using namespace cetty::channel;

template<typename ReqT, typename RepT>
class ClientServiceMessageHandler
    : public cetty::channel::ChannelMessageHandlerAdapter<
    boost::intrusive_ptr<OutstandingCall<ReqT, RepT> >,
    VoidChannelMessage,
    boost::intrusive_ptr<OutstandingCall<ReqT, RepT> >,
        boost::intrusive_ptr<OutstandingCall<ReqT, RepT> > > {

    using ChannelMessageHandlerAdapter<
    boost::intrusive_ptr<OutstandingCall<ReqT, RepT> >,
          VoidChannelMessage,
          boost::intrusive_ptr<OutstandingCall<ReqT, RepT> >,
          boost::intrusive_ptr<OutstandingCall<ReqT, RepT> > >::outboundTransfer;

    using ChannelInboundMessageHandler<boost::intrusive_ptr<OutstandingCall<ReqT, RepT> > >::inboundQueue;
    using ChannelOutboundMessageHandler<boost::intrusive_ptr<OutstandingCall<ReqT, RepT> > >::outboundQueue;

public:
    typedef ServiceFuture<RepT> ServiceFutureType;
    typedef OutstandingCall<ReqT, RepT> OutstandingCallType;

    typedef boost::intrusive_ptr<ServiceFutureType> ServiceFuturePtr;
    typedef boost::intrusive_ptr<OutstandingCallType> OutstandingCallPtr;

public:
    ClientServiceMessageHandler() : id(0) {}
    virtual ~ClientServiceMessageHandler() {}

    virtual void messageUpdated(ChannelHandlerContext& ctx) {
        bool notify = false;

        while (!inboundQueue.empty()) {
            OutstandingCallPtr& response = inboundQueue.front();

            int64_t id = response->getId();
            const OutstandingCallPtr& out = outMessages.front();

            if (out->future) {
                out->future->setSuccess();
            }

            outMessages.pop_front();
            inboundQueue.pop_front();
        }
    }

    virtual void flush(ChannelHandlerContext& ctx,
                       const ChannelFuturePtr& future) {
        while (!outboundQueue.empty()) {
            OutstandingCallPtr& request = outboundQueue.front();
            request->setId(++id);
            outMessages.push_back(request);

            outboundTransfer.unfoldAndAdd(request);
            outboundQueue.pop_front();
        }

        ctx.flush(future);
    }

    virtual ChannelHandlerPtr clone() {
        return ChannelHandlerPtr(new ClientServiceMessageHandler<ReqT, RepT>());
    }

    virtual std::string toString() const {
        return "ClientServiceMessageHandler";
    }

private:
    int64_t id;
    std::deque<OutstandingCallPtr> outMessages;
};

}
}

#endif //#if !defined(CETTY_SERVICE_CLIENTSERVICEMESSAGEHANDLER_H)

// Local Variables:
// mode: c++
// End:
