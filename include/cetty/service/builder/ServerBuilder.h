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
#include <cetty/bootstrap/ServerBootstrapPtr.h>
#include <cetty/service/builder/ServerBuilderConfig.cnf.h>

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

    virtual ~ServerBuilder();

    ServerBuilder& registerServer(const std::string& name,
                        const ChildInitializer& childInitializer);

    ServerBuilder& registerServer(const std::string& name,
                        const ChannelOptions& options,
                        const ChannelOptions& childOptions,
                        const ChildInitializer& childInitializer);

    ServerBuilder& setOptions(const std::string& name,
                    const ChannelOptions& options,
                    const ChannelOptions& childOptions);

    ChannelPtr build(const std::string& name, int port);

    ChannelPtr build(const std::string& name,
                     const std::string& host,
                     int port);

    ChannelPtr build(const std::string& name,
                     const ChildInitializer& childInitializer,
                     int port);

    ChannelPtr build(const std::string& name,
                     const ChildInitializer& childInitializer,
                     const std::string& host,
                     int port);

    ChannelPtr build(const std::string& name,
                     const ChannelOptions& options,
                     const ChannelOptions& childOptions,
                     const ChildInitializer& childInitializer,
                     int port);

    ChannelPtr build(const std::string& name,
                     const ChannelOptions& options,
                     const ChannelOptions& childOptions,
                     const ChildInitializer& childInitializer,
                     const std::string& host,
                     int port);

    ServerBuilder& buildAll();

    void waitingForExit();

    const ServerBuilderConfig& config() const;
    const EventLoopPoolPtr& parentPool() const;
    const EventLoopPoolPtr& childPool() const;

private:
    int init();
    void shutdown();

    ChannelPtr build(const ServerBootstrapPtr& bootstrap,
                     const std::string& host,
                     int port);

private:
    typedef std::map<std::string, ServerBootstrapPtr> ServerBootstraps;

private:
    ServerBuilderConfig config_;

    EventLoopPoolPtr parentEventLoopPool_;
    EventLoopPoolPtr childEventLoopPool_;

    ServerBootstraps bootstraps_;
};

inline
const ServerBuilderConfig& ServerBuilder::config() const {
    return config_;
}

inline
const EventLoopPoolPtr& ServerBuilder::parentPool() const {
    return parentEventLoopPool_;
}

inline
const EventLoopPoolPtr& ServerBuilder::childPool() const {
    return childEventLoopPool_;
}

}
}
}

#endif //#if !defined(CETTY_SERVICE_BUILDER_SERVERBUILDER_H)

// Local Variables:
// mode: c++
// End:
