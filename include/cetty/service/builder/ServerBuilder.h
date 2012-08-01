#if !defined(CETTY_SERVICE_BUILDER_SERVERBUILDER_H)
#define CETTY_SERVICE_BUILDER_SERVERBUILDER_H

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

#include <map>
#include <string>
#include <vector>
#include <cetty/bootstrap/ServerBootstrap.h>
#include <cetty/channel/socket/asio/AsioServicePoolFwd.h>
#include <cetty/service/builder/ServerBuilderConfig.h>

namespace cetty {
namespace config {
class ConfigCenter;
}
}

namespace cetty {
namespace service {
namespace builder {

using namespace cetty::bootstrap;
using namespace cetty::channel::socket::asio;
using namespace cetty::service;
using namespace cetty::config;

class ServerBuilder {
public:
    ServerBuilder();
    ServerBuilder(int parentThreadCnt, int childThreadCnt = 0);
    ServerBuilder(const ServerBuilderConfig& config);

    virtual ~ServerBuilder();

    ChannelPtr build(const std::string& name,
                     const ChannelPipelinePtr& pipeline,
                     int port);

    ChannelPtr build(const std::string& name,
                     const ChannelPipelinePtr& pipeline,
                     const std::string& host,
                     int port);

    void buildAll();

    void getBuiltServers(std::map<std::string, ChannelPtr>* names);

    void waitingForExit();

    void registerPipeline(const std::string& name, const ChannelPipelinePtr& pipeline);
    void unregisterPipeline(const std::string& name);

    ChannelPipelinePtr getPipeline(const std::string& name);

    const ServerBuilderConfig& getConfig() const { return config; }
    const AsioServicePoolPtr& getServicePool();

protected:
    ChannelPtr build(const std::string& name, int port);
    
    void stop();

private:
    int init();
    void deinit();

private:
    ServerBuilderConfig config;

    EventLoopPoolPtr parentEventLoopPool;
    EventLoopPoolPtr childEventLoopPool;

    AsioServicePoolPtr servicePool;

    std::map<std::string, ChannelPipelinePtr> pipelines;
    std::map<std::string, ServerBootstrap*> bootstraps;
};

}
}
}

#endif //#if !defined(CETTY_SERVICE_BUILDER_SERVERBUILDER_H)

// Local Variables:
// mode: c++
// End:
