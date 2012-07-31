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

class AsioClientServiceSink : public cetty::channel::ChannelSink {
public:
    AsioClientServiceSink() {}
    virtual ~AsioClientServiceSink() {}

    virtual void eventSunk(const ChannelPipeline& pipeline, const UserEvent& e) {}
    virtual void writeRequested(const ChannelPipeline& pipeline, const MessageEvent& e) {}
    virtual void stateChangeRequested(const ChannelPipeline& pipeline, const ChannelStateEvent& e) {
        const ChannelFuturePtr& future = e.getFuture();
        const ChannelPtr& channel = e.getChannel();
        const ChannelState& state = e.getState();
        const boost::any& value = e.getValue();

        AsioClientService* asioClientService =
            static_cast<AsioClientService*>(channel);

        if (state == ChannelState::OPEN) {
            if (value.empty()) {
                //asioSocketChannel->internalClose(future);
                //LOG_INFO(logger, "received a close channel event, so closed the channel.");
            }
            else {
                //asioClientService->setConnected();
            }
        }

#if 0
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

#endif
    }

    virtual void exceptionCaught(const ChannelPipeline& pipeline, const UserEvent& e, const ChannelPipelineException& cause) {}
};

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
