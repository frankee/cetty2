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
#include <cetty/channel/SimpleChannelHandler.h>
#include <cetty/channel/MessageEvent.h>
#include <cetty/channel/ChannelMessage.h>
#include <cetty/channel/ChannelHandlerContext.h>
#include <cetty/util/ReferenceCounter.h>
#include <cetty/service/OutstandingCall.h>

namespace cetty {
namespace service {

using namespace cetty::channel;

template<typename RequestT, typename ResponseT>
class ServiceRequestHandler : public cetty::channel::SimpleChannelHandler {
public:
    typedef boost::intrusive_ptr<ServiceFuture<ResponseT> > ServiceFuturePtr;
    typedef OutstandingCall<RequestT, ResponseT> OutstandingMessage;
    typedef boost::intrusive_ptr<OutstandingCall<RequestT, ResponseT> > OutstandingCallPtr;

public:
    ServiceRequestHandler() : id(0) {}
    virtual ~ServiceRequestHandler() {}

    virtual void messageReceived(ChannelHandlerContext& ctx, const MessageEvent& e) {
        ResponseT& response = e.getMessage().value<ResponseT>();

        if (response) {
            // TODO: using template traits to define the pointer or object.
            const OutstandingCallPtr& out = outMessages.front();
            boost::int64_t id = out->getId();
#if 0
            {
                std::map<boost::int64_t, OutstandingCall>::iterator it = outstandings.find(id);

                if (it != outstandings.end()) {
                    out = it->second;
                    outstandings.erase(it);
                }
            }
#endif

            if (out->future) {
                out->future->setSuccess(response);
            }

            outMessages.pop_front();
        }
        else {
            ctx.sendUpstream(e);
        }
    }

    virtual void writeRequested(ChannelHandlerContext& ctx, const MessageEvent& e) {
        OutstandingCallPtr msg = e.getMessage().smartPointer<OutstandingMessage>();
        msg->setId(++id);
        outMessages.push_back(msg);
        Channels::write(ctx, Channels::future(ctx.getChannel()), ChannelMessage(msg->request));
    }

    virtual ChannelHandlerPtr clone() {
        return ChannelHandlerPtr(new ServiceRequestHandler<RequestT, ResponseT>());
    }

    virtual std::string toString() const {
        return "ServiceRequestHandler";
    }

private:
    boost::int64_t id;
    std::deque<OutstandingCallPtr> outMessages;
};

}
}

#endif //#if !defined(CETTY_SERVICE_SERVICEREQUESTHANDLER_H)

// Local Variables:
// mode: c++
// End:
