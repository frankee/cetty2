#if !defined(CETTY_PROTOBUF_SERVICE_BUILDER_PROTOBUFCLIENTBUILDER_H)
#define CETTY_PROTOBUF_SERVICE_BUILDER_PROTOBUFCLIENTBUILDER_H

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

#include <cetty/protobuf/service/ProtobufServiceMessage.h>
#include <cetty/service/builder/ClientBuilder.h>

namespace cetty {
namespace protobuf {
namespace service {
namespace builder {

using namespace cetty::channel;
using namespace cetty::service::builder;

class ProtobufClientBuilder : private boost::noncopyable {
public:
    typedef ClientBuilder<ProtobufServiceMessagePtr> ClientBuilderType;

public:
    ProtobufClientBuilder();
    ProtobufClientBuilder(int threadCnt);
    ProtobufClientBuilder(const EventLoopPtr& eventLoop);
    ProtobufClientBuilder(const EventLoopPoolPtr& eventLoopPool);

    void setServiceInitializer(const Channel::PipelineInitializer& initializer) {
        builder_.setServiceInitializer(initializer);
    }

    void addConnection(const std::string& host, int port, int limit = 1) {
        builder_.addConnection(host, port, limit);
    }

    ChannelPtr build() {
        return builder_.build();
    }

    const EventLoopPtr& eventLoop() const {
        return builder_.eventLoop();
    }

    const EventLoopPoolPtr& eventLoopPool() const {
        return builder_.eventLoopPool();
    }

private:
    void init();
    bool initializeChannel(ChannelPipeline& pipeline);

private:
    ClientBuilderType builder_;
};

}
}
}
}

#endif //#if !defined(CETTY_PROTOBUF_SERVICE_BUILDER_PROTOBUFCLIENTBUILDER_H)

// Local Variables:
// mode: c++
// End:
