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

#include <cetty/service/asio/AsioClientService.h>

#include <cetty/channel/Channels.h>
#include <cetty/channel/ChannelPipelineFactory.h>
#include <cetty/channel/socket/asio/AsioServicePool.h>

namespace cetty {
namespace service {
namespace asio {

using namespace cetty::channel;

AsioClientService::AsioClientService(const ChannelFactoryPtr& factory,
                                     const ChannelPipelinePtr& pipeline,
                                     const ChannelSinkPtr& sink,
                                     const AsioServicePtr& ioService)
    : ClientService(factory, pipeline, sink),
      threadId(ioService->getThreadId()),
      ioService(ioService) {

    ChannelPipelines::fireChannelOpen(this);
    ChannelPipelines::fireChannelConnected(this, SocketAddress::NULL_ADDRESS);
}

}
}
}
