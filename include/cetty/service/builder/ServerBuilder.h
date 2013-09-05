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
    typedef Channel::PipelineInitializer PipelineInitializer;

public:
    ServerBuilder();
    ServerBuilder(int parentThreadCnt, int childThreadCnt = 0);

    virtual ~ServerBuilder();

    /**
     * build a server with the prototype, which has registered
     *
     * @param name
     * @param port
     */
    ChannelPtr build(const std::string& name, int port);

    /**
     * build a server with the prototype, which has registered
     */
    ChannelPtr build(const std::string& name,
                     const std::string& host,
                     int port);

    /**
     * build a server with the prototype, which has registered,
     * and reset the options.
     */
    ChannelPtr build(const std::string& name,
                     const std::string& host,
                     int port,
                     const ChannelOptions& options,
                     const ChannelOptions& childOptions);

    /**
     * build a new server channel
     */
    ChannelPtr build(const std::string& name,
                     const PipelineInitializer& childPipelineInitializer,
                     int port);

    /**
     * build a new server channel
     */
    ChannelPtr build(const std::string& name,
                     const PipelineInitializer& childPipelineInitializer,
                     const std::string& host,
                     int port);

    /**
     * build a new server channel
     */
    ChannelPtr build(const std::string& name,
                     const PipelineInitializer& childPipelineInitializer,
                     const std::string& host,
                     int port,
                     const ChannelOptions& options,
                     const ChannelOptions& childOptions);

    /**
     * build all server channels which setting in the configure file.
     */
    ServerBuilder& buildAll();

    /**
     * reset the channel options of the server prototype
     *
     * @param name the name of the server prototype
     * @param options
     * @param childOptions
     */
    ServerBuilder& setChannelOptions(const std::string& name,
                                     const ChannelOptions& options,
                                     const ChannelOptions& childOptions);

    /**
     * Waiting for all the {@link channel}s in the {@link ServerBuilder} to close.
     */
    void waitingForExit();

    /**
     *
     */
    ServerBuilder& registerPrototype(const std::string& name,
                                     const PipelineInitializer& childPipelineInitializer);

    /**
     *
     */
    ServerBuilder& registerPrototype(const std::string& name,
                                     const ChannelOptions& options,
                                     const ChannelOptions& childOptions,
                                     const PipelineInitializer& childPipelineInitializer);

    /**
     * only take affection before build made.
     */
    ServerBuilder& setDaemonize(bool daemonize);

    /**
     * only take affection before build made.
     */
    ServerBuilder& setPidfile(const std::string& filename);

    /**
     * any changes, which only before {@link build} made, to the {@link ServerBuilderConfig}
     * will take affection
     */
    ServerBuilderConfig& config();

    /**
     *
     */
    const ServerBuilderConfig& config() const;

    /**
     *
     */
    const EventLoopPoolPtr& pool() const;

    /**
     *
     */
    const EventLoopPoolPtr& childPool() const;

private:
    void init();
    void shutdown();

    ChannelPtr build(const ServerBootstrapPtr& bootstrap,
                     const std::string& host,
                     int port);

    void setChannelOptions(const ServerBootstrapPtr& bootstrap,
                           const ChannelOptions& options,
                           const ChannelOptions& childOptions);

    // read options from configure file
    void readOptions(const std::string& name,
                     ChannelOptions* options,
                     ChannelOptions* childOptions);

private:
    typedef std::map<std::string, ServerBootstrapPtr> ServerBootstraps;

private:
    bool init_;
    ServerBuilderConfig config_;

    EventLoopPoolPtr parentEventLoopPool_;
    EventLoopPoolPtr childEventLoopPool_;

    ServerBootstraps bootstraps_;
};

inline
ServerBuilder& ServerBuilder::setDaemonize(bool daemonize) {
    config_.daemonize = daemonize;
    return *this;
}

inline
ServerBuilder& ServerBuilder::setPidfile(const std::string& filename) {
    config_.pidfile = filename;
    return *this;
}

inline
ServerBuilderConfig& ServerBuilder::config() {
    return config_;
}

inline
const ServerBuilderConfig& ServerBuilder::config() const {
    return config_;
}

inline
const EventLoopPoolPtr& ServerBuilder::pool() const {
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
