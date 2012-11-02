#if !defined(CETTY_CHANNEL_SOCKET_ASIO_TASIOSERVERSOCKETCHANNELCONFIG_H)
#define CETTY_CHANNEL_SOCKET_ASIO_TASIOSERVERSOCKETCHANNELCONFIG_H

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

#include <boost/optional.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <cetty/channel/DefaultChannelConfig.h>
#include <cetty/channel/socket/ServerSocketChannelConfig.h>

namespace cetty {
namespace channel {
namespace socket {
namespace asio {

using namespace boost::asio::ip;

class AsioServerSocketChannelConfig
    : public cetty::channel::DefaultChannelConfig,
      public cetty::channel::socket::ServerSocketChannelConfig {

public:
    /**
     * Creates a new instance.
     */
    AsioServerSocketChannelConfig(boost::asio::ip::tcp::acceptor& acceptor);
    virtual ~AsioServerSocketChannelConfig();

    virtual bool setOption(const ChannelOption& option,
                           const ChannelOption::Variant& value);

    virtual const boost::optional<bool>& isReuseAddress() const;
    virtual void setReuseAddress(bool reuseAddress);

    virtual const boost::optional<int>& getReceiveBufferSize() const;
    virtual void setReceiveBufferSize(int receiveBufferSize);

    virtual void setPerformancePreferences(int connectionTime, int latency, int bandwidth);

    virtual const boost::optional<int>& getBacklog() const;
    virtual void setBacklog(int backlog);

    bool isReuseAddress(tcp::acceptor& acceptor) const;
    void setReuseAddress(tcp::acceptor& acceptor);

    int getReceiveBufferSize(tcp::acceptor& acceptor) const;
    void setReceiveBufferSize(tcp::acceptor& acceptor);

private:
    mutable boost::optional<bool> reuseAddress;
    mutable boost::optional<int>  receiveBufferSize;
    mutable boost::optional<int>  backlog;

    tcp::acceptor& acceptor;
};

}
}
}
}


#endif //#if !defined(CETTY_CHANNEL_SOCKET_ASIO_TASIOSERVERSOCKETCHANNELCONFIG_H)
