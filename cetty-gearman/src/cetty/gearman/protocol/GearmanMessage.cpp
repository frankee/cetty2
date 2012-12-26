
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

#include <cetty/gearman/protocol/GearmanMessage.h>
#include <cetty/util/StringUtil.h>

namespace cetty {
namespace gearman {
namespace protocol {

const std::string GearmanMessage::RESPONSE_MAGIC("\0RES", 4);
const std::string GearmanMessage::REQUEST_MAGIC("\0REQ", 4);

GearmanMessage::GearmanMessage(int type) {
    this->type_ = type;
}
GearmanMessage::GearmanMessage(int type, const ChannelBufferPtr& payload) {
    this->type_ = type;
    this->data_ = payload;
}
GearmanMessage::GearmanMessage(int type, const std::string& param) {
    this->type_ = type;
    this->parameters_.push_back(param);
}
GearmanMessage::GearmanMessage(int type, const std::string& param, const ChannelBufferPtr& payload) {
    this->type_ = type;

    if (param.size()>0) {
        this->parameters_.push_back(param);
    }

    this->data_ = payload;
}

GearmanMessage::GearmanMessage(int type, const std::string& param1, const std::string& param2) {
    this->type_ = type;
    this->parameters_.push_back(param1);
    this->parameters_.push_back(param2);
}

GearmanMessage::GearmanMessage(int type, const std::string& param1, const std::string& param2, const std::string& param3) {
    this->type_ = type;
    this->parameters_.push_back(param1);
    this->parameters_.push_back(param2);
    this->parameters_.push_back(param3);
}
GearmanMessage::GearmanMessage(int type, const std::string& param1, const std::string& param2, const ChannelBufferPtr& payload) {
    this->type_ = type;
    this->parameters_.push_back(param1);
    this->parameters_.push_back(param2);
    this->data_ = payload;
}


//worker




}
}
}
