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

#include <cetty/service/asio/AsioClientServiceFactory.h>

#include <cetty/channel/ChannelSink.h>
#include <cetty/channel/IpAddress.h>
#include <cetty/channel/SocketAddress.h>
#include <cetty/channel/ChannelState.h>
#include <cetty/channel/ChannelStateEvent.h>
#include <cetty/channel/ChannelPipelineFactory.h>
#include <cetty/channel/socket/asio/AsioServicePool.h>
#include <cetty/channel/socket/asio/AsioIpAddressImplFactory.h>
#include <cetty/channel/socket/asio/AsioSocketAddressImplFactory.h>
#include <cetty/util/TimerFactory.h>
#include <cetty/util/internal/asio/AsioDeadlineTimerFactory.h>
#include <cetty/service/asio/AsioClientService.h>

namespace cetty {
namespace service {
namespace asio {

using namespace cetty::channel;
using namespace cetty::channel::socket::asio;
using namespace cetty::util;
using namespace cetty::util::internal::asio;


AsioClientServiceFactory::AsioClientServiceFactory(const AsioServicePtr& ioService,
        const AsioServicePoolPtr& pool)
    : ioService(ioService),
      sink(new AsioClientServiceSink),
      timerFactory(),
      socketAddressFactory(),
      ipAddressFactory() {
    init(pool);
}

AsioClientServiceFactory::~AsioClientServiceFactory() {
    if (sink) {
        delete sink;
    }

    deinit();
}

cetty::channel::ChannelPtr AsioClientServiceFactory::newChannel(const ChannelPipelinePtr& pipeline) {
    if (ioService) {
        return new AsioClientService(shared_from_this(),
                                     pipeline,
                                     ioService);
    }
    else {
        return ChannelPtr();
    }
}

void AsioClientServiceFactory::shutdown() {

}

void AsioClientServiceFactory::init(const AsioServicePoolPtr& pool) {
    if (!TimerFactory::hasFactory()) {
        if (pool) {
            timerFactory = new AsioDeadlineTimerFactory(pool);
        }
        else {
            timerFactory = new AsioDeadlineTimerFactory(ioService);
        }

        TimerFactory::setFactory(timerFactory);
    }

    if (!SocketAddress::hasFactory()) {
        socketAddressFactory = new AsioTcpSocketAddressImplFactory(
            pool ? pool->getService(AsioServicePool::PRIORITY_BOSS)->service()
            : ioService->service());
        SocketAddress::setFacotry(socketAddressFactory);
    }

    if (!IpAddress::hasFactory()) {
        ipAddressFactory = new AsioIpAddressImplFactory();
        IpAddress::setFactory(ipAddressFactory);
    }
}

void AsioClientServiceFactory::deinit() {
    if (socketAddressFactory) {
        SocketAddress::resetFactory();

        delete socketAddressFactory;
        socketAddressFactory = NULL;
    }

    if (ipAddressFactory) {
        IpAddress::resetFactory();

        delete ipAddressFactory;
        ipAddressFactory = NULL;
    }

    if (timerFactory) {
        TimerFactory::resetFactory();

        delete timerFactory;
        timerFactory = NULL;
    }
}

}
}
}
