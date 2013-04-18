#if !defined(CETTY_SERVICE_SERVICEFILTER_H)
#define CETTY_SERVICE_SERVICEFILTER_H

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

#include <deque>
#include <cetty/channel/ChannelMessageHandlerContext.h>
#include <cetty/logging/LoggerHelper.h>

namespace cetty {
namespace service {

using namespace cetty::channel;

/**
 *  A Filter acts as a decorator/transformer of a service. It may apply
 * transformations to the input and output of that service:
 *
 *           (*  MyService  *)
 * [ReqIn -> (ReqOut -> RepIn) -> RepOut]
 *
 * For example, you may have a POJO service that takes Strings and
 * parses them as Ints.  If you want to expose this as a Network
 * Service via Thrift, it is nice to isolate the protocol handling
 * from the business rules. Hence you might have a Filter that
 * converts back and forth between Thrift structs. Again, your service
 * deals with POJOs:
 *
 * [ThriftIn -> (String  ->  Int) -> ThriftOut]
 *
 * Thus, a Filter[A, B, C, D] converts a Service[C, D] to a Service[A, B].
 * In other words, it converts a Service[ReqOut, RepIn] to a
 * Service[ReqIn, RepOut].
 *
 */

template<typename H,
         typename RequestIn,
         typename RequestOut,
         typename ResponseIn,
         typename ResponseOut>
class ServiceFilter {
public:
    typedef ServiceFilter<H, RequestIn, RequestOut, ResponseIn, ResponseOut> Self;

    typedef ChannelMessageContainer<RequestIn, MESSAGE_BLOCK> InboundContainer;
    typedef ChannelMessageContainer<ResponseIn, MESSAGE_BLOCK> OutboundContainer;

    typedef typename InboundContainer::MessageQueue InboundQueue;
    typedef typename OutboundContainer::MessageQueue OutboundQueue;

    typedef ChannelMessageContainer<RequestOut, MESSAGE_BLOCK> NextInboundContainer;
    typedef ChannelMessageContainer<ResponseOut, MESSAGE_BLOCK> NextOutboundContainer;

    typedef ChannelMessageTransfer<RequestOut, NextInboundContainer, TRANSFER_INBOUND> InboundTransfer;
    typedef ChannelMessageTransfer<ResponseOut, NextOutboundContainer, TRANSFER_OUTBOUND> OutboundTransfer;

    typedef ChannelMessageHandlerContext<H,
            RequestIn,
            RequestOut,
            ResponseIn,
            ResponseOut,
            InboundContainer,
            NextInboundContainer,
            OutboundContainer,
            NextOutboundContainer> Context;

    typedef typename Context::Handler Handler;
    typedef typename Context::HandlerPtr HandlerPtr;

    /**
     * Get called in the {@link #messageUpdated} when received RequestIn.
     * Filter the message of RequestIn type to the message of RequestOut Type.
     * 
     * Return the empty RequestOut when error happened, and the filter handler
     * should write out the error message of ResponseOut Type.
     *
     * <code>
     * RequestOut myFilter(ChannelHandlerContext& ctx, const RequestIn& req) {
     *     //do filter ...
     *     if (hasError) {
     *           ResponseOut error;
     *           ChannelFuturePtr future = ctx.newFuture();
     *           outboundTransfer().write(error, future);
     *     }
     * }
     * </code>
     */
    typedef boost::function<RequestOut(ChannelHandlerContext&,
                                       RequestIn const&)> RequestFilter;

    /**
     *
     */
    typedef boost::function<ResponseOut(ChannelHandlerContext&,
                                        RequestIn const&,
                                        ResponseIn const&,
                                        ChannelFuturePtr const&)> ResponseFilter;

public:
    ServiceFilter()
        : inboundTransfer_(),
          outboundTransfer_(),
          inboundContainer_(),
          outboundContainer_() {
    }

    ServiceFilter(const RequestFilter& requestFilter,
                  const ResponseFilter& responseFilter)
        : requestFilter_(requestFilter),
          responseFilter_(responseFilter),
          inboundTransfer_(),
          outboundTransfer_(),
          inboundContainer_(),
          outboundContainer_() {
    }

    ~ServiceFilter() {}

    void registerTo(Context& ctx) {
        inboundTransfer_ = ctx.inboundTransfer();
        inboundContainer_ = ctx.inboundContainer();

        outboundTransfer_ = ctx.outboundTransfer();
        outboundContainer_ = ctx.outboundContainer();

        ctx.setChannelMessageUpdatedCallback(boost::bind(&Self::messageUpdated,
                                             this,
                                             _1));

        ctx.setFlushFunctor(boost::bind(&Self::flush,
                                        this,
                                        _1,
                                        _2));
    }

    void setRequestFilter(const RequestFilter& filter) {
        requestFilter_ = filter;
    }

    void setResponseFilter(const ResponseFilter& filter) {
        responseFilter_ = filter;
    }

    InboundTransfer& inboundTransfer() {
        BOOST_ASSERT(inboundTransfer_);
        return *inboundTransfer_;
    }

    OutboundTransfer& outboundTransfer() {
        BOOST_ASSERT(outboundTransfer_);
        return *outboundTransfer_;
    }

private:
    void messageUpdated(ChannelHandlerContext& ctx) {
        BOOST_ASSERT(inboundContainer_ && inboundTransfer_ && "ServiceFilter has not registered.");
        BOOST_ASSERT(requestFilter_ && "HAS NOT set the request filter handler");

        bool notify = false;
        InboundQueue& queue = inboundContainer_->getMessages();

        while (!queue.empty()) {
            RequestIn& req = queue.front();
            RequestOut oreq = requestFilter_(ctx, req);

            if (!oreq) {
                LOG_WARN << "serviceFilter filterRequest has an empty result, "
                         "skip it, user handler should reply an error message if needed.";

                queue.pop_front();
                continue;
            }

            if (inboundTransfer_->unfoldAndAdd(oreq)) {
                reqs_.push_back(req);
                notify = true;
            }

            queue.pop_front();
        }

        if (notify) {
            ctx.fireMessageUpdated();
        }
    }

    void flush(ChannelHandlerContext& ctx,
               const ChannelFuturePtr& future) {
        BOOST_ASSERT(outboundContainer_ && outboundTransfer_ && "ServiceFilter has not registered.");
        BOOST_ASSERT(responseFilter_ && "HAS NOT set the request filter handler");

        OutboundQueue& queue = outboundContainer_->getMessages();

        while (!queue.empty()) {

            ResponseIn& rep = queue.front();
            ResponseOut orep = responseFilter_(ctx, reqs_.front(), rep, future);
            reqs_.pop_front();

            if (!orep) {
                LOG_ERROR << "serviceFilter filterResponse should not give an empty result";

                queue.pop_front();
                continue;
            }

            outboundTransfer_->unfoldAndAdd(orep);
            queue.pop_front();
        }

        ctx.flush(future);
    }

private:
    std::deque<RequestIn> reqs_;

    RequestFilter requestFilter_;
    ResponseFilter responseFilter_;

    InboundTransfer* inboundTransfer_;
    OutboundTransfer* outboundTransfer_;

    InboundContainer* inboundContainer_;
    OutboundContainer* outboundContainer_;
};

}
}

#endif //#if !defined(CETTY_SERVICE_SERVICEFILTER_H)

// Local Variables:
// mode: c++
// End:
