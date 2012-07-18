#if !defined(CETTY_CHANNEL_CHANNELSTATE_H)
#define CETTY_CHANNEL_CHANNELSTATE_H

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
#include <cetty/util/Enum.h>

namespace cetty {
namespace channel {

/**
 * The current or future state of a {@link Channel}.
 * <p>
 * The state of a {@link Channel} is interpreted differently depending on the
 * @link ChannelStateEvent#getValue() value} of a {@link ChannelStateEvent@endlink
 * and the direction of the event in a {@link ChannelPipeline}:
 *
 * <table border="1" cellspacing="0" cellpadding="6">
 * <tr>
 * <th>Direction</th><th>State</th><th>Value</th><th>Meaning</th>
 * </tr>
 * <tr>
 * <td>Upstream</td><td>{@link #OPEN}</td><td><tt>true</tt></td><td>The channel is open.</td>
 * </tr>
 * <tr>
 * <td>Upstream</td><td>{@link #OPEN}</td><td><tt>false</tt></td><td>The channel is closed.</td>
 * </tr>
 * <tr>
 * <td>Upstream</td><td>{@link #BOUND}</td><td>{@link SocketAddress}</td><td>The channel is bound to a local address.</td>
 * </tr>
 * <tr>
 * <td>Upstream</td><td>{@link #BOUND}</td><td><tt>NULL</tt></td><td>The channel is unbound to a local address.</td>
 * </tr>
 * <tr>
 * <td>Upstream</td><td>{@link #CONNECTED}</td><td>{@link SocketAddress}</td><td>The channel is connected to a remote address.</td>
 * </tr>
 * <tr>
 * <td>Upstream</td><td>{@link #CONNECTED}</td><td><tt>NULL</tt></td><td>The channel is disconnected from a remote address.</td>
 * </tr>
 * <tr>
 * <td>Upstream</td><td>{@link #INTEREST_OPS}</td><td>an integer</td><td>The channel interestOps has been changed.</td>
 * </tr>
 * <tr>
 * <td>Downstream</td><td>{@link #OPEN}</td><td><tt>true</tt></td><td>N/A</td>
 * </tr>
 * <tr>
 * <td>Downstream</td><td>{@link #OPEN}</td><td><tt>false</tt></td><td>Close the channel.</td>
 * </tr>
 * <tr>
 * <td>Downstream</td><td>{@link #BOUND}</td><td>{@link SocketAddress}</td><td>Bind the channel to the specified local address.</td>
 * </tr>
 * <tr>
 * <td>Downstream</td><td>{@link #BOUND}</td><td><tt>NULL</tt></td><td>Unbind the channel from the current local address.</td>
 * </tr>
 * <tr>
 * <td>Downstream</td><td>{@link #CONNECTED}</td><td>{@link SocketAddress}</td><td>Connect the channel to the specified remote address.</td>
 * </tr>
 * <tr>
 * <td>Downstream</td><td>{@link #CONNECTED}</td><td><tt>NULL</tt></td><td>Disconnect the channel from the current remote address.</td>
 * </tr>
 * <tr>
 * <td>Downstream</td><td>{@link #INTEREST_OPS}</td><td>an integer</td><td>Change the interestOps of the channel.</td>
 * </tr>
 * </table>
 * <p>
 * To see how an event is interpreted further, please refer to {@link ChannelEvent}.
 *
 *
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 *
 * @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
 */

class ChannelState : public cetty::util::Enum<ChannelState> {
public:
    /**
     * Represents a {@link Channel}'s {@link Channel#isOpen() open} property
     */
    static const ChannelState OPEN;

    /**
     * Represents a {@link Channel}'s {@link Channel#isBound() bound} property
     */
    static const ChannelState BOUND;

    /**
     * Represents a {@link Channel}'s {@link Channel#isConnected() connected}
     * property
     */
    static const ChannelState CONNECTED;

    /**
     * Represents a {@link Channel}'s {@link Channel#getInterestOps() interestOps}
     * property
     */
    static const ChannelState INTEREST_OPS;

    /**
     * Constructs a string describing this enum.
     * This method returns the string
     * "OPEN" for OPEN, "BOUND" for BOUND, "CONNECTED" for CONNECTED and
     * "INTEREST_OPS" for INTEREST_OPS.
     *
     * @return The specified string
     */
    std::string toString() const;

private:
    ChannelState(int value) : cetty::util::Enum<ChannelState>(value) {}
};

}
}

#endif //#if !defined(CETTY_CHANNEL_CHANNELSTATE_H)

// Local Variables:
// mode: c++
// End:
