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

#include <boost/date_time/posix_time/ptime.hpp>
#include <cetty/channel/ChannelEvent.h>

namespace cetty {
namespace handler {
namespace timeout {

class IdleState;

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
class IdleStateEvent : public cetty::channel::ChannelEvent {
public:
    typedef boost::posix_time::ptime time_type;

public:
    virtual ~IdleStateEvent() {}

    /**
     * Returns the detailed idle state.
     */
    virtual const IdleState& getState() const = 0;

    /**
     * Returns the last time when I/O occurred.
     */
    virtual const time_type& getLastActivityTime() const = 0;
};

}
}
}

#endif //#if !defined(CETTY_HANDLER_TIMEOUT_IDLESTATEEVENT_H)

// Local Variables:
// mode: c++
// End:

