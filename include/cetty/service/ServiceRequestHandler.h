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

#include <cetty/Types.h>
#include <cetty/channel/VoidMessage.h>
#include <cetty/channel/ChannelMessageTailLinkContext.h>
#include <cetty/service/OutstandingCall.h>

namespace cetty {
namespace service {

using namespace cetty::channel;

template<typename Request,
         typename Response,
         typename InboundOut = boost::intrusive_ptr<OutstandingCall<Request, Response> >,
         typename OutboundIn = boost::intrusive_ptr<OutstandingCall<Request, Response> > >
class ClientServiceRequestAdaptor : private boost::noncopyable {
public:
    typedef ClientServiceRequestAdaptor<Request, Response> Self;

    typedef ChannelMessageContainer<Response, MESSAGE_BLOCK> InboundContainer;
    typedef ChannelMessageContainer<InboundOut, MESSAGE_BLOCK> NextInboundContainer;
    typedef ChannelMessageContainer<OutboundIn, MESSAGE_BLOCK> OutboundContainer;
    typedef ChannelMessageContainer<Request, MESSAGE_BLOCK> NextOutboundContainer;

    typedef typename InboundContainer::MessageQueue InboundQueue;
    typedef typename OutboundContainer::MessageQueue OutboundQueue;

    typedef ChannelMessageTransfer<InboundOut,
            NextInboundContainer,
            TRANSFER_INBOUND> InboundTransfer;

    typedef ChannelMessageTransfer<Request,
            NextOutboundContainer,
            TRANSFER_OUTBOUND> OutboundTransfer;

    typedef ChannelMessageHandlerContext<Self,
            Response,
            InboundOut,
            OutboundIn,
            Request,
            InboundContainer,
            NextInboundContainer,
            OutboundContainer,
            NextOutboundContainer> Context;

    typedef typename Context::Handler Handler;
    typedef typename Context::HandlerPtr HandlerPtr;

public:
    ClientServiceRequestAdaptor(const ChannelPtr& parent)
        : flushIndex_(0),
          parent_(parent),
          pipeline_(parent->pipeline()),
          inboundTransfer_(),
          inboundContainer_(),
          outboundTransfer_(),
          outboundContainer_() {
    }

    ClientServiceRequestAdaptor(const ChannelWeakPtr& parent)
        : flushIndex_(0),
          parent_(parent),
          pipeline_(parent->pipeline()),
          inboundTransfer_(),
          inboundContainer_(),
          outboundTransfer_(),
          outboundContainer_() {
    }

    ~ClientServiceRequestAdaptor() {}

    void registerTo(Context& ctx) {
        inboundTransfer_ = ctx.inboundTransfer();
        inboundContainer_ = ctx.inboundContainer();

        outboundTransfer_ = ctx.outboundTransfer();
        outboundContainer_ = ctx.outboundContainer();

        if (!parent_.expired()) {
            NextInboundContainer* container =
                pipeline_->inboundMessageContainer<InboundOut, MESSAGE_BLOCK>();

            if (container) {
                inboundTransfer_->resetNextContainer(container);
            }
            else {
                LOG_WARN << "NOT found the proper inbound container in parent pipeline";
            }
        }

        ctx.setChannelMessageUpdatedCallback(boost::bind(
                &Self::messageUpdated,
                this,
                _1));

        ctx.setFlushFunctor(boost::bind(
                                &Self::flush,
                                this,
                                _1,
                                _2));
    }

    static ChannelHandlerContext* newContext(const ChannelPtr& parent) {
        return new Context("requestAdaptor",
            RequestAdaptor::HandlerPtr(new RequestAdaptor(parent)));
    }

    static ChannelHandlerContext* newContext(const ChannelWeakPtr& parent) {
        return new Context("requestAdaptor",
            RequestAdaptor::HandlerPtr(new RequestAdaptor(parent)));
    }

private:
    void onPipelineChanged(ChannelHandlerContext& ctx) {

    }

    void messageUpdated(ChannelHandlerContext& ctx) {
        bool notify = false;
        InboundQueue& inboundQueue = inboundContainer_->getMessages();
        OutboundQueue& outboundQueue = outboundContainer_->getMessages();

        while (!inboundQueue.empty()) {
            Response& response = inboundQueue.front();

            const OutstandingCallPtr& out = outboundQueue.front();

            if (!parent_.expired()) {
                inboundTransfer_->unfoldAndAdd(out);
                notify = true;
            }

            outboundQueue.pop_front();
            inboundQueue.pop_front();
            --flushIndex_;
        }

        if (notify) {
            pipeline_->fireMessageUpdated();
        }
    }

    void flush(ChannelHandlerContext& ctx,
               const ChannelFuturePtr& future) {
        OutboundQueue& queue = outboundContainer_->getMessages();

        for (std::size_t i = flushIndex_; i < queue.size(); ++i) {
            outboundTransfer_->unfoldAndAdd(queue[i]->request());
            ++flushIndex_;
        }

        ctx.flush(future);
    }

private:
    int flushIndex_;

    ChannelWeakPtr parent_;
    ChannelPipeline* pipeline_;

    InboundTransfer* inboundTransfer_;
    InboundContainer* inboundContainer_;

    OutboundTransfer* outboundTransfer_;
    OutboundContainer* outboundContainer_;
};

template<typename Request,
         typename Response>
class ClientServiceRequestAdaptor<Request, Response, Request, Response> : private boost::noncopyable {
public:
    typedef ClientServiceRequestAdaptor<Request, Response, Request, Response> Self;
    typedef Response InboundIn;
    typedef ChannelMessageContainer<InboundIn, MESSAGE_BLOCK> InboundContainer;

    typedef ChannelMessageTailLinkContext<Self,
            InboundIn,
            InboundContainer> Context;

    typedef typename Context::Handler Handler;
    typedef typename Context::HandlerPtr HandlerPtr;

public:
    ClientServiceRequestAdaptor(const ChannelPtr& parent)
        : parent_(parent),
          pipeline_(parent->pipeline()) {
    }

    ClientServiceRequestAdaptor(const ChannelWeakPtr& parent)
        : parent_(parent),
          pipeline_(parent->pipeline()) {
    }

    ~ClientServiceRequestAdaptor() {}

    void registerTo(Context& ctx) {
    }

    static ChannelHandlerContext* newContext(const ChannelPtr& parent) {
        return new Context("requestAdaptor",
            RequestAdaptor::HandlerPtr(new RequestAdaptor(parent)));
    }

    static ChannelHandlerContext* newContext(const ChannelWeakPtr& parent) {
        return new Context("requestAdaptor",
            RequestAdaptor::HandlerPtr(new RequestAdaptor(parent)));
    }

private:
    void messageUpdated(ChannelHandlerContext& ctx) {
        if (!parent_.expired()) {
            pipeline_->fireMessageUpdated();
        }
    }

private:
    ChannelWeakPtr parent_;
    ChannelPipeline* pipeline_;
};

}
}

#endif //#if !defined(CETTY_SERVICE_SERVICEREQUESTHANDLER_H)

// Local Variables:
// mode: c++
// End:
