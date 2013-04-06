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
#include <cetty/logging/LoggerHelper.h>

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

    const boost::optional<int>& receiveBufferSize() const;
    void setReceiveBufferSize(int receiveBufferSize);

    const boost::optional<int>& backlog() const;
    void setBacklog(int backlog);

    const boost::optional<int>& reservedChildCount() const;
    void setReservedChildCount(int count);

private:
    template<typename Option, typename Value>
    bool applyOptionToAcceptor(const ChannelOption& key, Value value) {
        boost::system::error_code ec;
        acceptor_.set_option(Option(value), ec);

        if (ec) {
            LOG_ERROR << "failed to set " << key.name()
                << " " << value
                << " to acceptor, code: " << ec.value()
                << " message: " << ec.message();
            return false;
        }
        else {
            LOG_INFO << "has set " << key.name()
                << " " << value << " to acceptor";
            return true;
        }
    }

    template<typename Option, typename Value>
    void updateOptionFromAcceptor(const ChannelOption& key, Value* value) const {
        BOOST_ASSERT(value);

        boost::system::error_code ec;
        Option option;
        acceptor_.get_option(option, ec);

        if (!ec) {
            *value = option.value();
        }
        else {
            LOG_ERROR << "fail to get " << key.name()
                << " option from acceptor, code: " << ec.value()
                << " message: " << ec.message();
        }
    }

private:
    tcp::acceptor& acceptor_;

    mutable boost::optional<bool> reuseAddress_;
    mutable boost::optional<int>  backlog_;
    mutable boost::optional<int>  receiveBufferSize_;
    mutable boost::optional<int>  reservedChildCount_;
};

inline
const boost::optional<int>& AsioServerSocketChannelConfig::backlog() const {
    return backlog_;
}

inline
const boost::optional<int>& AsioServerSocketChannelConfig::reservedChildCount() const {
    return reservedChildCount_;
}

}
}
}


#endif //#if !defined(CETTY_CHANNEL_ASIO_TASIOSERVERSOCKETCHANNELCONFIG_H)
