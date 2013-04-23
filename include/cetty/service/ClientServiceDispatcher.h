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
#include <cetty/service/ServiceMessageWrapper.h>
#include <cetty/service/ClientServiceRequestBridge.h>
#include <cetty/service/pool/ConnectionPool.h>

namespace cetty {
namespace service {

using namespace cetty::util;
using namespace cetty::channel;
using namespace cetty::bootstrap;
using namespace cetty::service::pool;

template<typename H,
         typename Request,
         typename Response>
class ClientServiceDispatcher : private boost::noncopyable {
public:
    typedef ClientServiceDispatcher<H, Request, Response> Self;

    typedef OutstandingCall<Request, Response> Call;
    typedef boost::intrusive_ptr<Call> OutstandingCallPtr;

    typedef typename IF<ServiceMessageWrapper<Response>::HAS_SERIAL_NUMBER,
            Response,
            OutstandingCallPtr>::Result InboundIn;

    typedef OutstandingCallPtr InboundOut;

    typedef OutstandingCallPtr OutboundIn;

    typedef typename IF<ServiceMessageWrapper<Request>::HAS_SERIAL_NUMBER,
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

    typedef ClientServiceRequestBridge<Request,
            Response,
            InboundIn,
            OutboundOut> RequestBridge;

public:
    ClientServiceDispatcher(const EventLoopPtr& eventLoop,
                            const Connections& connections,
                            const Channel::PipelineInitializer& initializer)
        : id_(0),
          eventLoop_(eventLoop),
          initializer_(initializer),
          pool_(connections, eventLoop) {
        pool_.setInitializer(boost::bind(&Self::initializeClientChannel,
                                                this,
                                                _1));
    }

    void registerTo(Context& ctx) {
        inboundContainer_ = ctx.inboundContainer();
        outboundContainer_ = ctx.outboundContainer();

        ctx.setChannelActiveCallback(boost::bind(&Self::channelActive,
                                     this,
                                     _1));

        ctx.setChannelMessageUpdatedCallback(boost::bind(&Self::messageUpdated,
                                             this,
                                             _1));

        ctx.setFlushFunctor(boost::bind(&Self::flush, this, _1, _2));
    }

private:
    void toOutboundOut(const OutstandingCallPtr& outbound,
                       OutstandingCallPtr* out) {
        *out = outbound;
    }

    void toOutboundOut(const OutstandingCallPtr& outbound, Request* out) {
        *out = outbound->request();
    }

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

    bool initializeClientChannel(ChannelPipeline& pipeline) {
        if (initializer_ && !initializer_(pipeline)) {
            return false;
        }

        pipeline.addLast(RequestBridge::newContext(channel_));
        return true;
    }

    void channelActive(ChannelHandlerContext& ctx) {
        channel_ = ctx.channel();
        ctx.fireChannelActive();
    }

    void setSuccess(Response& msg) {
        typename std::map<int64_t, OutstandingCallPtr>::iterator itr =
            outStandingCalls_.find(msg->id());

        if (itr != outStandingCalls_.end()) {
            itr->second->future()->setSuccess(msg);
            outStandingCalls_.erase(itr);
        }
    }

    void setSuccess(OutstandingCallPtr& msg) {
        msg->future()->setSuccess();
    }

    void messageUpdated(ChannelHandlerContext& ctx) {
        InboundQueue& queue = inboundContainer_->getMessages();

        while (!queue.empty()) {
            InboundIn& msg = queue.front();
            setSuccess(msg);

            //inboundTransfer.unfoldAndAdd(msg);
            queue.pop_front();
        }

        ctx.fireMessageUpdated();
    }

    void flush(ChannelHandlerContext& ctx,
               const ChannelFuturePtr& future) {
        bool notify = false;
        ChannelPtr ch = pool_.getChannel();
        OutboundQueue& queue = outboundContainer_->getMessages();

        if (ch) {
            while (!queue.empty()) {
                OutboundIn& request = queue.front();

                OutboundOut out;
                toOutboundOut(request, &out);
                out->setId(id_++);

                ch->pipeline().addOutboundChannelMessage<OutboundOut>(out);

                if (ServiceMessageWrapper<Request>::HAS_SERIAL_NUMBER) {
                    outStandingCalls_.insert(std::make_pair(out->id(), request));
                }

                notify = true;

                queue.pop_front();
            }

            if (notify) {
                ch->flush(future);
            }
        }
        else {
            BufferingCall bufferingCall;
            bufferingCall.calls = queue;
            bufferingCall.future = future;
            bufferingCalls_.push_back(bufferingCall);

            queue.clear();
            pool_.getChannel(boost::bind(
                                 &Self::connectedCallback,
                                 this,
                                 _1,
                                 boost::ref(ctx)));
        }
    }

    void connectedCallback(const ChannelPtr& channel, ChannelHandlerContext& ctx) {
        BOOST_ASSERT(channel);
        bool notify = false;

        while (!bufferingCalls_.empty()) {
            BufferingCall& call = bufferingCalls_.front();

            while (!call.calls.empty()) {
                OutboundIn& request = call.calls.front();

                OutboundOut out;
                toOutboundOut(request, &out);
                out->setId(id_++);

                channel->pipeline().addOutboundChannelMessage<OutboundOut>(out);

                if (ServiceMessageWrapper<Request>::HAS_SERIAL_NUMBER) {
                    outStandingCalls_.insert(std::make_pair(out->id(), request));
                }

                notify = true;
                call.calls.pop_front();
            }

            if (notify) {
                channel->flush(call.future);
            }

            notify = false;
            bufferingCalls_.pop_front();
        }
    }

private:
    struct BufferingCall {
        OutboundQueue calls;
        ChannelFuturePtr future;
    };

private:
    int64_t id_;
    ChannelWeakPtr channel_;
    EventLoopPtr eventLoop_;
    Channel::PipelineInitializer initializer_;

    ConnectionPool pool_;

    InboundContainer* inboundContainer_;
    OutboundContainer* outboundContainer_;

    std::deque<BufferingCall> bufferingCalls_;
    std::map<int64_t, OutstandingCallPtr> outStandingCalls_;
};

}
}

#endif //#if !defined(CETTY_SERVICE_CLIENTDISPATCHER_H)

// Local Variables:
// mode: c++
// End:
