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

#include <cetty/service/builder/ServerBuilder.h>

#include <cetty/channel/ChannelPipeline.h>
#include <cetty/channel/asio/AsioServicePool.h>
#include <cetty/bootstrap/ServerUtil.h>
#include <cetty/bootstrap/ServerBootstrap.h>
#include <cetty/config/ConfigCenter.h>
#include <cetty/logging/Logger.h>
#include <cetty/logging/LoggerHelper.h>

namespace cetty {
namespace service {
namespace builder {

using namespace cetty::channel;
using namespace cetty::channel::asio;
using namespace cetty::bootstrap;
using namespace cetty::service;
using namespace cetty::config;
using namespace cetty::logging;

ServerBuilder::ServerBuilder() {
    ConfigCenter::instance().configure(&config_);
    init();
}

ServerBuilder::ServerBuilder(int parentThreadCnt, int childThreadCnt) {
    ConfigCenter::instance().configure(&config_);
    config_.parentThreadCount = parentThreadCnt;
    config_.childThreadCount = childThreadCnt;
    init();
}

ServerBuilder::~ServerBuilder() {
}

ServerBuilder& ServerBuilder::registerServer(const std::string& name,
        const ChildInitializer& childInitializer) {
    ChannelOptions empty;
    return registerServer(name,
                          empty,
                          empty,
                          childInitializer);
}

ServerBuilder& ServerBuilder::registerServer(const std::string& name,
        const ChannelOptions& options,
        const ChannelOptions& childOptions,
        const ChildInitializer& childInitializer) {
    if (name.empty()) {
        LOG_WARN << "name is empty, can not register the server.";
        return *this;
    }

    if (!childInitializer) {
        LOG_WARN << "childInitializer is empty, can not register the server.";
        return *this;
    }

    ServerBootstrapPtr bootstrap = new ServerBootstrap(
        parentEventLoopPool_,
        childEventLoopPool_);

    bootstrap->setChildInitializer(childInitializer);

    if (!options.empty()) {
        bootstrap->setChannelOptions(options);
    }

    if (!childOptions.empty()) {
        bootstrap->setChildOptions(childOptions);
    }

    bootstraps_.insert(std::make_pair(name, bootstrap));

    return *this;
}

ServerBuilder& ServerBuilder::setOptions(const std::string& name,
        const ChannelOptions& options,
        const ChannelOptions& childOptions) {
    ServerBootstraps::const_iterator itr = bootstraps_.find(name);

    if (itr != bootstraps_.end()) {
        itr->second->setChannelOptions(options);
        itr->second->setChildOptions(childOptions);
    }
    else {
        LOG_WARN << "can not find the server: "
                 << name
                 << ", do nothing.";
    }

    return *this;
}

cetty::channel::ChannelPtr ServerBuilder::build(const std::string& name,
        int port) {
    return build(name, std::string(), port);
}

ChannelPtr ServerBuilder::build(const std::string& name,
                                const std::string& host,
                                int port) {
    ServerBootstraps::const_iterator itr = bootstraps_.find(name);

    if (itr == bootstraps_.end()) {
        LOG_WARN << "has not found such server: "
                 << name
                 << " in builder registers, should registerServer first.";

        return ChannelPtr();
    }

    return build(itr->second, host, port);
}

ChannelPtr ServerBuilder::build(const std::string& name,
                                const ChildInitializer& initializer,
                                int port) {
    return build(name,
                 ChannelOptions(),
                 ChannelOptions(),
                 initializer,
                 std::string(),
                 port);
}

ChannelPtr ServerBuilder::build(const std::string& name,
                                const ChildInitializer& initializer,
                                const std::string& host,
                                int port) {
    return build(name,
                 ChannelOptions(),
                 ChannelOptions(),
                 initializer,
                 host,
                 port);
}

cetty::channel::ChannelPtr ServerBuilder::build(const std::string& name,
        const ChannelOptions& options,
        const ChannelOptions& childOptions,
        const ChildInitializer& childInitializer,
        int port) {
    return build(name,
                 options,
                 childOptions,
                 childInitializer,
                 std::string(),
                 port);
}

cetty::channel::ChannelPtr ServerBuilder::build(const std::string& name,
        const ChannelOptions& options,
        const ChannelOptions& childOptions,
        const ChildInitializer& childInitializer,
        const std::string& host,
        int port) {
    if (name.empty()) {
        LOG_WARN << "parameter error, empty name or invalid address.";
        return ChannelPtr();
    }

    ServerBootstrap* bootstrap = new ServerBootstrap(
        parentEventLoopPool_,
        childEventLoopPool_);

    bootstraps_.insert(std::make_pair(name, bootstrap));

    if (childOptions.empty()) {
        //bootstrap->setChildOption(ChannelOption::CO_TCP_NODELAY, true);
    }

    if (options.empty()) {
        //bootstrap->setChannelOption(ChannelOption::CO_SO_LINGER, 0);
        //bootstrap->setChannelOption(ChannelOption::CO_SO_REUSEADDR, true);
        bootstrap->setChannelOption(ChannelOption::CO_TCP_NODELAY, true);
        bootstrap->setChannelOption(ChannelOption::CO_SO_BACKLOG, 4096);
        bootstrap->setChannelOption(ChannelOption::CO_SO_REUSEADDR, true);
    }

    if (childInitializer) {
        bootstrap->setChildInitializer(childInitializer);
    }
    else {
        LOG_WARN << "childInitializer is empty, channel will not work fine.";
    }

    return build(bootstrap, host, port);
}

cetty::channel::ChannelPtr ServerBuilder::build(const ServerBootstrapPtr& bootstrap,
        const std::string& host,
        int port) {
    ChannelFuturePtr future;

    if (host.empty()) {
        future = bootstrap->bind(port);
    }
    else {
        future = bootstrap->bind(host, port);
    }

    if (future->await()->isSuccess()) {
        return future->channel();
    }
    else {
        return ChannelPtr();
    }
}

int ServerBuilder::init() {
    if (config_.deamonize && boost::get<bool>(config_.deamonize)) {
        ServerUtil::daemonize();
    }

    if (!parentEventLoopPool_) {
        parentEventLoopPool_ = new AsioServicePool(config_.parentThreadCount);
    }

    if (!childEventLoopPool_) {
        if (config_.childThreadCount > 0) {
            childEventLoopPool_ = new AsioServicePool(config_.childThreadCount);
        }
        else {
            childEventLoopPool_ = parentEventLoopPool_;
        }
    }

    if (config_.logger) {
        Logger::logLevel(LogLevel::parseFrom(config_.logger->level));
    }

    return 0;
}

void ServerBuilder::shutdown() {
    ServerBootstraps::iterator itr;

    for (itr = bootstraps_.begin(); itr != bootstraps_.end(); ++itr) {
        itr->second->shutdown();
    }
}

void ServerBuilder::waitingForExit() {
    ServerBootstraps::iterator itr;

    if (config_.deamonize) {
        ServerUtil::createPidFile(config_.pidfile.c_str());

        for (itr = bootstraps_.begin(); itr != bootstraps_.end(); ++itr) {
            ServerBootstrap::Channels& channels = itr->second->channels();

            for (std::size_t i = 0; i < channels.size(); ++i) {
                channels[i]->closeFuture()->awaitUninterruptibly();
            }
        }
    }
    else {
        printf("Servers: \n");

        for (itr = bootstraps_.begin(); itr != bootstraps_.end(); ++itr) {
            ServerBootstrap::Channels& channels = itr->second->channels();
            ServerBootstrap::Channels::const_iterator itr = channels.begin();
            for (; itr != channels.end(); ++itr) {
                printf("    Channel ID: %d has bind to %s\n",
                       itr->first,
                       itr->second->localAddress().toString().c_str());
            }
        }

        printf("To quit server, press 'q'.\n");

        char input;

        do {
            input = getchar();

            if (input == 'q') {
                shutdown();
                break;
            }
        }
        while (true);
    }
}

ServerBuilder& ServerBuilder::buildAll() {
    std::size_t j = config_.servers.size();
    std::map<std::string, ServerBuilderConfig::Server*>::const_iterator itr =
        config_.servers.begin();
    for (; itr != config_.servers.end(); ++itr) {
        const std::string& name = itr->first;
        const ServerBuilderConfig::Server* server = itr->second;

        if (name.empty()) {
            LOG_WARN << "has not config the server name, will not start the server.";
            continue;
        }

        if (!server) {
            LOG_WARN << "config the server: "
                << name
                << " has no config item, will skip it.";
        }

        if (server->port <= 0) {
            LOG_WARN << "config the server: "
                     << name
                     << " , which port is not great than 0, will skip it.";
            continue;
        }

        ServerBootstraps::const_iterator itr
            = bootstraps_.find(name);

        if (itr == bootstraps_.end()) {
            LOG_WARN << "the server: "
                     << name
                     << "has not register to the builder, should register first.";
            continue;
        }

        //TODO setting the options

        build(itr->second, server->host, server->port);
    }

    return *this;
}

}
}
}
