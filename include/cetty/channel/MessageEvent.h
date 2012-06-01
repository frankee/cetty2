#if !defined(CETTY_CHANNEL_MESSAGEEVENT_H)
#define CETTY_CHANNEL_MESSAGEEVENT_H

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

class ChannelMessage;
class SocketAddress;

/**
 * A {@link ChannelEvent} which represents the transmission or reception of a
 * message.  It can mean the notification of a received message or the request
 * for writing a message, depending on whether it is an upstream event or a
 * downstream event respectively.  Please refer to the {@link ChannelEvent}
 * documentation to find out what an upstream event and a downstream event are
 * and what fundamental differences they have.
 *
 *
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 * @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
 */

class MessageEvent : public ChannelEvent {
public:
    virtual ~MessageEvent() {}

    /**
     * Returns the message.
     */
    virtual const ChannelMessage& getMessage() const = 0;

    /**
     * Returns the remote address of the message.
     */
    virtual const SocketAddress& getRemoteAddress() const = 0;
};

}
}


#endif //#if !defined(CETTY_CHANNEL_MESSAGEEVENT_H)

// Local Variables:
// mode: c++
// End:

