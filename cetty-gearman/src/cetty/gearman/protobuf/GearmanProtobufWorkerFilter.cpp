
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

#include <cetty/gearman/protobuf/GearmanProtobufWorkerFilter.h>

#include <string>

#include <cetty/gearman/GearmanMessage.h>
#include <cetty/protobuf/service/ProtobufServiceMessage.h>
#include <cetty/protobuf/service/handler/ProtobufServiceMessageEncoder.h>
#include <cetty/protobuf/service/handler/ProtobufServiceMessageDecoder.h>

namespace cetty {
namespace gearman {
namespace protobuf {
using namespace cetty::channel;
using namespace cetty::gearman;
using namespace cetty::protobuf::service;
using namespace cetty::protobuf::service::handler;

GearmanProtobufWorkerFilter::GearmanProtobufWorkerFilter() {}

GearmanProtobufWorkerFilter::~GearmanProtobufWorkerFilter() {}


ChannelHandlerPtr GearmanProtobufWorkerFilter::clone() {
    return GearmanProtobufWorkerFilterPtr(new GearmanProtobufWorkerFilter);
}

std::string GearmanProtobufWorkerFilter::toString() const {
    return "GearmanProtobufWorkerFilter";
}

ProtobufServiceMessagePtr GearmanProtobufWorkerFilter::filterReq(const GearmanMessagePtr& req) {
    //decode from GearmanMessage
    ProtobufServiceMessagePtr protoMsg(new ProtobufServiceMessage);
    ProtobufServiceMessageDecoder::decode(req->getData(),protoMsg);
    return protoMsg;
}

GearmanMessagePtr GearmanProtobufWorkerFilter::filterRep(const GearmanMessagePtr& req,
        const ProtobufServiceMessagePtr& rep) {
    std::string jobHandle = req->getParameters().front();
    ChannelBufferPtr buffer = ChannelBuffers::buffer(rep->getMessageSize()+8);

    //encode the protobufServiceMessage and set it to GearmanMessage
    ProtobufServiceMessageEncoder::encodeMessage(buffer,rep);
    return GearmanMessage::createWorkCompleteMessage(jobHandle,buffer);
}
}
}
}