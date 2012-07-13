#if !defined(CETTY_SERVICE_BUILDER_CLIENTBUILDER_H)
#define CETTY_SERVICE_BUILDER_CLIENTBUILDER_H

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

#include <cetty/bootstrap/ClientBootstrap.h>
#include <cetty/channel/Channels.h>
#include <cetty/channel/ChannelPipeline.h>
#include <cetty/channel/ChannelFactory.h>
#include <cetty/channel/socket/asio/AsioServicePool.h>
#include <cetty/channel/socket/asio/AsioClientSocketChannelFactory.h>

#include <cetty/config/ConfigCenter.h>

#include <cetty/service/ClientService.h>
#include <cetty/service/ClientServiceDispatcher.h>
#include <cetty/service/ClientServiceMessageHandler.h>
#include <cetty/service/asio/AsioClientService.h>
#include <cetty/service/asio/AsioClientServiceFactory.h>

namespace cetty {
namespace service {
namespace builder {

using namespace cetty::bootstrap;
using namespace cetty::channel;
using namespace cetty::channel::socket::asio;
using namespace cetty::config;
using namespace cetty::service;
using namespace cetty::service::asio;

class ClientBuilderConfig;

template<typename ReqT, typename RepT>
class ClientBuilder {
public:
    typedef cetty::service::ClientServiceDispatcher<ReqT, RepT> DispatcherType;
    typedef cetty::service::ClientServiceMessageHandler<ReqT, RepT> MessageHandlerType;

public:
    ClientBuilder()
        : serviceInited(false),
          asioServicePool(new AsioServicePool(1)) {
    }
    ClientBuilder(int threadCnt)
        : serviceInited(false),
          asioServicePool(new AsioServicePool(threadCnt)) {
    }
    ClientBuilder(const AsioServicePoolPtr& ioServicePool)
        : serviceInited(false),
          asioServicePool(ioServicePool) {
    }
    ClientBuilder(const AsioServicePtr& ioService)
        : serviceInited(false),
          asioService(ioService) {
    }

    ClientBuilder(const ClientBuilderConfig& conf);
    ClientBuilder(const ClientBuilderConfig& conf, const AsioServicePoolPtr& ioServciePool);
    ClientBuilder(const ClientBuilderConfig& conf, const AsioServicePtr& ioService);

    virtual ~ClientBuilder() {}

    void setPipeline(const ChannelPipelinePtr& pipeline) {
        clientPipeline = pipeline;
    }

    void addConnection(const std::string& host, int port, int limit = 1) {
        connections.push_back(Connection(host, port, limit));
    }

    ClientServicePtr build() {
        ServiceContext context;
        const AsioServicePtr& asioService =
            asioServicePool ? asioServicePool->getService() : asioService;

        ServiceContextMap::iterator itr = serviceContext.find(asioService->getId());

        if (itr != serviceContext.end()) {
            context = itr->second;
        }
        else {
            ChannelFactoryPtr factory =
                new AsioClientServiceFactory(asioService, asioServicePool);

            context = std::make_pair(factory,
                                     getPipelineFactory(asioService));

            serviceContext[asioService->getId()] = context;
        }

        ChannelPtr channel = context.first->newChannel(context.second->getPipeline());
        return (ClientServicePtr)channel;
    }

private:
    ChannelPipelineFactoryPtr getPipelineFactory(const AsioServicePtr& ioService) {
        // construct the pipeline
        ChannelHandlerPtr dispatcher(
            new DispatcherType(connections, clientPipeline, ioService));

        ChannelHandlerPtr messageHandler(new MessageHandlerType);

        ChannelPipelinePtr pipeline = Channels::pipeline();
        pipeline->addLast("dispatcher", dispatcher);
        pipeline->addLast("message", messageHandler);

        return Channels::pipelineFactory(pipeline);
    }

private:
    typedef std::pair<ChannelFactoryPtr, ChannelPipelineFactoryPtr> ServiceContext;
    typedef std::map<int, ServiceContext> ServiceContextMap;

private:
    bool serviceInited;
    std::vector<Connection> connections;

    AsioServicePoolPtr asioServicePool;
    AsioServicePtr asioService;

    ServiceContextMap serviceContext;

    ChannelPipelinePtr clientPipeline;
};

}
}
}

#endif //#if !defined(CETTY_SERVICE_BUILDER_CLIENTBUILDER_H)

// Local Variables:
// mode: c++
// End:
