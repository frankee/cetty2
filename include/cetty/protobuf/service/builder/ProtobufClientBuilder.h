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

#include <cetty/handler/codec/frame/LengthFieldBasedFrameDecoder.h>
#include <cetty/handler/codec/frame/LengthFieldPrepender.h>

#include <cetty/service/builder/ClientBuilder.h>
#include <cetty/protobuf/service/ProtobufServiceMessage.h>
#include <cetty/protobuf/service/handler/ProtobufServiceMessageDecoder.h>
#include <cetty/protobuf/service/handler/ProtobufServiceMessageEncoder.h>
#include <cetty/protobuf/service/handler/ProtobufServiceMessageHandler.h>
#include <cetty/protobuf/service/handler/ProtobufServiceRequestHandler.h>

namespace cetty {
namespace protobuf {
namespace service {
namespace builder {

using namespace cetty::bootstrap;
using namespace cetty::channel;
using namespace cetty::channel::socket::asio;
using namespace cetty::handler::codec::frame;
using namespace cetty::service;
using namespace cetty::service::builder;
using namespace cetty::protobuf::service::handler;

class ProtobufClientBuilder
        : public cetty::service::builder::ClientBuilder<ProtobufServiceMessagePtr, ProtobufServiceMessagePtr> {
public:
    typedef ClientBuilder<ProtobufServiceMessagePtr, ProtobufServiceMessagePtr> ClientBuilderType;

public:
    ProtobufClientBuilder()
        : ClientBuilderType() {
        init();
    }
    ProtobufClientBuilder(int threadCnt)
        : ClientBuilderType(threadCnt) {
        init();
    }
    ProtobufClientBuilder(const AsioServicePoolPtr& ioServicePool)
        : ClientBuilderType(ioServicePool) {
        init();
    }
    ProtobufClientBuilder(const AsioServicePtr& ioService)
        :  ClientBuilderType(ioService) {
        init();
    }

private:
    void init() {
        pipeline = ChannelPipelines::pipeline();

        pipeline->addLast("frameDecoder", new LengthFieldBasedFrameDecoder(16 * 1024 * 1024, 0, 4, 0, 4));
        pipeline->addLast("frameEncoder", new LengthFieldPrepender(4));

        pipeline->addLast("protobufDecoder", new ProtobufServiceMessageDecoder());
        pipeline->addLast("protobufEncoder", new ProtobufServiceMessageEncoder());

        pipeline->addLast("messageHandler", new ProtobufServiceMessageHandler());

        ClientBuilderType::setPipeline(pipeline);
    }

private:
    ChannelPipelinePtr pipeline;
};

}
}
}
}

#endif //#if !defined(CETTY_PROTOBUF_SERVICE_BUILDER_PROTOBUFCLIENTBUILDER_H)

// Local Variables:
// mode: c++
// End:
