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

#include <cetty/channel/SimpleChannelHandler.h>

namespace cetty {
namespace service {

using namespace cetty::channel;

template<typename RequestT, typename ResponseT>
class OutstandingCall {
public:
    typedef boost::intrusive_ptr<ServiceFuture<ResponseT> > ServiceFuturePtr;

public:
    RequestT request;
    ServiceFuturePtr future;
};

template<typename RequestT, typename ResponseT>
class ServiceRequestHandler : public cetty::channel::SimpleChannelHandler {
public:
    typedef boost::intrusive_ptr<ServiceFuture<ResponseT> > ServiceFuturePtr;
    typedef OutstandingCall<RequestT, ResponseT> OutstandingMessage;

public:
    ServiceRequestHandler() {}
    virtual ~ServiceRequestHandler() {}

    virtual void messageReceived(ChannelHandlerContext& ctx, const MessageEvent& e) {
        ResponseT& response = e.getMessage().value();

        if (response) {
            //boost::int64_t id = message->id();

            OutstandingMessage& out = outMessages.front();
#if 0
            {
                std::map<boost::int64_t, OutstandingCall>::iterator it = outstandings.find(id);

                if (it != outstandings.end()) {
                    out = it->second;
                    outstandings.erase(it);
                }
            }
#endif

            if (out.future) {
                out.future->setSuccess(response);
            }

            outMessages.pop_front();
        }
        else {
            ctx.sendUpstream(e);
        }
    }

    virtual void writeRequested(ChannelHandlerContext& ctx, const MessageEvent& e) {
        OutstandingMessage msg = e.getMessage().value<OutstandingMessage>();
        outMessages.push_back(msg);
        //Channels::write(ctx, )
        //ctx.sendDownstream(MessageEvent());
    }

    virtual ChannelHandlerPtr clone() {
        return ChannelHandlerPtr(new ServiceRequestHandler<RequestT, ResponseT>());
    }

    virtual std::string toString() const {
        return "ServiceRequestHandler";
    }

private:
    std::deque<OutstandingMessage> outMessages;
};

}
}

#endif //#if !defined(CETTY_SERVICE_SERVICEREQUESTHANDLER_H)

// Local Variables:
// mode: c++
// End:
