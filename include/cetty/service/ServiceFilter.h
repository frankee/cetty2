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
#include <cetty/channel/ChannelMessageHandlerAdapter.h>
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

template<typename RequestInT,
         typename RequestOutT,
         typename ResponseInT,
         typename ResponseOutT>
class ServiceFilter
        : public ChannelMessageHandlerAdapter<RequestInT, RequestOutT, ResponseInT, ResponseOutT> {
public:
    using ChannelMessageHandlerAdapter<RequestInT, RequestOutT, ResponseInT, ResponseOutT>::inboundTransfer;
    using ChannelMessageHandlerAdapter<RequestInT, RequestOutT, ResponseInT, ResponseOutT>::outboundTransfer;

    using ChannelInboundMessageHandler<RequestInT>::inboundQueue;
    using ChannelOutboundMessageHandler<ResponseInT>::outboundQueue;

public:
    virtual ~ServiceFilter() {}

protected:
    virtual void messageUpdated(ChannelHandlerContext& ctx) {
        bool notify = false;

        while (!inboundQueue.empty()) {
            RequestInT& req = inboundQueue.front();
            reqs.push_back(req);
            RequestOutT oreq = filterRequest(ctx, req);

            if (!oreq) {
                LOG_WARN << "serviceFilter filterRequest has an empty result, "
                         "skip it, user handler should reply an error message if needed.";

                inboundQueue.pop_front();
                continue;
            }

            if (inboundTransfer.unfoldAndAdd(oreq)) {
                notify = true;
            }

            inboundQueue.pop_front();
        }

        if (notify) {
            ctx.fireMessageUpdated();
        }
    }

    virtual void flush(ChannelHandlerContext& ctx,
                       const ChannelFuturePtr& future) {
        while (!outboundQueue.empty()) {
            ResponseInT& rep = outboundQueue.front();
            ResponseOutT orep = filterResponse(ctx, reqs.front(), rep, future);
            reqs.pop_front();

            if (!orep) {
                LOG_ERROR << "serviceFilter filterResponse has an empty result, "
                          "skip it, user handler should replace an error message if needed.";

                outboundQueue.pop_front();
                continue;
            }

            outboundTransfer.unfoldAndAdd(orep);
            outboundQueue.pop_front();
        }

        ctx.flush(future);
    }

    virtual RequestOutT filterRequest(ChannelHandlerContext& ctx,
                                      const RequestInT& req) = 0;

    virtual ResponseOutT filterResponse(ChannelHandlerContext& ctx,
                                        const RequestInT& req,
                                        const ResponseInT& rep,
                                        const ChannelFuturePtr& future) = 0;

private:
    std::deque<RequestInT>reqs;
};

}
}

#endif //#if !defined(CETTY_SERVICE_SERVICEFILTER_H)

// Local Variables:
// mode: c++
// End:
