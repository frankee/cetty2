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
#include <cetty/channel/ChannelPipelineFactory.h>

#include <cetty/util/Exception.h>
#include <cetty/util/NestedDiagnosticContext.h>

#include <cetty/logging/LoggerHelper.h>

namespace cetty {
namespace channel {
namespace asio {

using namespace cetty::channel;
using namespace cetty::util;

AsioSocketChannelConfig::AsioSocketChannelConfig(TcpSocket& socket)
    : socket(socket),
      writeBufferLowWaterMark_(0),
      writeBufferHighWaterMark_(DEFAULT_WRITE_BUFFER_HIGH_WATERMARK) {
}

bool AsioSocketChannelConfig::setOption(const ChannelOption& option,
                                        const ChannelOption::Variant& value) {
    if (DefaultChannelConfig::setOption(option, value)) {
        return true;
    }

    if (option == ChannelOption::CO_SO_RCVBUF) {
        setReceiveBufferSize(boost::get<int>(value));
    }
    else if (option == ChannelOption::CO_SO_SNDBUF) {
        setSendBufferSize(boost::get<int>(value));
    }
    else if (option == ChannelOption::CO_TCP_NODELAY) {
        setTcpNoDelay(boost::get<bool>(value));
    }
    else if (option == ChannelOption::CO_SO_KEEPALIVE) {
        setKeepAlive(boost::get<bool>(value));
    }
    else if (option == ChannelOption::CO_SO_REUSEADDR) {
        setReuseAddress(boost::get<bool>(value));
    }
    else if (option == ChannelOption::CO_SO_LINGER) {
        setSoLinger(boost::get<int>(value));
    }

#if 0
    else if (key == "writeBufferLowWaterMark") {
        setWriteBufferLowWaterMark(ConversionUtil::toInt(value));
    }
    else if (key == "receiveBufferLowWaterMark") {
        setReceiveBufferLowWaterMark(ConversionUtil::toInt(value));
    }

#endif
    else {
        return false;
    }

    return true;
}

const boost::optional<int>& AsioSocketChannelConfig::getReceiveBufferSize() const {
    if (receiveBufferSize_) {
        return receiveBufferSize_;
    }

    try {
        boost::asio::ip::tcp::socket::receive_buffer_size option;
        this->socket.get_option(option);
        this->receiveBufferSize_ = option.value();
    }
    catch (const boost::system::system_error& e) {
        throw ChannelException(e.what(), e.code().value());
    }

    return receiveBufferSize_;
}

const boost::optional<int>& AsioSocketChannelConfig::getSendBufferSize() const {
    if (sendBufferSize_) {
        return sendBufferSize_;
    }

    try {
        boost::asio::ip::tcp::socket::send_buffer_size option;
        this->socket.get_option(option);
        sendBufferSize_ = option.value();
    }
    catch (const boost::system::system_error& e) {
        throw ChannelException(e.what(), e.code().value());
    }

    return sendBufferSize_;
}

const boost::optional<int>& AsioSocketChannelConfig::getSoLinger() const {
    if (soLinger_) {
        return soLinger_;
    }

    try {
        boost::asio::ip::tcp::socket::linger option;
        this->socket.get_option(option);
        this->soLinger_ = option.enabled() ? 0 : option.timeout();
    }
    catch (const boost::system::system_error& e) {
        throw ChannelException(e.what(), e.code().value());
    }

    return soLinger_;
}

const boost::optional<bool>& AsioSocketChannelConfig::isKeepAlive() const {
    if (this->isKeepAlive()) {
        return this->isKeepAlive();
    }

    try {
        boost::asio::ip::tcp::socket::keep_alive option;
        this->socket.get_option(option);
        this->keepAlive_ = option.value();
    }
    catch (const boost::system::system_error& e) {
        throw ChannelException(e.what(), e.code().value());
    }

    return this->keepAlive_;
}

const boost::optional<bool>& AsioSocketChannelConfig::isReuseAddress() const {
    if (this->reuseAddress_) {
        return this->reuseAddress_;
    }

    try {
        boost::asio::ip::tcp::socket::reuse_address option;
        this->socket.get_option(option);
        this->reuseAddress_ = option.value();
    }
    catch (const boost::system::system_error& e) {
        throw ChannelException(e.what(), e.code().value());
    }

    return this->reuseAddress_;
}

const boost::optional<bool>& AsioSocketChannelConfig::isTcpNoDelay() const {
    if (tcpNoDelay_) {
        return tcpNoDelay_;
    }

    try {
        boost::asio::ip::tcp::no_delay option;
        this->socket.get_option(option);
        tcpNoDelay_ = option.value();
    }
    catch (const boost::system::system_error& e) {
        throw ChannelException(e.what(), e.code().value());
    }

    return tcpNoDelay_;
}

void AsioSocketChannelConfig::setKeepAlive(bool keepAlive) {
    try {
        this->keepAlive_ = keepAlive;

        boost::asio::ip::tcp::socket::keep_alive option(keepAlive);
        this->socket.set_option(option);
    }
    catch (const boost::system::system_error& e) {
        throw ChannelException(e.what(), e.code().value());
    }
}

void AsioSocketChannelConfig::setPerformancePreferences(int connectionTime,
        int latency,
        int bandwidth) {
}

void AsioSocketChannelConfig::setReceiveBufferSize(int receiveBufferSize) {
    try {
        boost::asio::ip::tcp::socket::receive_buffer_size option(receiveBufferSize);
        this->socket.set_option(option);
        this->receiveBufferSize_ = receiveBufferSize;

    }
    catch (const boost::system::system_error& e) {
        throw ChannelException(e.what(), e.code().value());
    }
}

void AsioSocketChannelConfig::setReuseAddress(bool reuseAddress) {
    try {
        boost::asio::ip::tcp::socket::reuse_address option(reuseAddress);
        this->socket.set_option(option);
        this->reuseAddress_ = reuseAddress;
    }
    catch (const boost::system::system_error& e) {
        throw ChannelException(e.what(), e.code().value());
    }
}

void AsioSocketChannelConfig::setSendBufferSize(int sendBufferSize) {
    try {
        boost::asio::ip::tcp::socket::send_buffer_size option(sendBufferSize);
        this->socket.set_option(option);
        this->sendBufferSize_ = sendBufferSize;
    }
    catch (const boost::system::system_error& e) {
        throw ChannelException(e.what(), e.code().value());
    }
}

void AsioSocketChannelConfig::setSoLinger(int soLinger) {
    try {
        boost::asio::ip::tcp::socket::linger option;

        if (soLinger > 0) {
            option.enabled(true);
            option.timeout(soLinger);
        }

        this->socket.set_option(option);
        this->soLinger_ = soLinger;
    }
    catch (const boost::system::system_error& e) {
        throw ChannelException(e.what(), e.code().value());
    }
}

void AsioSocketChannelConfig::setTcpNoDelay(bool tcpNoDelay) {
    try {
        boost::asio::ip::tcp::no_delay option(tcpNoDelay);
        this->socket.set_option(option);
        this->tcpNoDelay_ = tcpNoDelay;
    }
    catch (const boost::system::system_error& e) {
        throw ChannelException(e.what(), e.code().value());
    }
}

void AsioSocketChannelConfig::setWriteBufferLowWaterMark(int writeBufferLowWaterMark) {
    try {
        boost::asio::ip::tcp::socket::send_low_watermark option(writeBufferLowWaterMark);
        this->socket.set_option(option);

        this->writeBufferLowWaterMark_ = writeBufferLowWaterMark;
    }
    catch (const boost::system::system_error& e) {
        throw ChannelException(e.what(), e.code().value());
    }
}

int AsioSocketChannelConfig::getWriteBufferLowWaterMark() const {
    if (writeBufferLowWaterMark_ == 0) {
        try {
            boost::asio::ip::tcp::socket::send_low_watermark option;
            this->socket.get_option(option);
            writeBufferLowWaterMark_ = option.value();
        }
        catch (const boost::system::system_error& e) {
            //it will throw an exception in windows XP.
            //however, we do not care the exception, just set default value.
            writeBufferLowWaterMark_ = DEFAULT_WRITE_BUFFER_LOW_WATERMARK;
            //throw ChannelException(e.what(), e.code().value());

            LOG_ERROR_E(e) << "getWriteBufferLowWaterMark has error";
        }
    }

    return writeBufferLowWaterMark_;
}

void AsioSocketChannelConfig::setReceiveBufferLowWaterMark(int receiveBufferLowWaterMark) {
    try {
        boost::asio::ip::tcp::socket::receive_low_watermark option(receiveBufferLowWaterMark);
        this->socket.set_option(option);
    }
    catch (const boost::system::system_error& e) {
        throw ChannelException(e.what(), e.code().value());
    }
}

int AsioSocketChannelConfig::getReceiveBufferLowWaterMark() const {
    try {
        boost::asio::ip::tcp::socket::receive_low_watermark option;
        this->socket.get_option(option);
        return option.value();
    }
    catch (const boost::system::system_error& e) {
        throw ChannelException(e.what(), e.code().value());
    }
}

int AsioSocketChannelConfig::getWriteBufferHighWaterMark() const {
    return writeBufferHighWaterMark_;
}

void AsioSocketChannelConfig::setWriteBufferHighWaterMark(int writeBufferHighWaterMark) {
    this->writeBufferHighWaterMark_ = writeBufferHighWaterMark;
}

}
}
}
