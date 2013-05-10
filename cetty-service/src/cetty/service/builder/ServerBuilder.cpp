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

#include <cetty/channel/NullChannel.h>
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

ServerBuilder::ServerBuilder()
    : init_(false) {
    ConfigCenter::instance().configure(&config_);
    init();
}

ServerBuilder::ServerBuilder(int parentThreadCnt, int childThreadCnt)
    : init_(false) {
    ConfigCenter::instance().configure(&config_);

    config_.parentThreadCount = parentThreadCnt;
    config_.childThreadCount = childThreadCnt;
    init();
}

ServerBuilder::~ServerBuilder() {
}

ServerBuilder& ServerBuilder::registerPrototype(const std::string& name,
        const PipelineInitializer& childPipelineInitializer) {
    ChannelOptions empty;
    return registerPrototype(name,
                             empty,
                             empty,
                             childPipelineInitializer);
}

ServerBuilder& ServerBuilder::registerPrototype(const std::string& name,
        const ChannelOptions& options,
        const ChannelOptions& childOptions,
        const PipelineInitializer& childPipelineInitializer) {
    if (!init_) {
        LOG_WARN << "initialized failed, CAN NOT register server";
        return *this;
    }

    if (name.empty()) {
        LOG_WARN << "name is empty, can not register the server.";
        return *this;
    }

    if (!childPipelineInitializer) {
        LOG_WARN << "childPipelineInitializer is empty, can not register the server.";
        return *this;
    }

    ServerBootstrapPtr bootstrap = new ServerBootstrap(
        parentEventLoopPool_,
        childEventLoopPool_);

    bootstrap->setChildInitializer(childPipelineInitializer);

    setOptions(bootstrap, options, childOptions);

    bootstraps_.insert(std::make_pair(name, bootstrap));

    return *this;
}

ServerBuilder& ServerBuilder::setOptions(const std::string& name,
        const ChannelOptions& options,
        const ChannelOptions& childOptions) {
    ServerBootstraps::const_iterator itr = bootstraps_.find(name);

    if (itr != bootstraps_.end()) {
        if (!options.empty()) {
            itr->second->setOptions(options);
        }

        if (!childOptions.empty()) {
            itr->second->setChildOptions(childOptions);
        }
    }
    else {
        LOG_WARN << "can not find the server: " << name
                 << ", do nothing.";
    }

    return *this;
}

void ServerBuilder::setOptions(const ServerBootstrapPtr& bootstrap,
                               const ChannelOptions& options,
                               const ChannelOptions& childOptions) {
    if (!options.empty()) {
        bootstrap->setOptions(options);
    }
    else {
        bootstrap->setOption(ChannelOption::CO_SO_BACKLOG, 4096);
        bootstrap->setOption(ChannelOption::CO_SO_REUSEADDR, true);
    }

    if (!childOptions.empty()) {
        bootstrap->setChildOptions(childOptions);
    }
    else {
        bootstrap->setChildOption(ChannelOption::CO_TCP_NODELAY, true);
    }
}

ChannelPtr ServerBuilder::build(const std::string& name,
                                int port) {
    return build(name, std::string(), port);
}

ChannelPtr ServerBuilder::build(const std::string& name,
                                const std::string& host,
                                int port) {
    ChannelOptions empty;
    return build(name, host, port, empty, empty);
}

ChannelPtr ServerBuilder::build(const std::string& name,
                                const std::string& host,
                                int port,
                                const ChannelOptions& options,
                                const ChannelOptions& childOptions) {
    ServerBootstraps::const_iterator itr = bootstraps_.find(name);

    if (itr == bootstraps_.end()) {
        LOG_WARN << "has not found such server: "
                 << name
                 << " in builder registers, should registerServer first.";

        return NullChannel::instance();
    }

    const ServerBootstrapPtr& bootstrap = itr->second;
    setOptions(bootstrap, options, childOptions);
    return build(bootstrap, host, port);
}

ChannelPtr ServerBuilder::build(const std::string& name,
                                const PipelineInitializer& initializer,
                                int port) {
    ChannelOptions empty;
    return build(name,
                 initializer,
                 std::string(),
                 port,
                 empty,
                 empty);
}

ChannelPtr ServerBuilder::build(const std::string& name,
                                const PipelineInitializer& initializer,
                                const std::string& host,
                                int port) {
    ChannelOptions empty;
    return build(name,
                 initializer,
                 host,
                 port,
                 empty,
                 empty);
}

ChannelPtr ServerBuilder::build(const std::string& name,
                                const PipelineInitializer& childPipelineInitializer,
                                const std::string& host,
                                int port,
                                const ChannelOptions& options,
                                const ChannelOptions& childOptions) {
    if (name.empty()) {
        LOG_WARN << "parameter error, name should not be empty.";
        return NullChannel::instance();
    }

    if (bootstraps_.find(name) != bootstraps_.end()) {
        LOG_WARN << "the " << name << " server type has already registered, ";
    }

    ServerBootstrap* bootstrap = new ServerBootstrap(
        parentEventLoopPool_,
        childEventLoopPool_);

    bootstraps_.insert(std::make_pair(name, bootstrap));

    setOptions(bootstrap, options, childOptions);

    if (childPipelineInitializer) {
        bootstrap->setInitializer(childPipelineInitializer);
    }
    else {
        LOG_WARN << "childPipelineInitializer is empty, channel will not work fine.";
        return NullChannel::instance();
    }

    bootstrap->setDaemonize(config_.daemonize);

    return build(bootstrap, host, port);
}

ChannelPtr ServerBuilder::build(const ServerBootstrapPtr& bootstrap,
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

void ServerBuilder::init() {
    if (config_.daemonize) {
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
        Logger::setLevel(LogLevel::parseFrom(config_.logger->level));
    }

    init_ = true;
}

void ServerBuilder::shutdown() {
    ServerBootstraps::iterator itr;

    for (itr = bootstraps_.begin(); itr != bootstraps_.end(); ++itr) {
        itr->second->shutdown();
    }
}

void ServerBuilder::waitingForExit() {
    if (bootstraps_.empty()) {
        LOG_INFO << "there is no any servers, bye!";
        return;
    }

    if (config_.daemonize) {
        ServerUtil::createPidFile(config_.pidfile.c_str());
        ServerBootstraps::iterator itr;

        for (itr = bootstraps_.begin(); itr != bootstraps_.end(); ++itr) {
            ServerBootstrap::Channels& channels = itr->second->channels();
            ServerBootstrap::Channels::iterator channelItr = channels.begin();

            for (; channelItr != channels.end(); ++channelItr) {
                channelItr->second->closeFuture()->awaitUninterruptibly();
            }
        }
    }
    else {
        printf("Servers: \n");
        ServerBootstraps::iterator itr;

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

        const ServerBootstrapPtr& bootstrap = itr->second;
        const ServerChannelConfig* serverConfig = server->serverChannel;
        const ChildChannelConfig* childConfig = server->childChannel;

        if (serverConfig) {
            bootstrap->setOption(ChannelOption::CO_SO_REUSEADDR,
                                 serverConfig->reuseAddress)
            .setOption(ChannelOption::CO_SO_BACKLOG,
                       serverConfig->backLog)
            .setOption(ChannelOption::CO_REUSE_CHILD,
                       serverConfig->reuseChild)
            .setOption(ChannelOption::CO_RESERVED_CHILD_COUNT,
                       serverConfig->reservedChildCount);

            if (serverConfig->receiveBufferSize) {
                bootstrap->setOption(ChannelOption::CO_SO_RCVBUF,
                                     *serverConfig->receiveBufferSize);
            }
        }

        if (childConfig) {
            bootstrap->setChildOption(ChannelOption::CO_SO_KEEPALIVE,
                                      childConfig->isKeepAlive)
            .setChildOption(ChannelOption::CO_SO_REUSEADDR,
                            childConfig->isReuseAddress)
            .setChildOption(ChannelOption::CO_TCP_NODELAY,
                            childConfig->isTcpNoDelay);

            if (childConfig->soLinger) {
                bootstrap->setChildOption(ChannelOption::CO_SO_LINGER,
                                          childConfig->soLinger);
            }

            if (childConfig->sendBufferSize) {
                bootstrap->setChildOption(ChannelOption::CO_SO_SNDBUF,
                                          childConfig->sendBufferSize);
            }

            if (childConfig->receiveBufferSize) {
                bootstrap->setChildOption(ChannelOption::CO_SO_RCVBUF,
                                          childConfig->receiveBufferSize);
            }
        }

        build(bootstrap, server->host, server->port);
    }

    return *this;
}

}
}
}
