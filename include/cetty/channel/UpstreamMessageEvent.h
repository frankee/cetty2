#if !defined(CETTY_CHANNEL_UPSTREAMMESSAGEEVENT_H)
#define CETTY_CHANNEL_UPSTREAMMESSAGEEVENT_H

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

#include <cetty/channel/MessageEvent.h>

namespace cetty {
namespace channel {

class ChannelMessage;

/**
 * The default upstream {@link MessageEvent} implementation.
 *
 *
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 * @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
 *
 */

class UpstreamMessageEvent : public MessageEvent {
public:
    virtual ~UpstreamMessageEvent() {}

    /**
     * Creates a new instance.
     */
    UpstreamMessageEvent(const ChannelPtr& channel,
                         const ChannelMessage& message,
                         const SocketAddress& remoteAddress);

    virtual const ChannelPtr& getChannel() const;
    virtual const ChannelFuturePtr& getFuture() const;
    virtual const ChannelMessage& getMessage() const;
    virtual const SocketAddress& getRemoteAddress() const;

    virtual std::string toString() const;

//protected:
    UpstreamMessageEvent(const UpstreamMessageEvent&);
    UpstreamMessageEvent& operator=(const UpstreamMessageEvent&);

protected:
    const ChannelPtr&     channel;
    const ChannelMessage& message;
    const SocketAddress&  remoteAddress;
};

}
}

#endif //#if !defined(CETTY_CHANNEL_UPSTREAMMESSAGEEVENT_H)

// Local Variables:
// mode: c++
// End:

