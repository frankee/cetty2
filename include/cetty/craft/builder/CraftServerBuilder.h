#if !defined(CETTY_CRAFT_BUILDER_CRAFTSERVERBUILDER_H)
#define CETTY_CRAFT_BUILDER_CRAFTSERVERBUILDER_H

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

#include <cetty/channel/ChannelPtr.h>
#include <cetty/protobuf/service/ProtobufServicePtr.h>
#include <cetty/protobuf/service/builder/ProtobufServerBuilder.h>

namespace cetty {
namespace config {
class ConfigCenter;
}
}

namespace cetty {
namespace craft {
namespace builder {

using namespace cetty::channel;
using namespace cetty::protobuf::service;
using namespace cetty::protobuf::service::builder;

class CraftServerBuilder : private boost::noncopyable {
public:
    typedef ChannelPipeline::Initializer PipelineInitializer;

public:
    CraftServerBuilder();
    CraftServerBuilder(int parentThreadCnt, int childThreadCnt = 0);

    ~CraftServerBuilder();

    CraftServerBuilder& registerService(const ProtobufServicePtr& service);

    ChannelPtr build(const std::string& name,
                     const PipelineInitializer& childInitializer,
                     int port);

    ChannelPtr build(const std::string& name,
                     const PipelineInitializer& childInitializer,
                     const std::string& host,
                     int port);

    ChannelPtr buildRpc(int port);
    ChannelPtr buildHttp(int port);

    CraftServerBuilder& buildAll();

    void waitingForExit();

    const ServerBuilderConfig& config() const;
    const EventLoopPoolPtr& parentPool() const;
    const EventLoopPoolPtr& childPool() const;

    ServerBuilder& serverBuilder();
    const ServerBuilder& serverBuilder() const;

private:
    void init();
    bool initializeChildChannel(ChannelPipeline& pipeline);

private:
    ProtobufServerBuilder builder_;
};

inline
ChannelPtr CraftServerBuilder::build(const std::string& name,
                                     const ProtobufServerBuilder::PipelineInitializer& childInitializer,
                                     int port) {
    return builder_.build(name, childInitializer, port);
}

inline
ChannelPtr CraftServerBuilder::build(const std::string& name,
                                     const CraftServerBuilder::PipelineInitializer& childInitializer,
                                     const std::string& host,
                                     int port) {
    return builder_.build(name, childInitializer, host, port);
}

inline
CraftServerBuilder& CraftServerBuilder::buildAll() {
    builder_.buildAll();
    return *this;
}

inline
void CraftServerBuilder::waitingForExit() {
    builder_.waitingForExit();
}

inline
const ServerBuilderConfig& CraftServerBuilder::config() const {
    return builder_.config();
}

inline
const EventLoopPoolPtr& CraftServerBuilder::parentPool() const {
    return builder_.parentPool();
}

inline
const EventLoopPoolPtr& CraftServerBuilder::childPool() const {
    return builder_.childPool();
}

inline
ServerBuilder& CraftServerBuilder::serverBuilder() {
    return builder_.serverBuilder();
}

inline
const ServerBuilder& CraftServerBuilder::serverBuilder() const {
    return builder_.serverBuilder();
}

}
}
}

#endif //#if !defined(CETTY_CRAFT_BUILDER_CRAFTSERVERBUILDER_H)

// Local Variables:
// mode: c++
// End:
