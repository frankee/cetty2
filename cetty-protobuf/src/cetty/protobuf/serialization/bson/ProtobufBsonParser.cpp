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

#include <cetty/protobuf/serialization/bson/ProtobufBsonParser.h>

#include <google/protobuf/message.h>
#include <google/protobuf/descriptor.h>

#if defined(_WIN32)
#include <WinSock2.h>
#include <Windows.h>
#endif
#include <mongo/client/dbclient.h>

namespace cetty {
namespace protobuf {
namespace serialization {
namespace bson {

using namespace google::protobuf;
using namespace mongo;

void ProtobufBsonParser::parse(const BSONObj& obj, google::protobuf::Message* message) {
    if (obj.isEmpty() || !message) {
        return;
    }

    const google::protobuf::Reflection* reflection = message->GetReflection();
    const google::protobuf::Descriptor* descriptor = message->GetDescriptor();

    int fieldCnt = descriptor->field_count();

    for (int i = 0; i < fieldCnt; ++i) {
        const google::protobuf::FieldDescriptor* field = descriptor->field(i);
        const std::string& fieldName = field->camelcase_name();

        if (!fieldName.empty() && obj.hasField(fieldName.c_str())) {
            BSONElement element = obj.getField(fieldName.c_str());

            if (!parseField(message, field, element, fieldName)) {
                return;
            }
        }
    }
}

bool ProtobufBsonParser::parseField(google::protobuf::Message* message, const google::protobuf::FieldDescriptor* field, const BSONElement& element, const std::string& fieldName) {
    const google::protobuf::Reflection* reflection = message->GetReflection();

    if (field->is_extension()) {
        //TODO
    }
    else if (field->type() == google::protobuf::FieldDescriptor::TYPE_GROUP) {
        // Groups must be serialized with their original capitalization.
    }

    if (field->is_repeated()) {
        if (mongo::Array != element.type()) {
            printf("protobuf field is repeated, but BsonElement not.\n");
            return false;
        }

        vector<BSONElement> elements = element.Array();
        size_t count = elements.size();

        FieldDescriptor::CppType cppType = field->cpp_type();

        switch (cppType) {
        case FieldDescriptor::CPPTYPE_UINT32:
        case FieldDescriptor::CPPTYPE_INT32: {

            for (size_t i = 0; i < count; ++i) {
                reflection->AddInt32(message, field, elements[i].Int());
            }

            break;
        }

        case FieldDescriptor::CPPTYPE_UINT64:
        case FieldDescriptor::CPPTYPE_INT64: {

            for (size_t i = 0; i < count; ++i) {
                reflection->AddInt64(message, field, elements[i].Long());
            }

            break;
        }

        case FieldDescriptor::CPPTYPE_DOUBLE: {

            for (size_t i = 0; i < count; ++i) {
                reflection->AddDouble(message, field, elements[i].Double());
            }

            break;
        }

        case FieldDescriptor::CPPTYPE_STRING: {

            for (size_t i = 0; i < count; ++i) {
                std::string str = elements[i].toString(false,false);

                if (*str.begin() == '"' && *str.rbegin() == '"') {
                    str = str.substr(1,str.length() - 2);
                }

                reflection->AddString(message, field, str/*elements[i].String()*/);
            }

            break;
        }

        case FieldDescriptor::CPPTYPE_MESSAGE: {

            for (size_t i = 0; i < count; ++i) {
                google::protobuf::Message* msg = reflection->AddMessage(message, field);
                parse(elements[i].Obj(),msg);
            }

            break;
        }

        case FieldDescriptor::CPPTYPE_BOOL: {

            for (size_t i = 0; i < count; ++i) {
                reflection->AddBool(message, field, elements[i].Bool());
            }

            break;
        }

        default: {
            break;
        }

        }
    }
    else {
        switch (field->cpp_type()) {
        case google::protobuf::FieldDescriptor::CPPTYPE_BOOL: {
            reflection->SetBool(message, field, element.Bool());
            break;
        }

        case google::protobuf::FieldDescriptor::CPPTYPE_INT32:
            reflection->SetInt32(message, field, element.Int());
            break;

        case google::protobuf::FieldDescriptor::CPPTYPE_INT64:
            reflection->SetInt64(message, field, element.Long());
            break;

        case  google::protobuf::FieldDescriptor::CPPTYPE_DOUBLE:
            reflection->SetDouble(message, field, element.Double());
            break;

        case google::protobuf::FieldDescriptor::CPPTYPE_STRING: {
            if (element.type() == mongo::Object) {
                std::string str = element.jsonString(mongo::Strict, false);
                reflection->SetString(message, field, str);
            }
            else {
                std::string str = element.toString(false,true);
                str = str.substr(1,str.length() - 2);
                reflection->SetString(message, field, str);
            }
        }
        break;

        case google::protobuf::FieldDescriptor::CPPTYPE_MESSAGE:
            google::protobuf::Message* msg = reflection->MutableMessage(message, field);
            parse(element.Obj(),msg);
            break;
        }
    }

    return true;
}

}
}
}
}
