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
#include <cetty/channel/ChannelMessageHandlerAdapter.h>
#include <cetty/gearman/GearmanMessagePtr.h>

namespace cetty {
namespace gearman {

using namespace cetty::channel;

class GearmanClientHandler
        : public ChannelMessageHandlerAdapter<GearmanMessagePtr, GearmanMessagePtr, GearmanMessagePtr, GearmanMessagePtr> {

public:
    GearmanClientHandler();
    virtual ~GearmanClientHandler();

    virtual void channelActive(ChannelHandlerContext& ctx);

    virtual ChannelHandlerPtr clone();

    virtual std::string toString() const;

    virtual void messageReceived(ChannelHandlerContext& ctx,
                                 const GearmanMessagePtr& msg);

    virtual void flush(ChannelHandlerContext& ctx,
                       const ChannelFuturePtr& future);

private:
    void submitJob(ChannelHandlerContext& ctx, const GearmanMessagePtr& msg);

private:
    ChannelPtr channel;
    std::deque<GearmanMessagePtr> msgs;
};

}
}

#endif //#if !defined(CETTY_GEARMAN_GEARMANCLIENTHANDLER_H)

// Local Variables:
// mode: c++
// End:
