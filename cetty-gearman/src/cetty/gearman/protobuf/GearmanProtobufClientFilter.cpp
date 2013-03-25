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

#include <cetty/util/StringUtil.h>
#include <cetty/buffer/Unpooled.h>
#include <cetty/gearman/protocol/GearmanMessage.h>
#include <cetty/gearman/protocol/commands/Client.h>

#include <cetty/protobuf/service/ProtobufServiceMessage.h>
#include <cetty/protobuf/service/handler/MessageCodec.h>

namespace cetty {
namespace gearman {
namespace protobuf {

using namespace cetty::util;
using namespace cetty::channel;
using namespace cetty::buffer;
using namespace cetty::gearman::protobuf;
using namespace cetty::protobuf::service;
using namespace cetty::protobuf::service::handler;

GearmanProtobufClientFilter::GearmanProtobufClientFilter()
    : background_(false) {
    filter_.setRequestFilter(boost::bind(&GearmanProtobufClientFilter::filterRequest,
                                         this,
                                         _1,
                                         _2));
    filter_.setResponseFilter(boost::bind(&GearmanProtobufClientFilter::filterResponse,
                                          this,
                                          _1,
                                          _2,
                                          _3));
}

GearmanProtobufClientFilter::~GearmanProtobufClientFilter() {
}

GearmanMessagePtr GearmanProtobufClientFilter::filterRequest(
    ChannelHandlerContext& ctx,
    const ProtobufServiceMessagePtr& req) {
    std::string method;
    req->methodFullName(&method);

    ChannelBufferPtr buffer
        = Unpooled::buffer(req->messageSize() + 8);

    //encode the protobufServiceMessage and set it to GearmanMessage
    MessageCodec::encodeMessage(req, buffer);

    if (background_) {
        return commands::submitJobBGMessage(method,
                                            getUniqueId(req->id()),
                                            buffer);
    }
    else {
        return commands::submitJobMessage(method,
                                          getUniqueId(req->id()),
                                          buffer);
    }
}

const std::string& GearmanProtobufClientFilter::getUniqueId(int64_t id) {
    if (uniqueKey_.empty()) {
        return uniqueKey_;
    }
    else {
        uniqueId_ = uniqueKey_;
        StringUtil::numtostr(id, &uniqueId_);
        return uniqueId_;
    }
}

ProtobufServiceMessagePtr GearmanProtobufClientFilter::filterResponse(
    ChannelHandlerContext& ctx,
    const ProtobufServiceMessagePtr& req,
    const GearmanMessagePtr& rep) {

    //decode from GearmanMessage
    ProtobufServiceMessagePtr protoMsg(new ProtobufServiceMessage);
    MessageCodec::decodeMessage(rep->data(), protoMsg);
    return protoMsg;
}

}
}
}
