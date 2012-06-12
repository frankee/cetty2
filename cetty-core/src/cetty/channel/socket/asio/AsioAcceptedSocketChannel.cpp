/*
 * Copyright (c) 2010-2012 frankee zhou (frankee.zhou at gmail dot com)
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

#include <cetty/channel/socket/asio/AsioAcceptedSocketChannel.h>
#include <cetty/channel/Channels.h>
#include <cetty/channel/ChannelFactory.h>
#include <cetty/channel/ChannelPipeline.h>
#include <cetty/channel/socket/asio/AsioServicePool.h>
#include <cetty/buffer/ChannelBufferFactory.h>
#include <cetty/logging/LoggerHelper.h>

namespace cetty {
namespace channel {
namespace socket {
namespace asio {

using namespace cetty::channel;
using namespace cetty::buffer;

AsioAcceptedSocketChannel::AsioAcceptedSocketChannel(
    const ChannelPtr& parent,
    const ChannelFactoryPtr& factory,
    const ChannelPipelinePtr& pipeline,
    const ChannelSinkPtr& sink,
    const AsioServicePtr& ioService,
    const boost::thread::id& id)
    : AsioSocketChannel(parent, factory, pipeline, sink, ioService, id) {
}

AsioAcceptedSocketChannel::~AsioAcceptedSocketChannel() {

}

bool AsioAcceptedSocketChannel::start() {
    LOG_INFO(logger, "Accepted channel firing the Chanel Open event.");
    Channels::fireChannelOpen(this);

    const SocketAddress& localAddress = AsioSocketChannel::getLocalAddress();

    if (!localAddress.validated()) {
        LOG_WARN(logger, "Accepted Channel start to open, but the local address is invalid.");
        return false;
    }

    LOG_INFO(logger, "Accepted channel firing the Chanel Bound event.");
    Channels::fireChannelBound(this, localAddress);

    const SocketAddress& remoteAddress = AsioSocketChannel::getRemoteAddress();

    if (!remoteAddress.validated()) {
        LOG_WARN(logger, "Accepted Channel start to bind, but the remote address is invalid.");
        return false;
    }

    AsioSocketChannel::setConnectedState();
    LOG_INFO(logger, "Accepted channel firing the Chanel Connected event.");
    Channels::fireChannelConnected(this, remoteAddress);

    if (isReadable()) {
        Array arry;
        readBuffer->writableBytes(&arry);
        LOG_INFO(logger, "Accepted channel begin to async read, with the the buffer size %d.", arry.length());
        tcpSocket.async_read_some(
            boost::asio::buffer(arry.data(), arry.length()),
            boost::bind(&AsioSocketChannel::handleRead,
                        this,
                        boost::asio::placeholders::error,
                        boost::asio::placeholders::bytes_transferred));
    }
    else {
        LOG_INFO(logger, "Accepted channel is NOT readable, nothing with read operation.");
    }

    return true;
}

}
}
}
}