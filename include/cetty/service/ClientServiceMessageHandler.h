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
#include <boost/cstdint.hpp>
#include <cetty/channel/ChannelMessageHandler.h>
#include <cetty/channel/ChannelHandlerContext.h>
#include <cetty/handler/codec/CodecUtil.h>
#include <cetty/service/ServiceFuture.h>
#include <cetty/service/OutstandingCall.h>

namespace cetty {
namespace service {

using namespace cetty::channel;
using namespace cetty::handler::codec;

template<typename ReqT, typename RepT>
class ClientServiceMessageHandler
    : public cetty::channel::ChannelMessageHandler<
    boost::intrusive_ptr<OutstandingCall<ReqT, RepT> >,
        boost::intrusive_ptr<OutstandingCall<ReqT, RepT> > > {

public:
    typedef ServiceFuture<RepT> ServiceFutureType;
    typedef OutstandingCall<ReqT, RepT> OutstandingCallType;

    typedef boost::intrusive_ptr<ServiceFutureType> ServiceFuturePtr;
    typedef boost::intrusive_ptr<OutstandingCallType> OutstandingCallPtr;

public:
    ClientServiceMessageHandler() : id(0) {}
    virtual ~ClientServiceMessageHandler() {}

    virtual void messageUpdated(InboundMessageContext& ctx) {
        InboundMessageContext::MessageQueue& in =
            ctx.getInboundMessageQueue();

        bool notify = false;

        while (!in.empty()) {
            OutstandingCallPtr& response = in.front();

            boost::int64_t id = response->getId();
            const OutstandingCallPtr& out = outMessages.front();

            if (out->future) {
                out->future->setSuccess();
            }

            outMessages.pop_front();
            in.pop_front();
        }
    }

    virtual void flush(OutboundMessageContext& ctx,
                       const ChannelFuturePtr& future) {
        OutboundMessageContext::MessageQueue& in =
            ctx.getOutboundMessageQueue();

        while (!in.empty()) {
            OutstandingCallPtr& request = in.front();
            request->setId(++id);
            outMessages.push_back(request);

            CodecUtil<OutstandingCallPtr>::unfoldAndAdd(ctx, request, false);
            in.pop_front();
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
    boost::int64_t id;
    std::deque<OutstandingCallPtr> outMessages;
};

}
}

#endif //#if !defined(CETTY_SERVICE_CLIENTSERVICEMESSAGEHANDLER_H)

// Local Variables:
// mode: c++
// End:
