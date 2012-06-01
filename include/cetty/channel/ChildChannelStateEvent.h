#if !defined(CETTY_CHANNEL_CHILDCHANNELSTATEEVENT_H)
#define CETTY_CHANNEL_CHILDCHANNELSTATEEVENT_H

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

#include <cetty/channel/ChannelEvent.h>

namespace cetty {
namespace channel {

/**
 * A {@link ChannelEvent} which represents the notification of the state of
 * a child {@link Channel}.  This event is for going upstream only.  Please
 * refer to the {@link ChannelEvent} documentation to find out what an upstream
 * event and a downstream event are and what fundamental differences they have.
 *
 * @remark
 * {@link getChannel() } Returns the <strong>parent</strong> {@link Channel} which is associated
 * with this event.  Please note that you should use {@link #getChildChannel()}
 * to get the {@link Channel} created or accepted by the parent {@link Channel}.
 *
 *
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 * @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
 */

class ChildChannelStateEvent : public ChannelEvent {
public:
    virtual ~ChildChannelStateEvent() {}

    /**
     * Returns the child {@link Channel} whose state has been changed.
     */
    virtual const ChannelPtr& getChildChannel() const = 0;
};

}
}

#endif //#if !defined(CETTY_CHANNEL_CHILDCHANNELSTATEEVENT_H)

// Local Variables:
// mode: c++
// End:

