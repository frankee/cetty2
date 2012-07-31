#if !defined(CETTY_SERVICE_FILTER_H)
#define CETTY_SERVICE_FILTER_H

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

#include <queue>

#include <cetty/channel/ChannelHandlerContext.h>
#include <cetty/channel/SimpleChannelHandler.h>
#include <cetty/channel/ChannelMessage.h>
#include <cetty/channel/MessageEvent.h>
#include <cetty/channel/UpstreamMessageEvent.h>
#include <cetty/channel/DownstreamMessageEvent.h>

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

template<typename ReqInT, typename RepOutT, typename ReqOutT = ReqInT, typename RepInT = RepOutT>
class ClientFilter : public cetty::channel::SimpleChannelHandler {
public:
    virtual ~ClientFilter() {}

    virtual void messageReceived(ChannelHandlerContext& ctx,
                                 const MessageEvent& e) {
        ChannelMessage msg = e.getMessage();
        RepInT rep = msg.value<RepInT>();

        ReqInT req = reqs.front();
        ChannelMessage repMsg(filterRep(req,rep));
        reqs.pop();

        UpstreamMessageEvent evt(e.getChannel(),
                                 repMsg,e.getRemoteAddress());
        ctx.sendUpstream(evt);
    }

    virtual void writeRequested(ChannelHandlerContext& ctx,
                                const MessageEvent& e) {
        ChannelMessage msg = e.getMessage();
        ReqInT req = msg.value<ReqInT>();
        reqs.push(req);

        ChannelMessage reqMsg(filterReq(req));

        DownstreamMessageEvent evt(e.getChannel(),e.getFuture(),
                                   reqMsg,e.getRemoteAddress());
        ctx.sendDownstream(evt);
    }
protected:
    virtual ReqOutT filterReq(const ReqInT& req) = 0;
    virtual RepOutT filterRep(const ReqInT& req, const RepInT& rep) = 0;

private:
    std::queue<ReqInT>reqs;
};

}
}

#endif //#if !defined(CETTY_SERVICE_FILTER_H)

// Local Variables:
// mode: c++
// End:
