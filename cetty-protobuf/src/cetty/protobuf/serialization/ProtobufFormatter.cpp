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

#include <cetty/protobuf/serialization/ProtobufFormatter.h>

#include <cetty/util/StringUtil.h>

namespace cetty {
namespace protobuf {
namespace serialization {

using namespace cetty::util;

std::map<std::string, ProtobufFormatter*> ProtobufFormatter::formatters;

void ProtobufFormatter::format(const std::string& key, boost::int64_t value, std::string* str) {
    std::string v;
    StringUtil::strprintf(&v, "%lld", key);
    format(key, v, str);
}

void ProtobufFormatter::format(const std::string& key, boost::int64_t value, const ChannelBufferPtr& buffer) {

}

void ProtobufFormatter::format(const std::string& key, double value, std::string* str) {

}

void ProtobufFormatter::format(const std::string& key, double value, const ChannelBufferPtr& buffer) {

}

void ProtobufFormatter::format(const std::string& key, const std::vector<boost::int64_t>& value, std::string* str) {

}

void ProtobufFormatter::format(const std::string& key, const std::vector<boost::int64_t>& value, const ChannelBufferPtr& buffer) {

}

void ProtobufFormatter::format(const std::string& key, const std::vector<double>& value, std::string* str) {

}

void ProtobufFormatter::format(const std::string& key, const std::vector<double>& value, const ChannelBufferPtr& buffer) {

}








}
}
}