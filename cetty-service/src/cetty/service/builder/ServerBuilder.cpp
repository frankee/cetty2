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
#include <cetty/bootstrap/asio/AsioServerBootstrap.h>
#include <cetty/config/ConfigCenter.h>
#include <cetty/logging/Logger.h>
#include <cetty/logging/LoggerHelper.h>

#if (defined(linux) || defined(__linux) || defined(__linux__))

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

#endif

namespace cetty {
namespace service {
namespace builder {

using namespace cetty::channel;
using namespace cetty::channel::asio;
using namespace cetty::bootstrap::asio;
using namespace cetty::service;
using namespace cetty::config;
using namespace cetty::logging;

#if (defined(linux) || defined(__linux) || defined(__linux__))

/* Anti-warning macro... */
#define CETTY_NOTUSED(V) ((void) V)

static void daemonize() {
    int fd;

    if (fork() != 0) { exit(0); } /* parent exits */

    pid_t sid = setsid(); /* create a new session for the child process */

    if (sid < 0) {
        /* Log the failure */
        exit(EXIT_FAILURE);
    }

    /* Change the current working directory */
    if ((chdir("/")) < 0) {
        /* Log the failure */
        exit(EXIT_FAILURE);
    }

    /* Every output goes to /dev/null. */
    if ((fd = open("/dev/null", O_RDWR, 0)) != -1) {
        dup2(fd, STDIN_FILENO);
        dup2(fd, STDOUT_FILENO);
        dup2(fd, STDERR_FILENO);

        if (fd > STDERR_FILENO) {
            close(fd);
        }
    }
    else { /* Close out the standard file descriptors */
        close(STDIN_FILENO);
        close(STDOUT_FILENO);
        close(STDERR_FILENO);
    }
}

void createPidFile(const char* pidfile) {
    if (NULL == pidfile) { return; }

    /* Try to write the pid file in a best-effort way. */
    FILE* fp = fopen(pidfile, "w");

    if (fp) {
        fprintf(fp,"%d\n",(int)getpid());
        fclose(fp);
    }
}

static void sigtermHandler(int sig) {
    CETTY_NOTUSED(sig);

    //LOGGER(LOGGER_WARN,"Received SIGTERM, scheduling shutdown...");
    //server.shutdown_asap = 1;
}

#undef CETTY_NOTUSED

void setupSignalHandlers() {
    signal(SIGHUP, SIG_IGN);
    signal(SIGPIPE, SIG_IGN);

    struct sigaction act;

    /* When the SA_SIGINFO flag is set in sa_flags then sa_sigaction is used.
     * Otherwise, sa_handler is used. */
    sigemptyset(&act.sa_mask);
    act.sa_flags = SA_NODEFER | SA_ONSTACK | SA_RESETHAND;
    act.sa_handler = sigtermHandler;
    sigaction(SIGTERM, &act, NULL);

    return;
}

#else

static void daemonize() {}
static void createPidFile(const char* pidfile) {}

#endif

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

    ServerBootstrapPtr bootstrap = new AsioServerBootstrap(
        parentEventLoopPool_,
        childEventLoopPool_);

    bootstrap->setChildInitializer(childInitializer);

    if (!options.empty()) {
        bootstrap->setOptions(options);
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
        itr->second->setOptions(options);
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
    return build(name, ChannelOptions(), ChannelOptions(), initializer, std::string(), port);
}

ChannelPtr ServerBuilder::build(const std::string& name,
                                const ChildInitializer& initializer,
                                const std::string& host,
                                int port) {
    return build(name, ChannelOptions(), ChannelOptions(), initializer, host, port);
}

cetty::channel::ChannelPtr ServerBuilder::build(const std::string& name,
        const ChannelOptions& options,
        const ChannelOptions& childOptions,
        const ChildInitializer& childInitializer,
        int port) {
    return build(name, options, childOptions, childInitializer, std::string(), port);
}

cetty::channel::ChannelPtr ServerBuilder::build(const std::string& name,
        const ChannelOptions& options,
        const ChannelOptions& childOptions,
        const ChildInitializer& childInitializer,
        const std::string& host,
        int port) {
    if (name.empty()) {
        printf("parameter error, empty name or invalid address.");
        return ChannelPtr();
    }

    AsioServerBootstrap* bootstrap = new AsioServerBootstrap(
        parentEventLoopPool_,
        childEventLoopPool_);

    bootstraps_.insert(std::make_pair(name, bootstrap));


    if (childOptions.empty()) {
        //bootstrap->setChildOption(ChannelOption::CO_TCP_NODELAY, true);
    }

    if (options.empty()) {
        //bootstrap->setOption(ChannelOption::CO_SO_LINGER, 0);
        //bootstrap->setOption(ChannelOption::CO_SO_REUSEADDR, true);
        bootstrap->setOption(ChannelOption::CO_TCP_NODELAY, true);
        bootstrap->setOption(ChannelOption::CO_SO_BACKLOG, 4096);
        bootstrap->setOption(ChannelOption::CO_SO_REUSEADDR, true);
    }

    if (childInitializer) {
        bootstrap->setChildInitializer(childInitializer);
    }
    else {
        LOG_WARN << "childInitializer is empty, channel will not work fine.";
    }

    return build(bootstrap, host, port);
}

cetty::channel::ChannelPtr ServerBuilder::build(const ServerBootstrapPtr& bootstrap, const std::string& host, int port) {
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
    if (config_.deamonize) {
        daemonize();
    }

    if (!parentEventLoopPool_) {
        parentEventLoopPool_ = new AsioServicePool(config_.parentThreadCount);
    }

    if (!childEventLoopPool_) {
        if (config_.childThreadCount) {
            childEventLoopPool_ = new AsioServicePool(config_.childThreadCount);
        }
        else {
            childEventLoopPool_ = parentEventLoopPool_;
        }
    }

    Logger::logLevel(LogLevel::parseFrom(config_.logLevel));

    return 0;
}

void ServerBuilder::shutdown() {
    ServerBootstraps::iterator itr;

    for (itr = bootstraps_.begin(); itr != bootstraps_.end(); ++itr) {
        itr->second->shutdown();
    }
}

ServerBuilder& ServerBuilder::waitingForExit() {
    if (config_.deamonize) {
        createPidFile(config_.pidfile.c_str());

        ServerBootstraps::iterator itr;
        for (itr = bootstraps_.begin(); itr != bootstraps_.end(); ++itr) {
            const std::vector<ChannelPtr> channels =
                itr->second->serverChannels();

            for (std::size_t i = 0; i < channels.size(); ++i) {
                channels[i]->closeFuture()->awaitUninterruptibly();
            }
        }
    }
    else {
        printf("Server is running...\n");
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

    return *this;
}

ServerBuilder& ServerBuilder::buildAll() {
    std::size_t j = config_.servers.size();

    for (std::size_t i = 0; i < j; ++i) {
        const ServerBuilderConfig::Server& server = *config_.servers[i];
        const std::string& name = server.name;

        if (name.empty()) {
            LOG_WARN << "has not config the server name, will not start the server.";
            continue;
        }

        if (server.port == 0) {
            LOG_WARN << "config the server: "
                     << name
                     << " , which port is 0, will skip it.";
            continue;
        }

        ServerBootstraps::const_iterator itr
            = bootstraps_.find(server.name);

        if (itr == bootstraps_.end()) {
            LOG_WARN << "the server: "
                     << name
                     << "has not register to the builder, should register first.";
            continue;
        }

        //TODO setting the options

        build(itr->second, server.host, server.port);
    }

    return *this;
}

}
}
}
