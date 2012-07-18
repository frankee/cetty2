#if !defined(CETTY_HANDLER_TIMEOUT_IDLESTATEAWARECHANNELHANDLER_H)
#define CETTY_HANDLER_TIMEOUT_IDLESTATEAWARECHANNELHANDLER_H

/*
 * Copyright 2009 Red Hat, Inc.
 *
 * Red Hat licenses this file to you under the Apache License, version 2.0
 * (the "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at:
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 */
/*
 * Copyright (c) 2010-2011 frankee zhou (frankee.zhou at gmail dot com)
 * Distributed under under the Apache License, version 2.0 (the "License").
 */

#include <cetty/channel/SimpleChannelHandler.h>

namespace cetty {
namespace handler {
namespace timeout {

using namespace cetty::channel;

class IdleStateEvent;

/**
 * An extended {@link SimpleChannelHandler} that adds the handler method for
 * an {@link IdleStateEvent}.
 *
 *
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 * @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
 *
 * @apiviz.uses org.jboss.netty.handler.timeout.IdleStateEvent
 */

class IdleStateAwareChannelHandler : public cetty::channel::SimpleChannelHandler {
public:
    /**
     * Creates a new instance.
     */
    IdleStateAwareChannelHandler() {}
    virtual ~IdleStateAwareChannelHandler() {}

    virtual void handleUpstream(ChannelHandlerContext& ctx, const ChannelEvent& e);

    /**
     * Invoked when a {@link Channel} has been idle for a while.
     */
    virtual void channelIdle(ChannelHandlerContext& ctx, const IdleStateEvent& e);

    virtual ChannelHandlerPtr clone() {
        return shared_from_this();
    }

    virtual std::string toString() const {
        return std::string("IdleStateAwareChannelHandler");
    }
};

}
}
}

#endif //#if !defined(CETTY_HANDLER_TIMEOUT_IDLESTATEAWARECHANNELHANDLER_H)

// Local Variables:
// mode: c++
// End:
