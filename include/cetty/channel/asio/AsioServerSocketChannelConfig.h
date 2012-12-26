#if !defined(CETTY_CHANNEL_ASIO_TASIOSERVERSOCKETCHANNELCONFIG_H)
#define CETTY_CHANNEL_ASIO_TASIOSERVERSOCKETCHANNELCONFIG_H

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
#include <boost/noncopyable.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <cetty/channel/ChannelOption.h>

namespace cetty {
namespace channel {
namespace asio {

using namespace boost::asio::ip;

class AsioServerSocketChannelConfig : private boost::noncopyable {
public:
    /**
     * Creates a new instance.
     */
    AsioServerSocketChannelConfig(boost::asio::ip::tcp::acceptor& acceptor);
    ~AsioServerSocketChannelConfig();

    bool setOption(const ChannelOption& option,
                           const ChannelOption::Variant& value);

    const boost::optional<bool>& isReuseAddress() const;
    void setReuseAddress(bool reuseAddress);

    const boost::optional<int>& getReceiveBufferSize() const;
    void setReceiveBufferSize(int receiveBufferSize);

    void setPerformancePreferences(int connectionTime, int latency, int bandwidth);

    const boost::optional<int>& getBacklog() const;
    void setBacklog(int backlog);

    bool isReuseAddress(tcp::acceptor& acceptor) const;
    void setReuseAddress(tcp::acceptor& acceptor);

    int getReceiveBufferSize(tcp::acceptor& acceptor) const;
    void setReceiveBufferSize(tcp::acceptor& acceptor);

private:
    tcp::acceptor& acceptor_;

    mutable boost::optional<bool> reuseAddress_;
    mutable boost::optional<int>  receiveBufferSize_;
    mutable boost::optional<int>  backlog_;
};

}
}
}


#endif //#if !defined(CETTY_CHANNEL_ASIO_TASIOSERVERSOCKETCHANNELCONFIG_H)
