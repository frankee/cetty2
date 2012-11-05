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

#include <vector>
#include <boost/bind.hpp>
#include <cetty/bootstrap/ClientBootstrap.h>

#include <cetty/channel/ChannelFuture.h>
#include <cetty/channel/ChannelPipeline.h>
#include <cetty/channel/ChannelMessageHandlerAdapter.h>
#include <cetty/channel/ChannelInboundMessageHandler.h>
#include <cetty/channel/ChannelOutboundMessageHandler.h>
#include <cetty/channel/ChannelHandlerContext.h>
#include <cetty/channel/socket/asio/AsioServicePool.h>
#include <cetty/channel/socket/asio/AsioClientSocketChannelFactory.h>

#include <cetty/service/Connection.h>
#include <cetty/service/pool/ConnectionPool.h>

namespace cetty {
namespace service {

using namespace cetty::bootstrap;
using namespace cetty::channel;
using namespace cetty::channel::socket::asio;
using namespace cetty::service::pool;

template<typename ReqT, typename RepT>
class ClientServiceDispatcher
    : public cetty::channel::ChannelMessageHandlerAdapter<
    boost::intrusive_ptr<OutstandingCall<ReqT, RepT> >,
    boost::intrusive_ptr<OutstandingCall<ReqT, RepT> >,
    boost::intrusive_ptr<OutstandingCall<ReqT, RepT> >,
        boost::intrusive_ptr<OutstandingCall<ReqT, RepT> > > {
protected:
    using ChannelMessageHandlerAdapter<
    boost::intrusive_ptr<OutstandingCall<ReqT, RepT> >,
          boost::intrusive_ptr<OutstandingCall<ReqT, RepT> >,
          boost::intrusive_ptr<OutstandingCall<ReqT, RepT> >,
          boost::intrusive_ptr<OutstandingCall<ReqT, RepT> > >::outboundTransfer;

    using ChannelMessageHandlerAdapter<
        boost::intrusive_ptr<OutstandingCall<ReqT, RepT> >,
        boost::intrusive_ptr<OutstandingCall<ReqT, RepT> >,
        boost::intrusive_ptr<OutstandingCall<ReqT, RepT> >,
        boost::intrusive_ptr<OutstandingCall<ReqT, RepT> > >::inboundTransfer;

    using ChannelInboundMessageHandler<boost::intrusive_ptr<OutstandingCall<ReqT, RepT> > >::inboundQueue;
    using ChannelOutboundMessageHandler<boost::intrusive_ptr<OutstandingCall<ReqT, RepT> > >::outboundQueue;

public:
    typedef OutstandingCall<ReqT, RepT> OutstandingCallType;
    typedef ServiceRequestHandler<ReqT, RepT> ServiceRequestHandlerType;
    typedef ClientServiceDispatcher<ReqT, RepT> ClientServiceDispatcherType;

    typedef boost::intrusive_ptr<OutstandingCallType> OutstandingCallPtr;

public:
    ClientServiceDispatcher(const Connections& connections,
                            const ChannelPipelinePtr& pipeline,
                            const EventLoopPtr& eventLoop)
        : eventLoop(eventLoop),
          defaultPipeline(pipeline),
          connections(connections),
          pool(connections) {
    }

public:
    virtual void channelActive(ChannelHandlerContext& ctx) {
        ChannelPtr ch = ctx.getChannel();
        ChannelPipelinePtr pipeline = ChannelPipelines::pipeline(defaultPipeline);
        pipeline->addLast("requestHandler", new ServiceRequestHandlerType(ch));

        pool.getBootstrap().setPipeline(pipeline);
        pool.getBootstrap().setFactory(new AsioClientSocketChannelFactory(eventLoop));
    }
    
    virtual void messageUpdated(ChannelHandlerContext& ctx) {
        while (!inboundQueue.empty()) {
            OutstandingCallPtr msg = inboundQueue.front();
            inboundTransfer.unfoldAndAdd(msg);
            inboundQueue.pop_front();
        }

        ctx.fireMessageUpdated();
    }

    virtual void flush(ChannelHandlerContext& ctx,
                       const ChannelFuturePtr& future) {
        bool notify = false;
        ChannelPtr ch = pool.getChannel();

        if (ch) {
            while (!outboundQueue.empty()) {
                OutstandingCallPtr& request = outboundQueue.front();

                outboundTransfer.unfoldAndAdd(request);

                outStandingCalls.insert(std::make_pair(request->getId(), request));
                notify = true;

                outboundQueue.pop_front();
            }

            if (notify) {
                ch->flush(future);
            }
        }
        else {
            BufferingCall bufferingCall;
            bufferingCall.calls = outboundQueue;
            bufferingCall.future = future;
            bufferingCalls.push_back(bufferingCall);
            
            outboundQueue.clear();
            pool.getChannel(boost::bind(
                &ClientServiceDispatcherType::connectedCallback, this, _1, boost::ref(ctx)));
        }
    }

    void connectedCallback(const ChannelPtr& channel, ChannelHandlerContext& ctx) {
        BOOST_ASSERT(channel);

        while (!bufferingCalls.empty()) {
            BufferingCall& call = bufferingCalls.front();

            while (!call.calls.empty()) {
                OutstandingCallPtr& request = call.calls.front();

                outboundTransfer.unfoldAndAdd(request);
                
                outStandingCalls.insert(std::make_pair(request->getId(), request));

                call.calls.pop_front();
            }

            channel->flush(call.future);

            //ctx.flush(call.future);
            bufferingCalls.pop_front();
        }
    }

    virtual ChannelHandlerPtr clone() {
        return ChannelHandlerPtr(
                   new ClientServiceDispatcherType(connections,
                           defaultPipeline,
                           eventLoop));
    }

    virtual std::string toString() const { return "ClientServiceDispatcherType"; }

private:
    struct BufferingCall {
        std::deque<OutstandingCallPtr> calls;
        ChannelFuturePtr future;
    };

private:
    EventLoopPtr eventLoop;
    ChannelPipelinePtr defaultPipeline;

    Connections connections;
    ConnectionPool pool;

    std::deque<BufferingCall> bufferingCalls;
    std::map<int64_t, OutstandingCallPtr> outStandingCalls;
};

}
}

#endif //#if !defined(CETTY_SERVICE_CLIENTDISPATCHER_H)

// Local Variables:
// mode: c++
// End:
