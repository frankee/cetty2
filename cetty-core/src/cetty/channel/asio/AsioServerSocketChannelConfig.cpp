/*
 * Copyright (c) 2010-2011 frankee zhou (frankee.zhou at gmail dot com)
 *
 * Distributed under under the Apache License, version 2.0 (the "License").
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 */

#include <cetty/channel/asio/AsioServerSocketChannelConfig.h>

#include <boost/any.hpp>
#include <boost/asio.hpp>
#include <boost/system/system_error.hpp>
#include <cetty/channel/ChannelException.h>
#include <cetty/util/Exception.h>
#include <cetty/util/StringUtil.h>
#include <cetty/logging/LoggerHelper.h>

namespace cetty {
namespace channel {
namespace asio {

using namespace boost::asio::ip;
using namespace cetty::util;

AsioServerSocketChannelConfig::AsioServerSocketChannelConfig(
    tcp::acceptor& acceptor)
    : acceptor_(acceptor) {
}

AsioServerSocketChannelConfig::~AsioServerSocketChannelConfig() {
}

bool AsioServerSocketChannelConfig::setOption(const ChannelOption& option,
        const ChannelOption::Variant& value) {
    if (option == ChannelOption::CO_SO_RCVBUF) {
        setReceiveBufferSize(boost::get<int>(value));
    }
    else if (option == ChannelOption::CO_SO_REUSEADDR) {
        setReuseAddress(boost::get<bool>(value));
    }
    else if (option == ChannelOption::CO_SO_BACKLOG) {
        setBacklog(boost::get<int>(value));
    }
    else if (option == ChannelOption::CO_RESERVED_CHILD_COUNT) {
        setReservedChildCount(boost::get<int>(value));
    }
    else {
        return false;
    }

    return true;
}

const boost::optional<bool>&
AsioServerSocketChannelConfig::isReuseAddress() const {
    if (reuseAddress_) {
        return reuseAddress_;
    }

    if (acceptor_.is_open()) {
        updateOptionFromAcceptor<tcp::acceptor::reuse_address, boost::optional<bool> >(
            ChannelOption::CO_SO_REUSEADDR, &reuseAddress_);
    }

    return reuseAddress_;
}

void AsioServerSocketChannelConfig::setReuseAddress(bool reuseAddress) {
    if (reuseAddress_ && *reuseAddress_ == reuseAddress) {
        LOG_WARN << "the new ReuseAddress option is same with the old, need not set";
        return;
    }

    if (acceptor_.is_open() &&
            applyOptionToAcceptor<tcp::acceptor::reuse_address, bool>(
                ChannelOption::CO_SO_REUSEADDR,
                reuseAddress)) {
        reuseAddress_ = reuseAddress;
    }
}

const boost::optional<int>&
AsioServerSocketChannelConfig::receiveBufferSize() const {
    if (receiveBufferSize_) {
        return receiveBufferSize_;
    }

    if (acceptor_.is_open()) {
        updateOptionFromAcceptor<tcp::acceptor::receive_buffer_size, boost::optional<int> >(
            ChannelOption::CO_SO_RCVBUF, &receiveBufferSize_);
    }

    return receiveBufferSize_;
}

void AsioServerSocketChannelConfig::setReceiveBufferSize(int receiveBufferSize) {
    if (receiveBufferSize_ && *receiveBufferSize_ == receiveBufferSize) {
        LOG_WARN << "the new receiveBufferSize is same with the old, need not set";
        return;
    }

    if (receiveBufferSize > 0) {
        if (acceptor_.is_open() &&
                applyOptionToAcceptor<tcp::acceptor::receive_buffer_size>(
                    ChannelOption::CO_SO_RCVBUF,
                    receiveBufferSize)) {
            receiveBufferSize_ = receiveBufferSize;
        }
    }
    else {
        LOG_WARN << "the receiveBufferSize " << receiveBufferSize
                 << " should not be negative";
    }
}

void AsioServerSocketChannelConfig::setBacklog(int backlog) {
    if (backlog > 0) {
        backlog_ = backlog;
        LOG_INFO << "has set the backlog to " << backlog;
    }
    else {
        LOG_WARN << "backlog " << backlog
                 << " should not less then zero, will not set";
    }
}

void AsioServerSocketChannelConfig::setReservedChildCount(int count) {
    if (count > 0) {
        reservedChildCount_ = count;
        LOG_INFO << "has set the reservedChildCount to " << count;
    }
    else {
        LOG_WARN << "reservedChildCount " << count
                 << " should not be negative, will not set";
    }
}

}
}
}
