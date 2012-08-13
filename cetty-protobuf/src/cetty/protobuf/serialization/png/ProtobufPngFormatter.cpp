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

#include <cetty/protobuf/serialization/png/ProtobufPngFormatter.h>

#include <boost/assert.hpp>
#include <cetty/buffer/ChannelBuffer.h>

namespace cetty {
namespace protobuf {
namespace serialization {
namespace png {

using namespace cetty::buffer;
using namespace cetty::protobuf::serialization;

static const char PNG_FILE_SIGNATURE[] = { 0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a };

struct ProtobufPngFormatterRegister {
    ProtobufPngFormatterRegister() {
        ProtobufFormatter::registerFormatter("png", new ProtobufPngFormatter);
    }
} formatterRegister;

void ProtobufPngFormatter::format(const std::string& key, const std::string& value, std::string* str) {
    if (isPng(value)) {
        *str = value;
    }
}

void ProtobufPngFormatter::format(const std::string& key, const std::string& value, const ChannelBufferPtr& buffer) {
    if (isPng(value)) {
        buffer->writeBytes(value);
    }
}

void ProtobufPngFormatter::format(const google::protobuf::Message& value, std::string* str) {
    BOOST_ASSERT(false && "png not support format Message");
}

void ProtobufPngFormatter::format(const google::protobuf::Message& value, const ChannelBufferPtr& buffer) {
    BOOST_ASSERT(false && "png not support format Message");
}

void ProtobufPngFormatter::format(const std::string& key, const std::vector<const std::string*>& value, std::string* str) {
    BOOST_ASSERT(false && "png not support format vector of string");
}

void ProtobufPngFormatter::format(const std::string& key, const std::vector<const std::string*>& value, const ChannelBufferPtr& buffer) {
    BOOST_ASSERT(false && "png not support format vector of string");
}

void ProtobufPngFormatter::format(const std::string& key, const std::vector<const google::protobuf::Message*>& value, std::string* str) {
    BOOST_ASSERT(false && "png not support format vector of Message");
}

void ProtobufPngFormatter::format(const std::string& key, const std::vector<const google::protobuf::Message*>& value, const ChannelBufferPtr& buffer) {
    BOOST_ASSERT(false && "png not support format vector of Message");
}

bool ProtobufPngFormatter::isPng(const std::string& value) {
    return strncmp(value.c_str(),
                   (const char*)PNG_FILE_SIGNATURE,
                   sizeof(PNG_FILE_SIGNATURE)) == 0;
}

}
}
}
}