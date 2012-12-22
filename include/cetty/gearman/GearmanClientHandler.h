#if !defined(CETTY_GEARMAN_GEARMANCLIENTHANDLER_H)
#define CETTY_GEARMAN_GEARMANCLIENTHANDLER_H

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

#include <cetty/channel/Channel.h>
#include <cetty/channel/ChannelMessageHandler.h>
#include <cetty/gearman/protocol/GearmanMessagePtr.h>

namespace cetty {
namespace gearman {

using namespace cetty::channel;
using namespace cetty::gearman::protocol;

class GearmanClientHandler : private boost::noncopyable {
public:
    typedef ChannelMessageHandler<GearmanClientHandler,
            GearmanMessagePtr,
            GearmanMessagePtr,
            GearmanMessagePtr,
            GearmanMessagePtr> MessageHandler;

    typedef MessageHandler::Context Context;

    typedef MessageHandler::InboundContainer InboundContainer;
    typedef MessageHandler::OutboundContainer OutboundContainer;

    typedef MessageHandler::InboundQueue InboundQueue;
    typedef MessageHandler::OutboundQueue OutboundQueue;

    typedef MessageHandler::InboundTransfer InboundTransfer;
    typedef MessageHandler::OutboundTransfer OutboundTransfer;

    typedef MessageHandler::Handler Handler;
    typedef MessageHandler::HandlerPtr HandlerPtr;

public:
    GearmanClientHandler();
    ~GearmanClientHandler();

    void registerTo(Context& ctx);

private:
    void messageReceived(ChannelHandlerContext& ctx);

    void flush(ChannelHandlerContext& ctx,
               const ChannelFuturePtr& future);

private:
    void submitJob(ChannelHandlerContext& ctx, const GearmanMessagePtr& msg);

private:
    InboundTransfer* inboundTransfer_;
    InboundContainer* inboundContainer_;

    OutboundTransfer* outboundTransfer_;
    OutboundContainer* outboundContainer_;
};

}
}

#endif //#if !defined(CETTY_GEARMAN_GEARMANCLIENTHANDLER_H)

// Local Variables:
// mode: c++
// End:
