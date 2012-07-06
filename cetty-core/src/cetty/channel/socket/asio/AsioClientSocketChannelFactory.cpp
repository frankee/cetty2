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

#include <cetty/channel/socket/asio/AsioServicePool.h>
#include <cetty/channel/socket/asio/AsioIpAddressImplFactory.h>
#include <cetty/channel/socket/asio/AsioSocketAddressImplFactory.h>
#include <cetty/channel/socket/asio/AsioClientSocketChannel.h>
#include <cetty/channel/socket/asio/AsioClientSocketPipelineSink.h>
#include <cetty/channel/socket/asio/AsioClientSocketChannelFactory.h>
#include <cetty/util/internal/asio/AsioDeadlineTimerFactory.h>
#include <cetty/util/Exception.h>

namespace cetty {
namespace channel {
namespace socket {
namespace asio {

using namespace cetty::util;
using namespace cetty::util::internal::asio;

AsioClientSocketChannelFactory::AsioClientSocketChannelFactory(
    const AsioServicePoolPtr& ioServicePool)
    : ioServicePool(ioServicePool),
      timerFactory(NULL),
      socketAddressFactory(NULL),
      ipAddressFactory(NULL) {
    init();
}

AsioClientSocketChannelFactory::AsioClientSocketChannelFactory(
    const AsioServicePtr& ioService)
    : ioService(ioService),
      timerFactory(NULL),
      socketAddressFactory(NULL),
      ipAddressFactory(NULL) {
    init();
}

AsioClientSocketChannelFactory::AsioClientSocketChannelFactory(int threadCnt)
    : ioServicePool(new AsioServicePool(threadCnt)),
      timerFactory(NULL),
      socketAddressFactory(NULL),
      ipAddressFactory(NULL) {
    init();
}


AsioClientSocketChannelFactory::~AsioClientSocketChannelFactory() {
    deinit();
}

void AsioClientSocketChannelFactory::init() {
    sink = new AsioClientSocketPipelineSink(needManuallyStartAsioService());

    if (!TimerFactory::hasFactory()) {
        if (ioServicePool) {
            timerFactory = new AsioDeadlineTimerFactory(ioServicePool);
        }
        else {
            timerFactory = new AsioDeadlineTimerFactory(ioService);
        }

        TimerFactory::setFactory(timerFactory);
    }

    if (!SocketAddress::hasFactory()) {
        socketAddressFactory = new AsioTcpSocketAddressImplFactory(
            ioServicePool ? ioServicePool->getService(AsioServicePool::PRIORITY_BOSS)->service()
            : ioService->service());
        SocketAddress::setFacotry(socketAddressFactory);
    }

    if (!IpAddress::hasFactory()) {
        ipAddressFactory = new AsioIpAddressImplFactory();
        IpAddress::setFactory(ipAddressFactory);
    }
}

void AsioClientSocketChannelFactory::deinit() {
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

ChannelPtr AsioClientSocketChannelFactory::newChannel(const ChannelPipelinePtr& pipeline) {
    const AsioServicePtr& service = ioServicePool ? ioServicePool->getService() : ioService;
    ChannelPtr client =
        new AsioClientSocketChannel(shared_from_this(),
                                    pipeline,
                                    sink,
                                    service,
                                    service->getThreadId());

    clientChannels.push_back(client);
    return client;
}

void AsioClientSocketChannelFactory::releaseExternalResources() {
    if (ioServicePool) {
        ioServicePool->stop();
        ioServicePool->waitForExit();
    }

    clientChannels.clear();
}

bool AsioClientSocketChannelFactory::start() {
    if (needManuallyStartAsioService()) {
        return ioServicePool->run();
    }

    return true;
}

bool AsioClientSocketChannelFactory::needManuallyStartAsioService() const {
    return ioServicePool && ioServicePool->onlyMainThread();
}

}
}
}
}
