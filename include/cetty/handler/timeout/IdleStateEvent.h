#if !defined(CETTY_HANDLER_TIMEOUT_IDLESTATEEVENT_H)
#define CETTY_HANDLER_TIMEOUT_IDLESTATEEVENT_H

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

#include <cetty/Types.h>
#include <cetty/handler/timeout/IdleState.h>

namespace cetty {
namespace handler {
namespace timeout {

/**
 * A {@link ChannelEvent} that is triggered when a {@link Channel} has been idle
 * for a while.
 *
 *
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 * @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
 *
 * @apiviz.landmark
 * @apiviz.has org.jboss.netty.handler.timeout.IdleState oneway - -
 */
class IdleStateEvent {
public:
    IdleStateEvent(IdleState state, int count, int64_t durationMillis);

    /**
     * Returns the detailed idle state.
     */
    const IdleState& state() const {
        return state_;
    }

    int count() const {
        return count_;
    }

    int64_t durationMillis() const {
        return durationMillis_;
    }

    std::string toString() const;

private:
    IdleState state_;
    int count_;
    int64_t durationMillis_;
};

}
}
}

#endif //#if !defined(CETTY_HANDLER_TIMEOUT_IDLESTATEEVENT_H)

// Local Variables:
// mode: c++
// End:
