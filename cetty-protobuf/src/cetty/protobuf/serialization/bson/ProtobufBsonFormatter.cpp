
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

#include <cetty/protobuf/serialization/bson/ProtobufBsonFormatter.h>

#include <stdio.h>

#include <google/protobuf/message.h>
#include <google/protobuf/descriptor.h>

#if defined(_WIN32)
#include <WinSock2.h>
#include <Windows.h>
#endif
#include <mongo/client/dbclient.h>

#include <cetty/logging/LoggerHelper.h>

#if defined GetMessage
#undef GetMessage
#endif

namespace cetty {
namespace protobuf {
namespace serialization {
namespace bson {

using namespace std;
using namespace mongo;

using namespace google::protobuf;
using namespace google::protobuf;

void ProtobufBsonFormatter::format(const google::protobuf::Message& message, BSONObj* obj) {
    if (obj) {
        BSONObjBuilder objBuilder;
        formatMessage(message, objBuilder);
        *obj = objBuilder.obj();
    }
    else {
        LOG_WARN << "format message to bson, but obj is NULL.";
    }
}

void ProtobufBsonFormatter::formatMessage(const google::protobuf::Message& message,
    BSONObjBuilder& builder) {
    const google::protobuf::Reflection* reflection = message.GetReflection();
    const google::protobuf::Descriptor* descriptor = message.GetDescriptor();

    int fieldCnt = descriptor->field_count();

    for (int i = 0; i < fieldCnt; ++i) {
        bool serialized = formatField(message, descriptor->field(i), builder);
    }
}

bool ProtobufBsonFormatter::formatField(const google::protobuf::Message& message,
    const google::protobuf::FieldDescriptor* field,
    BSONObjBuilder& builder) {
    const google::protobuf::Reflection* reflection = message.GetReflection();

    //FIXME
    if (field->is_repeated() || reflection->HasField(message, field)) {
        formatSingleField(message, field, builder);
        return true;
    }

    return false;
}

void ProtobufBsonFormatter::formatSingleField(const google::protobuf::Message& message,
    const google::protobuf::FieldDescriptor* field,
    BSONObjBuilder& builder) {
    std::string fieldName("");

    if (field->is_extension()) {
        //TODO
    }
    else if (field->type() == google::protobuf::FieldDescriptor::TYPE_GROUP) {
        // Groups must be serialized with their original capitalization.
        fieldName = field->message_type()->name().c_str();
        //...append values
    }
    else {
        fieldName = field->camelcase_name();
        const google::protobuf::Reflection* reflection = message.GetReflection();

        if (field->is_repeated()) {
            int fieldsize = reflection->FieldSize(message, field);

            switch (field->cpp_type()) {
            case FieldDescriptor::CPPTYPE_INT32:    {       //= 1,     // TYPE_INT32, TYPE_SINT32, TYPE_SFIXED32
                std::vector<int32> values;
                values.reserve(fieldsize);

                for (int i = 0; i < fieldsize; ++i) {
                    values.push_back(reflection->GetRepeatedInt32(message,field,i));
                }

                builder.append(fieldName,values);

                break;
            }

            case FieldDescriptor::CPPTYPE_INT64:    {       //= 2,     // TYPE_INT64, TYPE_SINT64, TYPE_SFIXED64
                std::vector<long long> values;
                values.reserve(fieldsize);

                for (int i = 0; i < fieldsize; ++i) {
                    values.push_back(reflection->GetRepeatedInt64(message,field,i));
                }

                builder.append(fieldName, values);

                break;
            }

            case FieldDescriptor::CPPTYPE_UINT32:   {       //= 3,     // TYPE_UINT32, TYPE_FIXED32
                std::vector<uint32> values;
                values.reserve(fieldsize);

                for (int i = 0; i < fieldsize; ++i) {
                    values.push_back(reflection->GetRepeatedUInt32(message,field,i));
                }

                builder.append(fieldName,values);

                break;
            }

            case FieldDescriptor::CPPTYPE_UINT64:   {       //= 4,     // TYPE_UINT64, TYPE_FIXED64
                std::vector<long long> values;
                values.reserve(fieldsize);

                for (int i = 0; i < fieldsize; ++i) {
                    values.push_back((long long)reflection->GetRepeatedUInt64(message,field,i));
                }

                builder.append(fieldName,values);

                break;
            }

            case FieldDescriptor::CPPTYPE_DOUBLE:   {       //= 5,     // TYPE_DOUBLE
                std::vector<double> values;
                values.reserve(fieldsize);

                for (int i = 0; i < fieldsize; ++i) {
                    values.push_back(reflection->GetRepeatedDouble(message,field,i));
                }

                builder.append(fieldName,values);

                break;
            }

            case FieldDescriptor::CPPTYPE_FLOAT:    {       //= 6,     // TYPE_FLOAT
                std::vector<float> values;
                values.reserve(fieldsize);

                for (int i = 0; i < fieldsize; ++i) {
                    values.push_back(reflection->GetRepeatedFloat(message,field,i));
                }

                builder.append(fieldName,values);

                break;
            }

            case FieldDescriptor::CPPTYPE_BOOL:     {       //= 7,     // TYPE_BOOL
                std::vector<bool> values;
                values.reserve(fieldsize);

                for (int i = 0; i < fieldsize; ++i) {
                    values.push_back(reflection->GetRepeatedBool(message,field,i));
                }

                builder.append(fieldName,values);

                break;
            }

            case FieldDescriptor::CPPTYPE_STRING:   {       //= 9,     // TYPE_STRING, TYPE_BYTES

                std::vector<std::string> values;
                values.reserve(fieldsize);

                for (int i = 0; i < fieldsize; ++i) {
                    values.push_back(reflection->GetRepeatedString(message,field,i));
                }

                builder.append(fieldName,values);

                break;
            }

            case FieldDescriptor::CPPTYPE_ENUM:     {       //= 8,     // TYPE_ENUM
                std::vector<std::string> values;
                values.reserve(fieldsize);

                for (int i = 0; i < fieldsize; ++i) {
                    values.push_back(reflection->GetRepeatedEnum(message,field,i)->name());
                }

                builder.append(fieldName,values);

                break;
            }

            case FieldDescriptor::CPPTYPE_MESSAGE:  {       //= 10,    // TYPE_MESSAGE, TYPE_GROUP
                BSONObjBuilder sub(builder.subarrayStart(fieldName));

                for (int i = 0; i < fieldsize; ++i)  {
                    char number[16] = {0};
                    sprintf(number, "%d", i);
                    BSONObjBuilder obj(sub.subobjStart(number));
                    formatMessage(reflection->GetRepeatedMessage(message, field, i), obj);
                    obj.done();
                }

                sub.done();

                break;
            }

            default:                                {
                break;
            }
            }// end switch
        }
        else { //not repeated
            switch (/*cppType*/field->cpp_type()) {
            case FieldDescriptor::CPPTYPE_INT32:    {       //= 1,     // TYPE_INT32, TYPE_SINT32, TYPE_SFIXED32
                builder.append(fieldName, reflection->GetInt32(message,field));
                break;
            }

            case FieldDescriptor::CPPTYPE_INT64:    {       //= 2,     // TYPE_INT64, TYPE_SINT64, TYPE_SFIXED64
                builder.append(fieldName,
                    static_cast<long long>(reflection->GetInt64(message,field)));
                break;
            }

            case FieldDescriptor::CPPTYPE_UINT32:   {       //= 3,     // TYPE_UINT32, TYPE_FIXED32
                builder.append(fieldName,reflection->GetUInt32(message,field));
                break;
            }

            case FieldDescriptor::CPPTYPE_UINT64:   {       //= 4,     // TYPE_UINT64, TYPE_FIXED64
                builder.append(fieldName,
                    static_cast<long long>(reflection->GetUInt64(message,field)));
                break;
            }

            case FieldDescriptor::CPPTYPE_DOUBLE:   {       //= 5,     // TYPE_DOUBLE
                builder.append(fieldName,reflection->GetDouble(message,field));
                break;
            }

            case FieldDescriptor::CPPTYPE_FLOAT:    {       //= 6,     // TYPE_FLOAT
                builder.append(fieldName,reflection->GetFloat(message,field));
                break;
            }

            case FieldDescriptor::CPPTYPE_BOOL:     {       //= 7,     // TYPE_BOOL
                builder.append(fieldName,reflection->GetBool(message,field));
                break;
            }

            case FieldDescriptor::CPPTYPE_STRING:   {       //= 9,     // TYPE_STRING, TYPE_BYTES
                builder.append(fieldName,reflection->GetString(message,field));
                break;
            }

            case FieldDescriptor::CPPTYPE_ENUM:     {       //= 8,     // TYPE_ENUM
                builder.append(fieldName,reflection->GetEnum(message,field)->name());
                break;
            }

            case FieldDescriptor::CPPTYPE_MESSAGE:  {       //= 10,    // TYPE_MESSAGE, TYPE_GROUP
                BSONObjBuilder sub(builder.subobjStart(fieldName));
                formatMessage(reflection->GetMessage(message, field), sub);
                sub.done();
                break;
            }

            default:                                {
                break;
            }
            }// end switch
        }
    } //end else
}

}
}
}
}
