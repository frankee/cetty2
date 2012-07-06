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
#include <cetty/channel/SimpleChannelHandler.h>
#include <cetty/channel/MessageEvent.h>
#include <cetty/channel/ChannelMessage.h>
#include <cetty/channel/ChannelHandlerContext.h>
#include <cetty/service/ServiceFuture.h>
#include <cetty/service/OutstandingCall.h>

namespace cetty {
namespace service {

template<typename ReqT, typename RepT>
class ClientServiceMessageHandler : public cetty::channel::SimpleChannelHandler {
public:
    typedef ServiceFuture<RepT> ServiceFutureType;
    typedef OutstandingCall<ReqT, RepT> OutstandingCallType;

    typedef boost::intrusive_ptr<ServiceFutureType> ServiceFuturePtr;
    typedef boost::intrusive_ptr<OutstandingCallType> OutstandingCallPtr;

public:
    ClientServiceMessageHandler() : id(0) {}
    virtual ~ClientServiceMessageHandler() {}

    virtual void messageReceived(ChannelHandlerContext& ctx, const MessageEvent& e) {
        OutstandingCallPtr& response = e.getMessage().smartPointer<OutstandingCallType>();

        if (response) {
            boost::int64_t id = response->getId();

            const OutstandingCallPtr& out = outMessages.front();

            if (out->future) {
                out->future->setSuccess();
            }

            outMessages.pop_front();
        }
        else {
            ctx.sendUpstream(e);
        }
    }

    virtual void writeRequested(ChannelHandlerContext& ctx, const MessageEvent& e) {
        OutstandingCallPtr msg = e.getMessage().smartPointer<OutstandingCallType>();
        msg->setId(++id);
        outMessages.push_back(msg);

        Channels::write(ctx, Channels::future(ctx.getChannel()), ChannelMessage(msg));
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
