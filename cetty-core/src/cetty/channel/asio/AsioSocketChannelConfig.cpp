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
#include <cetty/channel/asio/AsioSocketChannelConfig.h>

#include <cetty/channel/ChannelException.h>

#include <cetty/util/Exception.h>
#include <cetty/util/NestedDiagnosticContext.h>

#include <cetty/logging/LoggerHelper.h>

namespace cetty {
namespace channel {
namespace asio {

using namespace boost::asio::ip;
using namespace cetty::channel;
using namespace cetty::util;

static const int DEFAULT_SEND_BUFFER_LOW_WATERMARK  = 2 * 1024;
static const int DEFAULT_SEND_BUFFER_HIGH_WATERMARK = 2 * 1024 * 1024;

AsioSocketChannelConfig::AsioSocketChannelConfig(TcpSocket& socket)
    : socket_(socket),
      sendBufferLowWaterMark_(0),
      sendBufferHighWaterMark_(DEFAULT_SEND_BUFFER_HIGH_WATERMARK) {
}

bool AsioSocketChannelConfig::setOption(const ChannelOption& option,
                                        const ChannelOption::Variant& value) {

    if (option == ChannelOption::CO_TCP_NODELAY) {
        setTcpNoDelay(boost::get<bool>(value));
        LOG_INFO << "has set the " << option.name()
                 << " to " << *isTcpNoDelay();
    }
    else if (option == ChannelOption::CO_SO_LINGER) {
        setSoLinger(boost::get<int>(value));
        LOG_INFO << "has set the " << option.name()
                 << " to " << *soLinger();
    }
    else if (option == ChannelOption::CO_SO_RCVBUF) {
        setReceiveBufferSize(boost::get<int>(value));
        LOG_INFO << "has set the " << option.name()
                 << " to " << *receiveBufferSize();
    }
    else if (option == ChannelOption::CO_SO_SNDBUF) {
        setSendBufferSize(boost::get<int>(value));
        LOG_INFO << "has set the " << option.name()
                 << " to " << *sendBufferSize();
    }
    else if (option == ChannelOption::CO_SO_REUSEADDR) {
        setReuseAddress(boost::get<bool>(value));
        LOG_INFO << "has set the " << option.name()
                 << " to " << *isReuseAddress();
    }
    else if (option == ChannelOption::CO_SO_KEEPALIVE) {
        setKeepAlive(boost::get<bool>(value));
        LOG_INFO << "has set the " << option.name()
                 << " to " << *isKeepAlive();
    }
    else if (option == ChannelOption::CO_SO_SNDLOWAT) {
        setSendBufferLowWaterMark(boost::get<int>(value));
        LOG_INFO << "has set the " << option.name()
                 << " to " << *sendBufferLowWaterMark();
    }
    else if (option == ChannelOption::CO_SO_RCVLOWAT) {
        setReceiveBufferLowWaterMark(boost::get<int>(value));
        LOG_INFO << "has set the " << option.name()
                 << " to " << *receiveBufferLowWaterMark();
    }
    else if (option == ChannelOption::CO_SNDHIGHWAT) {
        setSendBufferHighWaterMark(boost::get<int>(value));
        LOG_INFO << "has set the " << option.name()
                 << " to " << *sendBufferHighWaterMark();
    }
    else if (option == ChannelOption::CO_RCVHIGHWAT) {
        setReceiveBufferHighWaterMark(boost::get<int>(value));
        LOG_INFO << "has set the " << option.name()
                 << " to " << *receiveBufferSize();
    }
    else {
        return false;
    }

    return true;
}

const boost::optional<int>& AsioSocketChannelConfig::receiveBufferSize() const {
    if (receiveBufferSize_) {
        return receiveBufferSize_;
    }
    else {
        if (socket_.is_open()) {
            updateOptionFromSocket<tcp::socket::receive_buffer_size, boost::optional<int> >(
                ChannelOption::CO_SO_RCVBUF, &receiveBufferSize_);
        }

        return receiveBufferSize_;
    }
}

const boost::optional<int>& AsioSocketChannelConfig::sendBufferSize() const {
    if (sendBufferSize_) {
        return sendBufferSize_;
    }
    else {
        if (socket_.is_open()) {
            updateOptionFromSocket<tcp::socket::send_buffer_size, boost::optional<int> >(
                ChannelOption::CO_SO_SNDBUF, &sendBufferSize_);
        }

        return sendBufferSize_;
    }
}

const boost::optional<int>& AsioSocketChannelConfig::soLinger() const {
    if (soLinger_) {
        return soLinger_;
    }
    else {
        if (socket_.is_open()) {
            boost::system::error_code ec;
            tcp::socket::linger option;
            socket_.get_option(option, ec);

            if (!ec) {
                soLinger_ = option.enabled() ? 0 : option.timeout();
            }
            else {
                LOG_ERROR << "fail to get SO_LINGER option from socket, code: "
                          << ec.value()
                          << " message: " << ec.message();
            }
        }

        return soLinger_;
    }
}

const boost::optional<bool>& AsioSocketChannelConfig::isKeepAlive() const {
    if (keepAlive_) {
        return keepAlive_;
    }
    else {
        if (socket_.is_open()) {
            updateOptionFromSocket<tcp::socket::keep_alive, boost::optional<bool> >(
                ChannelOption::CO_SO_KEEPALIVE, &keepAlive_);
        }

        return keepAlive_;
    }
}

const boost::optional<bool>& AsioSocketChannelConfig::isReuseAddress() const {
    if (reuseAddress_) {
        return reuseAddress_;
    }
    else {
        if (socket_.is_open()) {
            updateOptionFromSocket<tcp::socket::reuse_address, boost::optional<bool> >(
                ChannelOption::CO_SO_REUSEADDR, &reuseAddress_);
        }

        return reuseAddress_;
    }
}

const boost::optional<bool>& AsioSocketChannelConfig::isTcpNoDelay() const {
    if (tcpNoDelay_) {
        return tcpNoDelay_;
    }
    else {
        if (socket_.is_open()) {
            updateOptionFromSocket<tcp::no_delay, boost::optional<bool> >(
                ChannelOption::CO_TCP_NODELAY, &tcpNoDelay_);
        }

        return tcpNoDelay_;
    }
}

void AsioSocketChannelConfig::setKeepAlive(bool keepAlive) {
    setSocketOption<tcp::socket::keep_alive>(ChannelOption::CO_SO_KEEPALIVE,
            keepAlive,
            &keepAlive_);
}

void AsioSocketChannelConfig::setReceiveBufferSize(int receiveBufferSize) {
    setSocketOption<tcp::socket::receive_buffer_size>(ChannelOption::CO_SO_RCVBUF,
            receiveBufferSize,
            &receiveBufferSize_);
}

void AsioSocketChannelConfig::setReuseAddress(bool reuseAddress) {
    setSocketOption<tcp::socket::reuse_address>(ChannelOption::CO_SO_REUSEADDR,
            reuseAddress,
            &reuseAddress_);
}

void AsioSocketChannelConfig::setSendBufferSize(int sendBufferSize) {
    setSocketOption<tcp::socket::send_buffer_size>(ChannelOption::CO_SO_SNDBUF,
            sendBufferSize,
            &sendBufferSize_);
}

void AsioSocketChannelConfig::setSoLinger(int soLinger) {
    if (soLinger_ && *soLinger_ == soLinger) {
        LOG_WARN << "the new SO_LINGER is same with the old, need not set";
        return;
    }

    if (socket_.is_open()) {
        boost::system::error_code ec;
        tcp::socket::linger option;

        if (soLinger > 0) {
            option.enabled(true);
            option.timeout(soLinger);
        }
        else {
            option.enabled(false);
        }

        socket_.set_option(option, ec);

        if (!ec) {
            soLinger_ = soLinger;
            LOG_INFO << "has set SO_LINGER " << soLinger << " to socket";
        }
        else {
            LOG_ERROR << "failed to set SO_LINGER " << soLinger
                      << " to socket, code: " << ec.value()
                      << " message: " << ec.message();
        }
    }
}

void AsioSocketChannelConfig::setTcpNoDelay(bool tcpNoDelay) {
    setSocketOption<tcp::no_delay>(ChannelOption::CO_TCP_NODELAY,
                                   tcpNoDelay,
                                   &tcpNoDelay_);
}

void AsioSocketChannelConfig::setSendBufferLowWaterMark(int sendBufferLowWaterMark) {
    setSocketOption<tcp::socket::send_low_watermark>(ChannelOption::CO_SO_SNDLOWAT,
            sendBufferLowWaterMark,
            &sendBufferLowWaterMark_);
}

const boost::optional<int>& AsioSocketChannelConfig::sendBufferLowWaterMark() const {
    if (sendBufferLowWaterMark_) {
        return sendBufferLowWaterMark_;
    }
    else {
        if (socket_.is_open()) {
            updateOptionFromSocket<tcp::socket::send_low_watermark>(
                ChannelOption::CO_SO_SNDLOWAT,
                &sendBufferLowWaterMark_);
        }

        return sendBufferLowWaterMark_;
    }
}

void AsioSocketChannelConfig::setReceiveBufferLowWaterMark(int receiveBufferLowWaterMark) {
    setSocketOption<tcp::socket::send_low_watermark>(ChannelOption::CO_SO_RCVLOWAT,
            receiveBufferLowWaterMark,
            &receiveBufferLowWaterMark_);
}

const boost::optional<int>& AsioSocketChannelConfig::receiveBufferLowWaterMark() const {
    if (receiveBufferLowWaterMark_) {
        return receiveBufferLowWaterMark_;
    }
    else {
        if (socket_.is_open()) {
            updateOptionFromSocket<tcp::socket::receive_low_watermark>(
                ChannelOption::CO_SO_RCVLOWAT,
                &receiveBufferLowWaterMark_);
        }

        return receiveBufferLowWaterMark_;
    }
}

const boost::optional<int>& AsioSocketChannelConfig::sendBufferHighWaterMark() const {
    return sendBufferHighWaterMark_;
}

void AsioSocketChannelConfig::setSendBufferHighWaterMark(int bufferHighWaterMark) {
    if (bufferHighWaterMark > 0) {
        sendBufferHighWaterMark_ = bufferHighWaterMark;
    }
    else {
        LOG_WARN << "the SNDHIGHWAT " << bufferHighWaterMark
                 << " should not be negative";
    }
}

const boost::optional<int>& AsioSocketChannelConfig::receiveBufferHighWaterMark() const {
    return receiveBufferHighWaterMark_;
}

void AsioSocketChannelConfig::setReceiveBufferHighWaterMark(int bufferHighWaterMark) {
    if (bufferHighWaterMark > 0) {
        receiveBufferHighWaterMark_ = bufferHighWaterMark;
    }
    else {
        LOG_WARN << "the RCVHIGHWAT " << bufferHighWaterMark
                 << " should not be negative";
    }
}

}
}
}
