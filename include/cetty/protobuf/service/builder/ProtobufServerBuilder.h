#if !defined(CETTY_PROTOBUF_SERVICE_BUILDER_PROTOBUFSERVERBUILDER_H)
#define CETTY_PROTOBUF_SERVICE_BUILDER_PROTOBUFSERVERBUILDER_H

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
#include <cetty/service/builder/ServerBuilder.h>
#include <cetty/protobuf/service/ProtobufServicePtr.h>

namespace cetty {
namespace config {
class ConfigCenter;
}
}

namespace cetty {
namespace protobuf {
namespace service {
namespace builder {

using namespace cetty::channel;
using namespace cetty::config;
using namespace cetty::service;
using namespace cetty::service::builder;
using namespace cetty::protobuf::service;

class ProtobufServerBuilder : private boost::noncopyable {
public:
    typedef ServerBuilder::PipelineInitializer PipelineInitializer;

public:
    ProtobufServerBuilder();
    ProtobufServerBuilder(int parentThreadCnt, int childThreadCnt = 0);

    ~ProtobufServerBuilder();

    ProtobufServerBuilder& registerService(const ProtobufServicePtr& service);

    ChannelPtr build(const std::string& name,
                     const PipelineInitializer& childPipelineInitializer,
                     int port);

    ChannelPtr build(const std::string& name,
                     const PipelineInitializer& childPipelineInitializer,
                     const std::string& host,
                     int port);

    ChannelPtr buildRpc(int port);

    ProtobufServerBuilder& buildAll();

    void waitingForExit();

    const ServerBuilderConfig& config() const;
    const EventLoopPoolPtr& parentPool() const;
    const EventLoopPoolPtr& childPool() const;

    ServerBuilder& serverBuilder();
    const ServerBuilder& serverBuilder() const;

private:
    void init();
    bool initializeChannel(ChannelPipeline& pipeline);

private:
    ServerBuilder builder_;
};

inline
ChannelPtr ProtobufServerBuilder::build(const std::string& name,
                                        const ProtobufServerBuilder::PipelineInitializer& childPipelineInitializer,
                                        int port) {
    return builder_.build(name, childPipelineInitializer, port);
}

inline
ChannelPtr ProtobufServerBuilder::build(const std::string& name,
                                        const ProtobufServerBuilder::PipelineInitializer& childPipelineInitializer,
                                        const std::string& host,
                                        int port) {
    return builder_.build(name, childPipelineInitializer, host, port);
}

inline
ProtobufServerBuilder& ProtobufServerBuilder::buildAll() {
    builder_.buildAll();
    return *this;
}

inline
void ProtobufServerBuilder::waitingForExit() {
    builder_.waitingForExit();
}

inline
const ServerBuilderConfig& ProtobufServerBuilder::config() const {
    return builder_.config();
}

inline
const EventLoopPoolPtr& ProtobufServerBuilder::parentPool() const {
    return builder_.pool();
}

inline
const EventLoopPoolPtr& ProtobufServerBuilder::childPool() const {
    return builder_.childPool();
}

inline
ServerBuilder& ProtobufServerBuilder::serverBuilder() {
    return builder_;
}

inline
const ServerBuilder& ProtobufServerBuilder::serverBuilder() const {
    return builder_;
}

}
}
}
}

#endif //#if !defined(CETTY_PROTOBUF_SERVICE_BUILDER_PROTOBUFSERVERBUILDER_H)

// Local Variables:
// mode: c++
// End:
