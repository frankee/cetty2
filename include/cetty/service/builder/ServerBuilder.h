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
#include <boost/noncopyable.hpp>
#include <cetty/channel/Channel.h>
#include <cetty/channel/EventLoopPoolPtr.h>
#include <cetty/service/builder/ServerBuilderConfig.h>

namespace cetty {
    namespace bootstrap {
        class ServerBootstrap;
    }
}


namespace cetty {
namespace service {
namespace builder {

using namespace cetty::channel;
using namespace cetty::bootstrap;
using namespace cetty::service;
using namespace cetty::config;

class ServerBuilder : private boost::noncopyable {
public:
    typedef Channel::Initializer ChildInitializer;

public:
    ServerBuilder();
    ServerBuilder(int parentThreadCnt, int childThreadCnt = 0);
    ServerBuilder(const ServerBuilderConfig& config);

    virtual ~ServerBuilder();

    ChannelPtr build(const std::string& name,
                     const ChildInitializer& initializer,
                     int port);

    ChannelPtr build(const std::string& name,
                     const ChildInitializer& initializer,
                     const std::string& host,
                     int port);

    void buildAll();

    void getBuiltServers(std::map<std::string, ChannelPtr>* names);

    void waitingForExit();

    void registerChildInitializer(const std::string& name, const ChildInitializer& initializer);
    void unregisterChildInitializer(const std::string& name);

    const ServerBuilderConfig& getConfig() const { return config_; }

    const EventLoopPoolPtr& getParentPool() const { return parentEventLoopPool_; }
    const EventLoopPoolPtr& getChildPool() const { return childEventLoopPool_; }

protected:
    ChannelPtr build(const std::string& name, int port);

    void stop();

private:
    int init();
    void deinit();

private:
    typedef std::map<std::string, ChildInitializer> ChildInitializers;
    typedef std::map<std::string, ServerBootstrap*> ServerBootstraps;

private:
    ServerBuilderConfig config_;

    EventLoopPoolPtr parentEventLoopPool_;
    EventLoopPoolPtr childEventLoopPool_;

    ServerBootstraps bootstraps_;
    ChildInitializers childInitializers_;
};

}
}
}

#endif //#if !defined(CETTY_SERVICE_BUILDER_SERVERBUILDER_H)

// Local Variables:
// mode: c++
// End:
