#if !defined(CETTY_CHANNEL_SOCKET_SERVERSOCKETCHANNELCONFIG_H)
#define CETTY_CHANNEL_SOCKET_SERVERSOCKETCHANNELCONFIG_H

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

#include <cetty/channel/ChannelConfig.h>

namespace cetty {
namespace channel  {
namespace socket {

/**
 * A {@link ChannelConfig} for a {@link ServerSocketChannel}.
 *
 * <h3>Available options</h3>
 *
 * In addition to the options provided by {@link ChannelConfig},
 * {@link ServerSocketChannelConfig} allows the following options in the
 * option map:
 *
 * <table border="1" cellspacing="0" cellpadding="6">
 * <tr>
 * <th>Name</th><th>Associated setter method</th>
 * </tr><tr>
 * <td><tt>"backlog"</tt></td><td>{@link #setBacklog(int)}</td>
 * </tr><tr>
 * <td><tt>"reuseAddress"</tt></td><td>{@link #setReuseAddress(bool)}</td>
 * </tr><tr>
 * <td><tt>"receiveBufferSize"</tt></td><td>{@link #setReceiveBufferSize(int)}</td>
 * </tr>
 * </table>
 *
 *
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 *
 * @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
 */

class ServerSocketChannelConfig {
public:
    virtual ~ServerSocketChannelConfig() {}

    /**
     * Gets the backlog value to specify when the channel binds to a local
     * address.
     */
    virtual const boost::optional<int>& getBacklog() const = 0;

    /**
     * Sets the backlog value to specify when the channel binds to a local
     * address.
     */
    virtual void setBacklog(int backlog) = 0;

    /**
     * Gets the <a ><tt>SO_REUSEADDR</tt></a> option.
     */
    virtual const boost::optional<bool>& isReuseAddress() const = 0;

    /**
     * Sets the <a><tt>SO_REUSEADDR</tt></a> option.
     */
    virtual void setReuseAddress(bool reuseAddress) = 0;

    /**
     * Gets the <a><tt>SO_RCVBUF</tt></a> option.
     */
    virtual const boost::optional<int>& getReceiveBufferSize() const = 0;

    /**
     * Sets the <a><tt>SO_RCVBUF</tt></a> option.
     */
    virtual void setReceiveBufferSize(int receiveBufferSize) = 0;

    /**
     * Sets the performance preferences as specified in
     * {@link ServerSocket#setPerformancePreferences(int, int, int)}.
     */
    virtual void setPerformancePreferences(int connectionTime, int latency, int bandwidth) = 0;
};

}
}
}


#endif //#if !defined(CETTY_CHANNEL_SOCKET_SERVERSOCKETCHANNELCONFIG_H)

// Local Variables:
// mode: c++
// End:
