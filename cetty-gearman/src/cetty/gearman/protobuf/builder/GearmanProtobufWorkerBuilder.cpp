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

#include <cetty/gearman/protobuf/builder/GearmanProtobufWorkerBuilder.h>

#include <google/protobuf/descriptor.h>

#include <cetty/channel/ChannelPipeline.h>
#include <cetty/handler/codec/LengthFieldBasedFrameDecoder.h>
#include <cetty/protobuf/service/ProtobufService.h>
#include <cetty/protobuf/service/ProtobufServiceMessage.h>
#include <cetty/protobuf/service/handler/ProtobufServiceMessageHandler.h>
#include <cetty/gearman/GearmanWorker.h>
#include <cetty/gearman/GearmanWorkerHandler.h>
#include <cetty/gearman/protocol/GearmanMessageCodec.h>
#include <cetty/gearman/protobuf/GearmanProtobufWorkerFilter.h>

#include <cetty/logging/LoggerHelper.h>

namespace cetty {
namespace gearman {
namespace protobuf {
namespace builder {

using namespace google::protobuf;
using namespace cetty::channel;
using namespace cetty::handler::codec;
using namespace cetty::protobuf::service::handler;
using namespace cetty::gearman::protocol;

bool intializeChannel(ChannelPipeline& pipeline) {
    pipeline.addLast<GearmanProtobufWorkerFilter::HandlerPtr>("gearmanProtobufFilter",
            GearmanProtobufWorkerFilter::HandlerPtr(new GearmanProtobufWorkerFilter));

    pipeline.addLast<ProtobufServiceMessageHandler::HandlerPtr>("protobufMessageHandler",
            ProtobufServiceMessageHandler::HandlerPtr(new ProtobufServiceMessageHandler));

    return true;
}

GearmanProtobufWorkerBuilder::GearmanProtobufWorkerBuilder() {
    builder_.setAdditionalInitializer(boost::bind(&intializeChannel, _1));
}

GearmanProtobufWorkerBuilder::GearmanProtobufWorkerBuilder(int threadCnt)
    : builder_(threadCnt) {
    builder_.setAdditionalInitializer(boost::bind(&intializeChannel, _1));
}

GearmanProtobufWorkerBuilder::GearmanProtobufWorkerBuilder(
    const EventLoopPtr& eventLoop)
    : builder_(eventLoop) {
    builder_.setAdditionalInitializer(boost::bind(&intializeChannel, _1));
}

GearmanProtobufWorkerBuilder::GearmanProtobufWorkerBuilder(
    const EventLoopPoolPtr& eventLoopPool)
    : builder_(eventLoopPool) {
    builder_.setAdditionalInitializer(boost::bind(&intializeChannel, _1));
}

GearmanProtobufWorkerBuilder& GearmanProtobufWorkerBuilder::registerService(
    const ProtobufServicePtr& service) {

    if (!service) {
        LOG_WARN << "registered service is NULL";
        return *this;
    }

    //register service
    ProtobufServiceRegister::instance().registerService(service);

    const ServiceDescriptor* descriptor = service->GetDescriptor();
    BOOST_ASSERT(descriptor);

    std::string serviceName = descriptor->full_name();
    int methodCnt = descriptor->method_count();

    std::string functionName;
    WorkFunctor nullWorker;

    for (int i = 0; i < methodCnt; ++i) {
        const MethodDescriptor* method = descriptor->method(i);
        functionName = method->full_name();

        //register the worker function
        builder_.registerWorker(functionName, nullWorker);
    }

    return *this;
}

}
}
}
}
