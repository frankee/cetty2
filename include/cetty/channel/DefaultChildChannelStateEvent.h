#if !defined(CETTY_CHANNEL_DEFAULTCHILDCHANNELSTATEEVENT_H)
#define CETTY_CHANNEL_DEFAULTCHILDCHANNELSTATEEVENT_H

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

#include <cetty/channel/Channel.h>
#include <cetty/channel/ChildChannelStateEvent.h>
#include <cetty/channel/Channels.h>

namespace cetty {
namespace channel {

/**
 * The default {@link ChildChannelStateEvent} implementation.
 *
 *
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 * @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
 *
 */

class DefaultChildChannelStateEvent : public ChildChannelStateEvent {
public:
    virtual ~DefaultChildChannelStateEvent() {}

    /**
     * Creates a new instance.
     */
    DefaultChildChannelStateEvent(const ChannelPtr& parentChannel, const ChannelPtr& childChannel)
        : parentChannel(parentChannel),
          childChannel(childChannel) {
    }

    const ChannelPtr& getChannel() const {
        return this->parentChannel;
    }

    const ChannelFuturePtr& getFuture() const {
        return this->parentChannel->getSucceededFuture();
    }

    const ChannelPtr& getChildChannel() const {
        return this->childChannel;
    }

    std::string toString() const {
        std::string str = parentChannel->toString();
        str.append(childChannel->isOpen() ?
                   " CHILD_OPEN: " : " CHILD_CLOSED: ");
        //str.append(childChannel->getId().toString());

        return str;
    }

private:
    const ChannelPtr& parentChannel;
    const ChannelPtr& childChannel;
};

}
}

#endif //#if !defined(CETTY_CHANNEL_DEFAULTCHILDCHANNELSTATEEVENT_H)

// Local Variables:
// mode: c++
// End:
