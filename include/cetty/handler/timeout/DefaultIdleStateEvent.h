#if !defined(CETTY_HANDLER_TIMEOUT_DEFAULTIDLESTATEEVENT_H)
#define CETTY_HANDLER_TIMEOUT_DEFAULTIDLESTATEEVENT_H

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

#include <string>
#include <cetty/handler/timeout/IdleState.h>
#include <cetty/handler/timeout/IdleStateEvent.h>

namespace cetty {
namespace handler {
namespace timeout {

using namespace cetty::channel;

/**
 * The default {@link IdleStateEvent} implementation.
 *
 *
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 * @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
 */
class DefaultIdleStateEvent : public IdleStateEvent {
public:
    /**
     * Creates a new instance.
     */
    DefaultIdleStateEvent(const ChannelPtr& channel,
                          const IdleState& state,
                          const time_type& lastActivityTime)
        : channel(channel),
          state(state),
          lastActivityTime(lastActivityTime) {
    }

    virtual const ChannelPtr& getChannel() const {
        return channel;
    }

    virtual const ChannelFuturePtr& getFuture() const;

    virtual const IdleState& getState() const {
        return state;
    }

    virtual const time_type& getLastActivityTime() const {
        return lastActivityTime;
    }

    virtual std::string toString() const;

private:
    const ChannelPtr&  channel;
    IdleState state;
    time_type lastActivityTime;
};

}
}
}

#endif //#if !defined(CETTY_HANDLER_TIMEOUT_DEFAULTIDLESTATEEVENT_H)

// Local Variables:
// mode: c++
// End:

