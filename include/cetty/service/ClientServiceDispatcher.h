#if !defined(CETTY_SERVICE_CLIENTDISPATCHER_H)
#define CETTY_SERVICE_CLIENTDISPATCHER_H

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

#include <map>
#include <vector>
#include <boost/bind.hpp>
#include <cetty/bootstrap/ClientBootstrap.h>

#include <cetty/util/MetaProgramming.h>

#include <cetty/channel/ChannelFuture.h>
#include <cetty/channel/ChannelPipeline.h>
#include <cetty/channel/ChannelMessageHandlerContext.h>

#include <cetty/service/Connection.h>
#include <cetty/service/OutstandingCall.h>
#include <cetty/service/ServiceMessageTraits.h>
#include <cetty/service/ServiceRequestHandler.h>
#include <cetty/service/pool/ConnectionPool.h>

namespace cetty {
namespace service {

using namespace cetty::bootstrap;
using namespace cetty::channel;
using namespace cetty::service::pool;

namespace detail {

template<typename Request,
         typename Response,
         typename R>
struct OutboundChanger {
    typedef boost::intrusive_ptr<OutstandingCall<Request, Response> > OutstandingCallPtr;
    static R toOutboundOut(const OutstandingCallPtr& outbound) {
        return outbound;
    }
};


template<typename Request, typename Response>
struct OutboundChanger<Request, Response, Request> {
    typedef boost::intrusive_ptr<OutstandingCall<Request, Response> > OutstandingCallPtr;

    static Request toOutboundOut(const OutstandingCallPtr& outbound) {
        return outbound->request();
    }
};


// template<typename Request, typename Response>
// boost::intrusive_ptr<OutstandingCall<Request, Response> >
//     toOutboundOut<Request, Response, boost::intrusive_ptr<OutstandingCall<Request, Response> > >(
//     const boost::intrusive_ptr<OutstandingCall<Request, Response> >& outbound) {
//         return outbound->request();
// }

}

template<typename H,
         typename Request,
         typename Response>
class ClientServiceDispatcher : private boost::noncopyable {
public:
    typedef ClientServiceDispatcher<H, Request, Response> Self;

    typedef boost::intrusive_ptr<OutstandingCall<Request, Response> > OutstandingCallPtr;
    typedef typename cetty::util::IF<ServiceMessageTraits<Response>::HAS_SERIAL_NUMBER,
            Response,
            OutstandingCallPtr>::Result InboundIn;

    typedef OutstandingCallPtr InboundOut;

    typedef OutstandingCallPtr OutboundIn;

    typedef typename cetty::util::IF<ServiceMessageTraits<Request>::HAS_SERIAL_NUMBER,
            Request,
            OutstandingCallPtr>::Result OutboundOut;

    typedef ChannelMessageContainer<InboundIn, MESSAGE_BLOCK> InboundContainer;
    typedef ChannelMessageContainer<InboundOut, MESSAGE_BLOCK> NextInboundContainer;
    typedef ChannelMessageContainer<OutboundIn, MESSAGE_BLOCK> OutboundContainer;
    typedef ChannelMessageContainer<OutboundOut, MESSAGE_BLOCK> NextOutboundContainer;

    typedef typename InboundContainer::MessageQueue InboundQueue;
    typedef typename OutboundContainer::MessageQueue OutboundQueue;

    typedef ChannelMessageHandlerContext<H,
            InboundIn,
            InboundOut,
            OutboundIn,
            OutboundOut,
            InboundContainer,
            NextInboundContainer,
            OutboundContainer,
            NextOutboundContainer> Context;

    typedef typename Context::Handler Handler;
    typedef typename Context::HandlerPtr HandlerPtr;

    typedef ClientServiceRequestAdaptor<Request, Response, InboundIn, OutboundOut> RequestAdaptor;

public:
    ClientServiceDispatcher(const EventLoopPtr& eventLoop,
                            const Connections& connections,
                            const Channel::Initializer& initializer)
        : eventLoop_(eventLoop),
          pool_(connections) {
    }

    void registerTo(Context& ctx) {

    }

private:
    OutstandingCallPtr toInboundOut(const OutstandingCallPtr& inbound) {
        return inbound;
    }

    OutstandingCallPtr toInboundOut(const Response& inbound) {
        typename std::map<int64_t, OutstandingCallPtr>::iterator itr =
            outStandingCalls_.find(inbound.id());

        if (itr != outStandingCalls_.end()) {
            return itr->second;
        }
    }

    void initializeClientChannel(const ChannelPtr& channel) {
        if (initializer_) {
            initializer_(channel);
        }

        channel->pipeline().addLast<RequestAdaptor::HandlerPtr>("requestAdaptor",
                RequestAdaptor::HandlerPtr(new RequestAdaptor(channel_)));
    }

    void channelActive(ChannelHandlerContext& ctx) {
        channel_ = ctx.channel();
    }

    void messageUpdated(ChannelHandlerContext& ctx) {
        OutboundQueue& queue = inboundContainer->getMessages();
        //while ()
        //         while (!inboundQueue.empty()) {
        //             OutstandingCallPtr msg = inboundQueue.front();
        //             inboundTransfer.unfoldAndAdd(msg);
        //             inboundQueue.pop_front();
        //         }

        ctx.fireMessageUpdated();
    }

    void flush(ChannelHandlerContext& ctx,
               const ChannelFuturePtr& future) {
        bool notify = false;
        ChannelPtr ch = pool_.getChannel();

        if (ch) {
            OutboundQueue& queue = outboundContainer->getMessages();

            while (!queue.empty()) {
                OutboundIn& request = queue.front();

                ch->pipeline().addOutboundMessage<OutboundOut>(
                    detail::OutboundChanger<Request, Response, OutboundOut>::toOutboundOut(request));

                if (ServiceMessageTraits<Request>::HAS_SERIAL_NUMBER) {
                    outStandingCalls_.insert(std::make_pair(request->id(), request));
                }

                notify = true;

                queue.pop_front();
            }

            if (notify) {
                ch->flush(future);
            }
        }
        else {
//             BufferingCall bufferingCall;
//             bufferingCall.calls = outboundQueue;
//             bufferingCall.future = future;
//             bufferingCalls.push_back(bufferingCall);

//             outboundQueue.clear();
//             pool_.getChannel(boost::bind(
//                                  &Self::connectedCallback, this, _1, boost::ref(ctx)));
        }
    }

    void connectedCallback(const ChannelPtr& channel, ChannelHandlerContext& ctx) {
        BOOST_ASSERT(channel);
#if 0

        while (!bufferingCalls.empty()) {
            BufferingCall& call = bufferingCalls.front();

            while (!call.calls.empty()) {
                OutstandingCallPtr& request = call.calls.front();

                channel->pipeline()->addOutboundMessage<OutstandingCallPtr>(request);
                //outboundTransfer.unfoldAndAdd(request);

                outStandingCalls_.insert(std::make_pair(request->getId(), request));

                call.calls.pop_front();
            }

            channel->flush(call.future);

            //ctx.flush(call.future);
            bufferingCalls.pop_front();
        }

#endif
    }

private:
    struct BufferingCall {
        int from;
        int to;
        ChannelFuturePtr future;
    };

private:
    ChannelWeakPtr channel_;
    EventLoopPtr eventLoop_;
    Channel::Initializer initializer_;

    ConnectionPool pool_;

    InboundContainer* inboundContainer;
    OutboundContainer* outboundContainer;

    std::deque<BufferingCall> bufferingCalls;
    std::map<int64_t, OutstandingCallPtr> outStandingCalls_;
};

}
}

#endif //#if !defined(CETTY_SERVICE_CLIENTDISPATCHER_H)

// Local Variables:
// mode: c++
// End:
