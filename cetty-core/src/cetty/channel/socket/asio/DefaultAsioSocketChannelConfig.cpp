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

#include <cetty/buffer/ChannelBufferFactory.h>
#include <cetty/channel/ChannelException.h>
#include <cetty/channel/ChannelPipelineFactory.h>
#include <cetty/channel/ReceiveBufferSizePredictor.h>
#include <cetty/channel/ReceiveBufferSizePredictorFactory.h>
#include <cetty/channel/AdaptiveReceiveBufferSizePredictorFactory.h>
#include <cetty/channel/socket/asio/DefaultAsioSocketChannelConfig.h>

#include <cetty/util/internal/ConversionUtil.h>
#include <cetty/util/Exception.h>
#include <cetty/util/NestedDiagnosticContext.h>

#include <cetty/logging/LoggerHelper.h>
#include <cetty/logging/InternalLoggerFactory.h>

namespace cetty {
namespace channel {
namespace socket {
namespace asio {

using namespace cetty::channel;
using namespace cetty::util;
using namespace cetty::util::internal;
using namespace cetty::logging;

InternalLogger* DefaultAsioSocketChannelConfig::logger = NULL;

ReceiveBufferSizePredictorFactoryPtr
DefaultAsioSocketChannelConfig::DEFAULT_PREDICTOR_FACTORY = new AdaptiveReceiveBufferSizePredictorFactory;

DefaultAsioSocketChannelConfig::DefaultAsioSocketChannelConfig(TcpSocket& socket) 
    : socket(socket),
    writeBufferLowWaterMark(0),
    writeBufferHighWaterMark(DEFAULT_WRITE_BUFFER_HIGH_WATERMARK),
    predictor(NULL),
    predictorFactory(DEFAULT_PREDICTOR_FACTORY) {

    if (NULL == logger) {
        InternalLoggerFactory::getInstance("DefaultAsioSocketChannelConfig");
    }
}

bool DefaultAsioSocketChannelConfig::setOption(const std::string& key, const boost::any& value) {
    if (DefaultChannelConfig::setOption(key, value)) {
        return true;
    }

    if (key == "receiveBufferSize") {
        setReceiveBufferSize(ConversionUtil::toInt(value));
    }
    else if (key == "sendBufferSize") {
        setSendBufferSize(ConversionUtil::toInt(value));
    }
    else if (key == "tcpNoDelay") {
        setTcpNoDelay(ConversionUtil::toBoolean(value));
    }
    else if (key == "keepAlive") {
        setKeepAlive(ConversionUtil::toBoolean(value));
    }
    else if (key == "reuseAddress") {
        setReuseAddress(ConversionUtil::toBoolean(value));
    }
    else if (key == "soLinger") {
        setSoLinger(ConversionUtil::toInt(value));
    }
    else if (key == "writeBufferLowWaterMark") {
        setWriteBufferLowWaterMark(ConversionUtil::toInt(value));
    }
    else if (key == "receiveBufferLowWaterMark") {
        setReceiveBufferLowWaterMark(ConversionUtil::toInt(value));
    }
    else if (key == "receiveBufferSizePredictorFactory") {
        ReceiveBufferSizePredictorFactoryPtr const* factory =
            boost::any_cast<ReceiveBufferSizePredictorFactoryPtr>(&value);

        if (factory && *factory) {
            setReceiveBufferSizePredictorFactory(*factory);
        }
    }
    else if (key == "receiveBufferSizePredictor") {
        ReceiveBufferSizePredictorPtr const* predictor =
            boost::any_cast<ReceiveBufferSizePredictorPtr>(&value);

        if (predictor && *predictor) {
            setReceiveBufferSizePredictor(*predictor);
        }
    }
    else {
        return false;
    }

    return true;
}

const boost::optional<int>& DefaultAsioSocketChannelConfig::getReceiveBufferSize() const {
    if (receiveBufferSize) {
        return receiveBufferSize;
    }

    try {
        boost::asio::ip::tcp::socket::receive_buffer_size option;
        this->socket.get_option(option);
        this->receiveBufferSize = option.value();
    }
    catch (const boost::system::system_error& e) {
        throw ChannelException(e.what(), e.code().value());
    }

    return receiveBufferSize;
}

const boost::optional<int>& DefaultAsioSocketChannelConfig::getSendBufferSize() const {
    if (sendBufferSize) {
        return sendBufferSize;
    }

    try {
        boost::asio::ip::tcp::socket::send_buffer_size option;
        this->socket.get_option(option);
        sendBufferSize = option.value();
    }
    catch (const boost::system::system_error& e) {
        throw ChannelException(e.what(), e.code().value());
    }

    return sendBufferSize;
}

const boost::optional<int>& DefaultAsioSocketChannelConfig::getSoLinger() const {
    if (soLinger) {
        return soLinger;
    }

    try {
        boost::asio::ip::tcp::socket::linger option;
        this->socket.get_option(option);
        this->soLinger = option.enabled() ? 0 : option.timeout();
    }
    catch (const boost::system::system_error& e) {
        throw ChannelException(e.what(), e.code().value());
    }

    return soLinger;
}

const boost::optional<bool>& DefaultAsioSocketChannelConfig::isKeepAlive() const {
    if (this->isKeepAlive()) {
        return this->isKeepAlive();
    }

    try {
        boost::asio::ip::tcp::socket::keep_alive option;
        this->socket.get_option(option);
        this->keepAlive = option.value();
    }
    catch (const boost::system::system_error& e) {
        throw ChannelException(e.what(), e.code().value());
    }

    return this->keepAlive;
}

const boost::optional<bool>& DefaultAsioSocketChannelConfig::isReuseAddress() const {
    if (this->reuseAddress) {
        return this->reuseAddress;
    }

    try {
        boost::asio::ip::tcp::socket::reuse_address option;
        this->socket.get_option(option);
        this->reuseAddress = option.value();
    }
    catch (const boost::system::system_error& e) {
        throw ChannelException(e.what(), e.code().value());
    }

    return this->reuseAddress;
}

const boost::optional<bool>& DefaultAsioSocketChannelConfig::isTcpNoDelay() const {
    if (tcpNoDelay) {
        return tcpNoDelay;
    } 

    try {
        boost::asio::ip::tcp::no_delay option;
        this->socket.get_option(option);
        tcpNoDelay = option.value();
    }
    catch (const boost::system::system_error& e) {
        throw ChannelException(e.what(), e.code().value());
    }

    return tcpNoDelay;
}

void DefaultAsioSocketChannelConfig::setKeepAlive(bool keepAlive) {
    try {
        this->keepAlive = keepAlive;

        boost::asio::ip::tcp::socket::keep_alive option(keepAlive);
        this->socket.set_option(option);
    }
    catch (const boost::system::system_error& e) {
        throw ChannelException(e.what(), e.code().value());
    }
}

void DefaultAsioSocketChannelConfig::setPerformancePreferences(int connectionTime, int latency, int bandwidth) {
}

void DefaultAsioSocketChannelConfig::setReceiveBufferSize(int receiveBufferSize) {
    try {
        boost::asio::ip::tcp::socket::receive_buffer_size option(receiveBufferSize);
        this->socket.set_option(option);
        this->receiveBufferSize = receiveBufferSize;

    }
    catch (const boost::system::system_error& e) {
        throw ChannelException(e.what(), e.code().value());
    }
}

void DefaultAsioSocketChannelConfig::setReuseAddress(bool reuseAddress) {
    try {
        boost::asio::ip::tcp::socket::reuse_address option(reuseAddress);
        this->socket.set_option(option);
        this->reuseAddress = reuseAddress;
    }
    catch (const boost::system::system_error& e) {
        throw ChannelException(e.what(), e.code().value());
    }
}

void DefaultAsioSocketChannelConfig::setSendBufferSize(int sendBufferSize) {
    try {
        boost::asio::ip::tcp::socket::send_buffer_size option(sendBufferSize);
        this->socket.set_option(option);
        this->sendBufferSize = sendBufferSize;
    }
    catch (const boost::system::system_error& e) {
        throw ChannelException(e.what(), e.code().value());
    }
}

void DefaultAsioSocketChannelConfig::setSoLinger(int soLinger) {
    try {
        boost::asio::ip::tcp::socket::linger option;

        if (soLinger > 0) {
            option.enabled(true);
            option.timeout(soLinger);
        }

        this->socket.set_option(option);
        this->soLinger = soLinger;
    }
    catch (const boost::system::system_error& e) {
        throw ChannelException(e.what(), e.code().value());
    }
}

void DefaultAsioSocketChannelConfig::setTcpNoDelay(bool tcpNoDelay) {
    try {
        boost::asio::ip::tcp::no_delay option(tcpNoDelay);
        this->socket.set_option(option);
        this->tcpNoDelay = tcpNoDelay;
    }
    catch (const boost::system::system_error& e) {
        throw ChannelException(e.what(), e.code().value());
    }
}

void DefaultAsioSocketChannelConfig::setWriteBufferLowWaterMark(int writeBufferLowWaterMark) {
    try {
        boost::asio::ip::tcp::socket::send_low_watermark option(writeBufferLowWaterMark);
        this->socket.set_option(option);

        this->writeBufferLowWaterMark = writeBufferLowWaterMark;
    }
    catch (const boost::system::system_error& e) {
        throw ChannelException(e.what(), e.code().value());
    }
}

int DefaultAsioSocketChannelConfig::getWriteBufferLowWaterMark() const {
    if (writeBufferLowWaterMark == 0) {
        try {
            boost::asio::ip::tcp::socket::send_low_watermark option;
            this->socket.get_option(option);
            writeBufferLowWaterMark = option.value();
        }
        catch (const boost::system::system_error& e) {
            //it will throw an exception in windows XP.
            //however, we do not care the exception, just set default value.
            writeBufferLowWaterMark = DEFAULT_WRITE_BUFFER_LOW_WATERMARK;
            //throw ChannelException(e.what(), e.code().value());

            LOG_ERROR(logger, "has error %d, %s", e.code(), e.what());
        }
    }

    return writeBufferLowWaterMark;
}

void DefaultAsioSocketChannelConfig::setReceiveBufferLowWaterMark(int receiveBufferLowWaterMark) {
    try {
        boost::asio::ip::tcp::socket::receive_low_watermark option(receiveBufferLowWaterMark);
        this->socket.set_option(option);
    }
    catch (const boost::system::system_error& e) {
        throw ChannelException(e.what(), e.code().value());
    }
}

int DefaultAsioSocketChannelConfig::getReceiveBufferLowWaterMark() const {
    try {
        boost::asio::ip::tcp::socket::receive_low_watermark option;
        this->socket.get_option(option);
        return option.value();
    }
    catch (const boost::system::system_error& e) {
        throw ChannelException(e.what(), e.code().value());
    }
}

const ReceiveBufferSizePredictorPtr& DefaultAsioSocketChannelConfig::getReceiveBufferSizePredictor() {
    if (!predictor) {
        try {
            CETTY_NDC_SCOPE("DefaultAsioSocketChannelConfig::getReceiveBufferSizePredictor");
            predictor = predictorFactory->getPredictor();
            this->predictor = predictor;
        }
        catch (const Exception& e) {
            throw ChannelException(
                std::string("Failed to create a new predictor"), e.getCode());
        }
    }

    return predictor;
}

void DefaultAsioSocketChannelConfig::setReceiveBufferSizePredictor(const ReceiveBufferSizePredictorPtr& predictor) {
    if (!predictor) {
        throw NullPointerException("predictor");
    }

    this->predictor = predictor;
}

void DefaultAsioSocketChannelConfig::setReceiveBufferSizePredictorFactory(const ReceiveBufferSizePredictorFactoryPtr& predictorFactory) {
    if (!predictorFactory) {
        throw NullPointerException("predictorFactory");
    }

    this->predictorFactory = predictorFactory;
}

const ReceiveBufferSizePredictorFactoryPtr& DefaultAsioSocketChannelConfig::getReceiveBufferSizePredictorFactory() const {
    return this->predictorFactory;
}

bool DefaultAsioSocketChannelConfig::channelHasReaderBuffer() const {
    return true;
}

int DefaultAsioSocketChannelConfig::getWriteBufferHighWaterMark() const {
    return writeBufferHighWaterMark;
}

void DefaultAsioSocketChannelConfig::setWriteBufferHighWaterMark(int writeBufferHighWaterMark) {
    this->writeBufferHighWaterMark = writeBufferHighWaterMark;
}




}
}
}
}