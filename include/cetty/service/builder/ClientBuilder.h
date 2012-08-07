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


#include <cetty/channel/EventLoopPtr.h>
#include <cetty/channel/EventLoopPoolPtr.h>
#include <cetty/channel/ChannelPipeline.h>
#include <cetty/channel/ChannelPipelines.h>
#include <cetty/channel/ChannelFactory.h>

#include <cetty/service/ClientService.h>
#include <cetty/service/ClientServiceDispatcher.h>
#include <cetty/service/ClientServiceMessageHandler.h>
#include <cetty/service/asio/AsioClientServiceFactory.h>

namespace cetty {
namespace service {
namespace builder {

using namespace cetty::channel;
using namespace cetty::service;
using namespace cetty::service::asio;

class ClientBuilderConfig;

template<typename ReqT, typename RepT = ReqT>
class ClientBuilder {
public:
    typedef cetty::service::ClientServiceDispatcher<ReqT, RepT> DispatcherType;
    typedef cetty::service::ClientServiceMessageHandler<ReqT, RepT> MessageHandlerType;

public:
    ClientBuilder()
        : serviceInited(false),
          eventLoopPool(new AsioServicePool(1)) {
        bootstrap.init(eventLoopPool, this);
    }
    ClientBuilder(int threadCnt)
        : serviceInited(false),
          eventLoopPool(new AsioServicePool(threadCnt)) {
        bootstrap.init(eventLoopPool, this);
    }
    ClientBuilder(const EventLoopPtr& eventLoop)
        : serviceInited(false),
          eventLoop(eventLoop) {
        bootstrap.init(eventLoop, this);
    }
    ClientBuilder(const EventLoopPoolPtr& eventLoopPool)
        : serviceInited(false),
          eventLoopPool(eventLoopPool) {
        bootstrap.init(eventLoopPool, this);
    }

    ClientBuilder(const ClientBuilderConfig& conf);
    ClientBuilder(const ClientBuilderConfig& conf, const EventLoopPtr& eventLoop);
    ClientBuilder(const ClientBuilderConfig& conf, const EventLoopPoolPtr& eventLoopPool);


    virtual ~ClientBuilder() {}

    void setPipeline(const ChannelPipelinePtr& pipeline) {
        clientPipeline = pipeline;
    }

    void addConnection(const std::string& host, int port, int limit = 1) {
        connections.push_back(Connection(host, port, limit));
    }

    ClientServicePtr build() {
        ChannelPtr channel = bootstrap.newChannel();
        channel->getPipeline()->fireChannelActive();
        return boost::dynamic_pointer_cast<ClientService>(channel);
    }

private:
    ChannelPipelinePtr getPipeline(const EventLoopPtr& eventLoop) {
        // construct the pipeline
        ChannelHandlerPtr dispatcher(
            new DispatcherType(connections, clientPipeline, eventLoop));

        ChannelHandlerPtr messageHandler(new MessageHandlerType);

        ChannelPipelinePtr pipeline = ChannelPipelines::pipeline();
        pipeline->addLast("message", messageHandler);

        pipeline->setSinkHandler(dispatcher);

        return pipeline;
    }

private:
    class ClientServiceBootstrap {
    public:
        ClientServiceBootstrap() : clientBuilder() {}

        void init(const EventLoopPtr& eventLoop,
                  ClientBuilder* builder) {
            factory = new AsioClientServiceFactory(eventLoop);
            clientBuilder = builder;
        }

        void init(const EventLoopPoolPtr& eventLoopPool,
                  ClientBuilder* builder) {
            factory = new AsioClientServiceFactory(eventLoopPool);
            clientBuilder = builder;
        }

        ChannelPtr newChannel() {
            const EventLoopPtr& eventLoop = factory->getEventLoop();
            return factory->newChannel(clientBuilder->getPipeline(eventLoop), eventLoop);
        }

    private:
        ClientBuilder* clientBuilder;
        AsioClientServiceFactoryPtr factory;
    };

private:
    bool serviceInited;
    std::vector<Connection> connections;

    EventLoopPtr eventLoop;
    EventLoopPoolPtr eventLoopPool;

    ChannelPipelinePtr clientPipeline;

    ClientServiceBootstrap bootstrap;
};

}
}
}

#endif //#if !defined(CETTY_SERVICE_BUILDER_CLIENTBUILDER_H)

// Local Variables:
// mode: c++
// End:
