#if !defined(CETTY_SERVICE_CLIENTSERVICEFILTER_H)
#define CETTY_SERVICE_CLIENTSERVICEFILTER_H

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
#include <cetty/channel/ChannelMessageHandler.h>
#include <cetty/handler/codec/CodecUtil.h>

namespace cetty {
namespace service {

using namespace cetty::channel;
using namespace cetty::handler::codec;

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
class ClientServiceFilter
        : public cetty::channel::ChannelMessageHandler<ResponseInT, RequestInT> {
public:
    virtual ~ClientServiceFilter() {}

protected:
    virtual void messageUpdated(InboundMessageContext& ctx) {
        InboundMessageContext::MessageQueue& in =
            ctx.getInboundMessageQueue();

        bool notify = false;

        while (!in.empty()) {
            ResponseInT& msg = in.front();
            ResponseOutT omsg = filterResponse(ctx, reqs.front(), msg);
            reqs.pop_front();

            if (CodecUtil<ResponseOutT>::unfoldAndAdd(ctx, omsg, true)) {
                notify = true;
            }
        }

        if (notify) {
            ctx.fireMessageUpdated();
        }
    }

    virtual void flush(OutboundMessageContext& ctx,
                       const ChannelFuturePtr& future) {
        OutboundMessageContext::MessageQueue& in =
            ctx.getOutboundMessageQueue();

        while (!in.empty()) {
            RequestInT& req = in.front();

            reqs.push_back(req);
            RequestOutT oreq = filterRequest(ctx, req);
            CodecUtil<RequestOutT>::unfoldAndAdd(ctx, oreq, false);

            in.pop_front();
        }

        ctx.flush(future);
    }

    virtual RequestOutT filterRequest(OutboundMessageContext& ctx,
                                      const RequestInT& req) = 0;

    virtual ResponseOutT filterResponse(InboundMessageContext& ctx,
                                        const RequestInT& req,
                                        const ResponseInT& rep) = 0;

private:
    std::deque<RequestInT> reqs;
};

}
}

#endif //#if !defined(CETTY_SERVICE_CLIENTSERVICEFILTER_H)

// Local Variables:
// mode: c++
// End:
