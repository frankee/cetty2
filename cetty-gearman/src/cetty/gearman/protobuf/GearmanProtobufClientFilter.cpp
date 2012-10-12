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

#include <cetty/gearman/protobuf/GearmanProtobufClientFilter.h>

#include <string>

#include <cetty/buffer/Unpooled.h>
#include <cetty/gearman/GearmanMessage.h>
#include <cetty/protobuf/service/ProtobufServiceMessage.h>
#include <cetty/protobuf/service/handler/ProtobufServiceMessageEncoder.h>
#include <cetty/protobuf/service/handler/ProtobufServiceMessageDecoder.h>

namespace cetty {
namespace gearman {
namespace protobuf {

using namespace cetty::channel;
using namespace cetty::buffer;
using namespace cetty::gearman;
using namespace cetty::protobuf::service;
using namespace cetty::protobuf::service::handler;

GearmanProtobufClientFilter::GearmanProtobufClientFilter() {}

GearmanProtobufClientFilter::~GearmanProtobufClientFilter() {}


ChannelHandlerPtr GearmanProtobufClientFilter::clone() {
    return GearmanProtobufClientFilterPtr(new GearmanProtobufClientFilter);
}

std::string GearmanProtobufClientFilter::toString() const {
    return "GearmanProtobufClientFilter";
}

GearmanMessagePtr GearmanProtobufClientFilter::filterRequest(
    ChannelHandlerContext& ctx,
    const ProtobufServiceMessagePtr& req) {
    const std::string& method = req->getMethod();
    ChannelBufferPtr buffer
        = Unpooled::buffer(req->getMessageSize() + 8);

    //encode the protobufServiceMessage and set it to GearmanMessage
    ProtobufServiceMessageEncoder::encodeMessage(buffer, req);
    return GearmanMessage::createsubmitJobMessage(method, "12345", buffer);
}

ProtobufServiceMessagePtr GearmanProtobufClientFilter::filterResponse(
    ChannelHandlerContext& ctx,
    const ProtobufServiceMessagePtr& req,
    const GearmanMessagePtr& rep) {
    //decode from GearmanMessage
    ProtobufServiceMessagePtr protoMsg(new ProtobufServiceMessage);
    ProtobufServiceMessageDecoder::decode(rep->getData(),protoMsg);
    return protoMsg;
}

}
}
}
