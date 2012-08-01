#if !defined(CETTY_SERVICE_SERVICEREQUESTHANDLER_H)
#define CETTY_SERVICE_SERVICEREQUESTHANDLER_H

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
#include <cetty/service/OutstandingCall.h>

namespace cetty {
namespace service {

using namespace cetty::channel;

template<typename ReqT, typename RepT>
class ServiceRequestHandler
    : public cetty::channel::ChannelMessageHandler<
    boost::intrusive_ptr<OutstandingCall<ReqT, RepT> >,
        RepT> {

public:
    typedef ServiceFuture<RepT> ServiceFutureType;
    typedef OutstandingCall<ReqT, RepT> OutstandingCallType;
    typedef ServiceRequestHandler<ReqT, RepT> ServiceRequestHandlerType;

    typedef boost::intrusive_ptr<ServiceFutureType> ServiceFuturePtr;
    typedef boost::intrusive_ptr<OutstandingCallType> OutstandingCallPtr;

public:
    ServiceRequestHandler(const ChannelPtr& parent) : parent(parent) {}
    virtual ~ServiceRequestHandler() {}

    virtual void messageUpdated(InboundMessageContext& ctx) {
        InboundMessageContext::MessageQueue& in =
            ctx.getInboundMessageQueue();

        bool notify = false;

        while (!in.empty()) {
            RepT& response = in.front();

            const OutstandingCallPtr& out = outMessages.front();

            if (out->future) {
                if (parent) { // if has father channel, just delegate the message to father channel.
                    out->future->setResponse(response);
                }
                else {
                    out->future->setSuccess(response);
                }
            }

            if (parent) {
                parent->getPipeline()->addInboundMessage<OutstandingCallPtr>(out);
                notify = true;
            }

            outMessages.pop_front();
        }

        if (notify) {
            parent->getPipeline()->fireMessageUpdated();
        }
    }

    virtual void flush(OutboundMessageContext& ctx,
                       const ChannelFuturePtr& future) {
        OutboundMessageContext::MessageQueue& in =
            ctx.getOutboundMessageQueue();

        while (!in.empty()) {
            OutstandingCallPtr& msg = in.front();
            outMessages.push_back(msg);

            CodecUtil<RepT>::unfoldAndAdd(ctx, msg->request, false);
        }

        ctx.flush(future);
    }

    virtual ChannelHandlerPtr clone() {
        return ChannelHandlerPtr(new ServiceRequestHandlerType(parent));
    }

    virtual std::string toString() const {
        return "ServiceRequestHandler";
    }

private:
    ChannelPtr parent;
    std::deque<OutstandingCallPtr> outMessages;
};

}
}

#endif //#if !defined(CETTY_SERVICE_SERVICEREQUESTHANDLER_H)

// Local Variables:
// mode: c++
// End:
