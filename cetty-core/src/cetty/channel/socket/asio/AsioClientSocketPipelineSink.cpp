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

#include <cetty/channel/socket/asio/AsioClientSocketPipelineSink.h>

#include <boost/bind.hpp>
#include <boost/asio.hpp>

#include <cetty/channel/Channels.h>
#include <cetty/channel/ChannelState.h>
#include <cetty/channel/ChannelFuture.h>
#include <cetty/channel/MessageEvent.h>
#include <cetty/channel/ChannelStateEvent.h>
#include <cetty/channel/AbstractChannelSink.h>
#include <cetty/channel/ChannelException.h>
#include <cetty/channel/IpAddress.h>
#include <cetty/channel/SocketAddress.h>

#include <cetty/channel/socket/asio/AsioClientSocketChannel.h>
#include <cetty/channel/socket/asio/AsioClientSocketChannelFactory.h>
#include <cetty/channel/socket/asio/AsioServicePool.h>
#include <cetty/channel/socket/asio/AsioWriteOperationQueue.h>

#include <cetty/buffer/ChannelBuffer.h>

#include <cetty/logging/LoggerHelper.h>
#include <cetty/logging/InternalLoggerFactory.h>

#include <cetty/util/Integer.h>
#include <cetty/util/internal/ConversionUtil.h>

namespace cetty {
namespace channel {
namespace socket {
namespace asio {

using namespace cetty::channel;
using namespace cetty::util;
using namespace cetty::util::internal;
using namespace cetty::logging;

InternalLogger* AsioClientSocketPipelineSink::logger =  NULL;

AsioClientSocketPipelineSink::AsioClientSocketPipelineSink(
    bool needStartRunService) : needStartRunService(needStartRunService) {
    if (NULL == logger) {
        logger = InternalLoggerFactory::getInstance("AsioClientSocketPipelineSink");
    }
}

AsioClientSocketPipelineSink::~AsioClientSocketPipelineSink() {
}

void AsioClientSocketPipelineSink::writeRequested(
    const ChannelPipeline& pipeline,
    const MessageEvent& e) {
    const ChannelPtr& channel = e.getChannel();
    (static_cast<AsioSocketChannel*>(channel))->internalWrite(e);

    LOG_INFO(logger, "has written the msg to the address.");
}

void AsioClientSocketPipelineSink::stateChangeRequested(
    const ChannelPipeline& pipeline,
    const ChannelStateEvent& e) {
    handleStateChange(e);
}

void AsioClientSocketPipelineSink::handleStateChange(const ChannelStateEvent& evt) {
    const ChannelFuturePtr& future = evt.getFuture();
    const ChannelPtr& channel = evt.getChannel();
    const ChannelState& state = evt.getState();
    const boost::any& value = evt.getValue();

    AsioSocketChannel* asioSocketChannel = static_cast<AsioSocketChannel*>(channel);

    if (state == ChannelState::OPEN) {
        if (value.empty()) {
            asioSocketChannel->internalClose(future);
            LOG_INFO(logger, "received a close channel event, so closed the channel.");
        }
    }
    else if (state == ChannelState::BOUND) {
        if (value.empty()) {
            asioSocketChannel->internalClose(future);
            LOG_INFO(logger, "received an unbound channel event, so closed the channel.");
        }
        else {
            LOG_WARN(logger, "has not implement the client channel bind.");
        }
    }
    else if (state == ChannelState::CONNECTED) {
        if (!value.empty()) {
            const SocketAddress* address = boost::any_cast<SocketAddress>(&value);

            if (address) {
                connect(channel, future, *address);
                LOG_INFO(logger, "received an connect channel event, so connect to %s.", address->toString().c_str());
            }
            else {
                LOG_ERROR(logger, "received an connect channel event, but address is invalid.");
            }
        }
        else {
            asioSocketChannel->internalClose(future);
            LOG_INFO(logger, "received an disconnect channel event, so closed the channel.");
        }
    }
    else if (state == ChannelState::INTEREST_OPS) {
        if (!value.empty()) {
            int v = ConversionUtil::toInt(value);
            asioSocketChannel->internalSetInterestOps(future, v);
            LOG_INFO(logger, "received an interestOps (%s) event.", Channel::getInterestOpsString(v));
        }
        else {
            LOG_WARN(logger, "received an invalid interestOps event.");
        }
    }
}

void AsioClientSocketPipelineSink::connect(const ChannelPtr& channel,
        const ChannelFuturePtr& cf,
        const SocketAddress& remoteAddress) {
    AsioSocketChannel* socketChannel = static_cast<AsioSocketChannel*>(channel);
    const std::string& hostname = remoteAddress.hostName();
    std::string port = Integer::toString(remoteAddress.port());

    boost::asio::ip::tcp::resolver resolver(socketChannel->getService()->service());
    boost::asio::ip::tcp::resolver::query query(hostname, port);

    boost::system::error_code error;
    boost::asio::ip::tcp::resolver::iterator iterator = resolver.resolve(query, error);

    if (error) {
        LOG_ERROR(logger, "boost asio can NOT resolve %s:%s, code=%d, msg=%s, then firing this exception.", error.value(), error.message().c_str());

        ChannelException exception(error.message(), error.value());
        cf->setFailure(exception);
        Channels::fireExceptionCaught(channel, exception);

        socketChannel->internalClose(channel->getCloseFuture());
        return;
    }

    boost::asio::ip::tcp::endpoint endpoint = *iterator;
    socketChannel->getSocket().async_connect(
        endpoint,
        boost::bind(&AsioSocketChannel::handleConnect,
                    socketChannel,
                    boost::asio::placeholders::error,
                    ++iterator,
                    cf));

    if (needStartRunService) {
        AsioClientSocketChannelFactory* factory
            = dynamic_cast<AsioClientSocketChannelFactory*>(channel->getFactory().get());

        if (factory) {
            LOG_INFO(logger, "the asio service pool starting to run in main thread.");

            if (factory->start()) {
                LOG_INFO(logger, "the asio service pool started to running.");
            }
            else {
                //TODO: may duplicated with the callback of AsioSocketChannel::handleConnect
                LOG_ERROR(logger, "start the boost asio service error, and firing an exception.");
                ChannelException e("the boost asio service can not be started.");
                cf->setFailure(e);
                Channels::fireExceptionCaught(channel, e);
                socketChannel->internalClose(channel->getCloseFuture());

                //TODO: should terminate the program
                std::terminate();
            }
        }
        else {
            LOG_ERROR(logger, "the asio service pool is in single thread mode, but the factory type is unknown.");
        }
    }
}

}
}
}
}
