
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

#include <cetty/service/builder/ServerBuilderConfig.h>

namespace cetty {
namespace service {
namespace builder {

using namespace cetty::config;

CETTY_CONFIG_ADD_DESCRIPTOR(ServerBuilderConfig_Server,
                            new ConfigDescriptor(
                                3,
                                CETTY_CONFIG_FIELD(ServerBuilderConfig::Server, port, INT32),
                                CETTY_CONFIG_FIELD(ServerBuilderConfig::Server, host, STRING),
                                CETTY_CONFIG_FIELD(ServerBuilderConfig::Server, name, STRING)),
                            new ServerBuilderConfig::Server);

CETTY_CONFIG_ADD_DESCRIPTOR(ServerBuilderConfig_Limit,
                            new ConfigDescriptor(
                                1,
                                CETTY_CONFIG_FIELD(ServerBuilderConfig::Limit, maxConcurrentRequests, INT32)),
                            new ServerBuilderConfig::Limit);

CETTY_CONFIG_ADD_DESCRIPTOR(ServerBuilderConfig_Timeout,
                            new ConfigDescriptor(
                                1,
                                CETTY_CONFIG_FIELD(ServerBuilderConfig::Timeout, hostConnectionMaxIdleTime, INT32)),
                            new ServerBuilderConfig::Timeout);

CETTY_CONFIG_ADD_DESCRIPTOR(ServerBuilderConfig,
                            new ConfigDescriptor(
                                10,
                                CETTY_CONFIG_FIELD(ServerBuilderConfig, deamonize, INT32),
                                CETTY_CONFIG_FIELD(ServerBuilderConfig, pidfile, STRING),
                                CETTY_CONFIG_REPEATED_OBJECT_FIELD(ServerBuilderConfig, servers, Server),
                                CETTY_CONFIG_FIELD(ServerBuilderConfig, parentThreadCount, INT32),
                                CETTY_CONFIG_FIELD(ServerBuilderConfig, childThreadCount, INT32),
                                CETTY_CONFIG_FIELD(ServerBuilderConfig, backlog, INT32),
                                CETTY_CONFIG_FIELD(ServerBuilderConfig, logger, STRING),
                                CETTY_CONFIG_FIELD(ServerBuilderConfig, logLevel, STRING),
                                CETTY_CONFIG_OBJECT_FIELD(ServerBuilderConfig, limit, Limit),
                                CETTY_CONFIG_OBJECT_FIELD(ServerBuilderConfig, timeout, Timeout)),
                            new ServerBuilderConfig);


ServerBuilderConfig::Server::Server() : ConfigObject("ServerBuilderConfig_Server") {

}

ServerBuilderConfig::Limit::Limit() : ConfigObject("ServerBuilderConfig_Limit") {

}

ServerBuilderConfig::Timeout::Timeout() : ConfigObject("ServerBuilderConfig_Timeout") {

}

ServerBuilderConfig::ServerBuilderConfig() : ConfigObject("ServerBuilderConfig"),
    deamonize(0),
    parentThreadCount(1),
    childThreadCount(0),
    backlog(4096),
    limit(),
    timeout() {

}

}
}
}
