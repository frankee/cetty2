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

#include <cetty/channel/ChannelHandlerContext.h>
#include <cetty/channel/MessageEvent.h>
#include <cetty/channel/ChannelMessage.h>
#include <cetty/channel/UpstreamMessageEvent.h>
#include <cetty/channel/DownstreamMessageEvent.h>
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

GearmanProtobufClientFilter::GearmanProtobufClientFilter() {}

GearmanProtobufClientFilter::~GearmanProtobufClientFilter() {}


ChannelHandlerPtr GearmanProtobufClientFilter::clone() {
    return GearmanProtobufClientFilterPtr(new GearmanProtobufClientFilter);
}

std::string GearmanProtobufClientFilter::toString() const {
    return "GearmanProtobufClientFilter";
}

void GearmanProtobufClientFilter::messageReceived(ChannelHandlerContext& ctx,
        const MessageEvent& e) {

    ChannelMessage msg = e.getMessage();
    GearmanMessagePtr req = msg.smartPointer<GearmanMessage>();

    //gearmanReqs.push(req->clone());
    ChannelMessage protobufRep(filterReq(req));
    //ctx.getChannel()->write(ChannelMessage(protobufRep));
    UpstreamMessageEvent evt(e.getChannel(),
                             protobufRep,e.getRemoteAddress());

    ctx.sendUpstream(evt);
}

void GearmanProtobufClientFilter::writeRequested(ChannelHandlerContext& ctx,
        const MessageEvent& e) {
    ChannelMessage msg = e.getMessage();
    ProtobufServiceMessagePtr rep = msg.smartPointer<ProtobufServiceMessage>();

    //GearmanMessagePtr req = gearmanReqs.top();
    GearmanMessagePtr req;
    ChannelMessage gearmanRep(filterRep(req,rep));
    DownstreamMessageEvent evt(e.getChannel(),e.getFuture(),
                               gearmanRep,e.getRemoteAddress());

    ctx.sendDownstream(evt);
}

ProtobufServiceMessagePtr GearmanProtobufClientFilter::filterReq(const GearmanMessagePtr& req) {
    //decode from GearmanMessage
    ProtobufServiceMessagePtr protoMsg(new ProtobufServiceMessage);
    ProtobufServiceMessageDecoder::decode(req->getData(),protoMsg);
    return protoMsg;
}

GearmanMessagePtr GearmanProtobufClientFilter::filterRep(const GearmanMessagePtr& req,const ProtobufServiceMessagePtr& rep) {

    const std::string& method = rep->getMethod();
    ChannelBufferPtr buffer = ChannelBuffers::buffer(rep->getMessageSize()+8);

    //encode the protobufServiceMessage and set it to GearmanMessage
    ProtobufServiceMessageEncoder::encodeMessage(buffer,rep);

    return GearmanMessage::createsubmitJobMessage(method,"12345",buffer);
}

}
}
}