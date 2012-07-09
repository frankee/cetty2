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
#include <cetty/channel/Channels.h>
#include <cetty/channel/SimpleChannelHandler.h>
#include <cetty/channel/MessageEvent.h>
#include <cetty/channel/ChannelMessage.h>
#include <cetty/channel/ChannelHandlerContext.h>
#include <cetty/service/OutstandingCall.h>

namespace cetty {
namespace service {

using namespace cetty::channel;

template<typename ReqT, typename RepT>
class ServiceRequestHandler : public cetty::channel::SimpleChannelHandler {
public:
    typedef ServiceFuture<RepT> ServiceFutureType;
    typedef OutstandingCall<ReqT, RepT> OutstandingCallType;
    typedef ServiceRequestHandler<ReqT, RepT> ServiceRequestHandlerType;

    typedef boost::intrusive_ptr<ServiceFutureType> ServiceFuturePtr;
    typedef boost::intrusive_ptr<OutstandingCallType> OutstandingCallPtr;

public:
    ServiceRequestHandler(const ChannelPtr& parent) : parent(parent) {}
    virtual ~ServiceRequestHandler() {}

    virtual void messageReceived(ChannelHandlerContext& ctx, const MessageEvent& e) {
        RepT& response = e.getMessage().value<RepT>();

        if (response) {
            // TODO: using template traits to define the pointer or object.
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
                Channels::fireMessageReceived(parent, ChannelMessage(out));
            }
            outMessages.pop_front();
        }
        else {
            ctx.sendUpstream(e);
        }
    }

    virtual void writeRequested(ChannelHandlerContext& ctx, const MessageEvent& e) {
        OutstandingCallPtr msg = e.getMessage().smartPointer<OutstandingCallType>();
        outMessages.push_back(msg);
        Channels::write(ctx, Channels::future(ctx.getChannel()), ChannelMessage(msg->request));
    }

    virtual ChannelHandlerPtr clone() {
        return ChannelHandlerPtr(new ServiceRequestHandlerType(parent));
    }

    virtual std::string toString() const {
        return "ServiceRequestHandlerType";
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
