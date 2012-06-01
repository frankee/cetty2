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

#include <cetty/channel/socket/asio/AsioClientSocketChannel.h>
#include <cetty/channel/Channels.h>
#include <cetty/channel/ChannelFactory.h>
#include <cetty/channel/ChannelPipeline.h>
#include <cetty/channel/socket/SocketChannel.h>
#include <cetty/buffer/ChannelBufferFactory.h>
#include <cetty/logging/LoggerHelper.h>

namespace cetty {
namespace channel {
namespace socket {
namespace asio {

using namespace ::cetty::logging;

AsioClientSocketChannel::AsioClientSocketChannel(
    const ChannelFactoryPtr& factory,
    const ChannelPipelinePtr& pipeline,
    const ChannelSinkPtr& sink,
    const AsioServicePtr& ioService,
    const boost::thread::id& id)
    : AsioSocketChannel(ChannelPtr(), factory, pipeline, sink, ioService, id) {

    LOG_INFO(logger, "Client channel firing the Channel Open Event.");
    Channels::fireChannelOpen(this);
}

AsioClientSocketChannel::~AsioClientSocketChannel() {
}

}
}
}
}
