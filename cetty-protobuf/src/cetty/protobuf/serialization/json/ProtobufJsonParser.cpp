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

#include <cetty/protobuf/serialization/json/ProtobufJsonParser.h>

#include <google/protobuf/message.h>
#include <google/protobuf/descriptor.h>
#include <yaml-cpp/yaml.h>

namespace cetty {
namespace protobuf {
namespace serialization {
namespace json {

using namespace cetty::protobuf::serialization;

int ProtobufJsonParser::parse(const char* buffer,
                              int bufferSize,
                              google::protobuf::Message* message) {
    if (!buffer || bufferSize <= 0 || !message) {
        return -1;
    }

    std::string str(buffer, bufferSize);
    YAML::Node root = YAML::Load(str);

    if (root) {
        return parseMessage(root, message);
    }

    return -2;
}

int ProtobufJsonParser::parseMessage(const YAML::Node& node,
                                     google::protobuf::Message* message) {
    BOOST_ASSERT(node && message);

    const google::protobuf::Descriptor* descriptor = message->GetDescriptor();

    int fieldCnt = descriptor->field_count();

    for (int i = 0; i < fieldCnt; ++i) {
        const google::protobuf::FieldDescriptor* field = descriptor->field(i);
        YAML::Node fieldNode = node[field->camelcase_name()];

        if (fieldNode) {
            if (parseField(fieldNode, field, message) < 0) {
                return -3;
            }
        }
    }

    return 0;
}

int ProtobufJsonParser::parseField(const YAML::Node& node,
                                   const google::protobuf::FieldDescriptor* field,
                                   google::protobuf::Message* message) {
    const google::protobuf::Reflection* reflection = message->GetReflection();

    if (field->is_extension()) {
        //TODO
    }
    else if (field->type() == google::protobuf::FieldDescriptor::TYPE_GROUP) {
        // Groups must be serialized with their original capitalization.
    }

    if (field->is_repeated()) {
        //if (!value.isArray()) {
        //    printf("protobuf field is repeated, but json not.\n");
        //    return false;
        //}

        YAML::Node::const_iterator itr = node.begin();

        for (; itr != node.end(); ++itr) {
            switch (field->cpp_type()) {
            case google::protobuf::FieldDescriptor::CPPTYPE_INT32:
                reflection->AddInt32(message, field, itr->as<google::protobuf::int32>());
                break;

            case google::protobuf::FieldDescriptor::CPPTYPE_INT64:
                reflection->AddInt64(message, field, itr->as<google::protobuf::int64>());
                break;

            case  google::protobuf::FieldDescriptor::CPPTYPE_DOUBLE:
                reflection->AddDouble(message, field, itr->as<double>());
                break;

            case google::protobuf::FieldDescriptor::CPPTYPE_STRING:
                reflection->AddString(message, field, itr->Scalar());
                break;

            case google::protobuf::FieldDescriptor::CPPTYPE_MESSAGE:
                google::protobuf::Message* msg = reflection->AddMessage(message, field);

                if (parseMessage(*itr, msg) < 0) {
                    return -3;
                }

                break;
            }
        }
    }
    else {
        switch (field->cpp_type()) {
        case google::protobuf::FieldDescriptor::CPPTYPE_INT32:
            reflection->SetInt32(message, field, node.as<google::protobuf::int32>());
            break;

        case google::protobuf::FieldDescriptor::CPPTYPE_INT64:
            reflection->SetInt64(message, field, node.as<google::protobuf::int64>());
            break;

        case  google::protobuf::FieldDescriptor::CPPTYPE_DOUBLE:
            reflection->SetDouble(message, field, node.as<double>());
            break;

        case google::protobuf::FieldDescriptor::CPPTYPE_STRING:
            reflection->SetString(message, field, node.Scalar());
            break;

        case google::protobuf::FieldDescriptor::CPPTYPE_MESSAGE:
            google::protobuf::Message* msg = reflection->MutableMessage(message, field);
            return parseMessage(node, msg);
            break;
        }
    }

    return 0;
}

}
}
}
}
