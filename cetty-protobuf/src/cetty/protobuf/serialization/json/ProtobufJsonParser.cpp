#include "cetty/handler/rpc/protobuf/ProtobufJsonParser.h"

#include <google/protobuf/message.h>
#include <google/protobuf/descriptor.h>
#include <json/json.h>

namespace cetty { namespace handler { namespace rpc { namespace protobuf { 

void ProtobufJsonParser::parseFromString(const std::string& str, google::protobuf::Message* message) {
    if (str.empty() || !message) return;

    Json::Value root;
    Json::Reader reader;

    reader.parse(str, root);
    parseMessage(message, root);
}

bool ProtobufJsonParser::parseMessage(google::protobuf::Message* message,
    const Json::Value& value) {

    const google::protobuf::Reflection* reflection = message->GetReflection();
    const google::protobuf::Descriptor* descriptor = message->GetDescriptor();

    int fieldCnt = descriptor->field_count();
    for (int i = 0; i < fieldCnt; ++i) {
        const google::protobuf::FieldDescriptor* field = descriptor->field(i);
        const Json::Value& fieldValue = value[field->camelcase_name()];
        if (!fieldValue.isNull()) {
            if (!parseField(message, field, fieldValue)) {
                return false;
            }
        }
    }

    return true;
}

static int value2Int(const Json::Value& value) {
    if (value.isString()) {
        return atoi(value.asCString());
    }
    return value.asInt();
}

static google::protobuf::int64 value2Int64(const Json::Value& value) {
    if (value.isString()) {
        return atoi(value.asCString());
    }
    return value.asInt64();
}

static double value2Double(const Json::Value& value) {
    if (value.isString()) {
        return atof(value.asCString());
    }
    return value.asDouble();
}

bool ProtobufJsonParser::parseField(google::protobuf::Message* message,
                const google::protobuf::FieldDescriptor* field,
                const Json::Value& value) {
    const google::protobuf::Reflection* reflection = message->GetReflection();

    if (field->is_extension()) {
        //TODO
    }
    else if (field->type() == google::protobuf::FieldDescriptor::TYPE_GROUP) {
        // Groups must be serialized with their original capitalization.
    }

    if (field->is_repeated()) {
        if (!value.isArray()) {
            printf("protobuf field is repeated, but json not.\n");
            return false;
        }

        Json::ArrayIndex count = value.size();
        for (Json::ArrayIndex i = 0; i < count; ++i) {
            switch (field->cpp_type()) {
            case google::protobuf::FieldDescriptor::CPPTYPE_INT32:
                reflection->AddInt32(message, field, value2Int(value[i]));
                break;
            case google::protobuf::FieldDescriptor::CPPTYPE_INT64:
                reflection->AddInt64(message, field, value2Int64(value[i]));
                break;
            case  google::protobuf::FieldDescriptor::CPPTYPE_DOUBLE:
                reflection->AddDouble(message, field, value2Double(value[i]));
                break;
            case google::protobuf::FieldDescriptor::CPPTYPE_STRING:
                reflection->AddString(message, field, value[i].asString());
                break;
            case google::protobuf::FieldDescriptor::CPPTYPE_MESSAGE:
                google::protobuf::Message* msg = reflection->AddMessage(message, field);
                if (!parseMessage(msg, value[i])) {
                    return false;
                }
                break;
            }
        }
    }
    else {
        switch (field->cpp_type()) {
        case google::protobuf::FieldDescriptor::CPPTYPE_INT32:
            reflection->SetInt32(message, field, value2Int(value));
        	break;
        case google::protobuf::FieldDescriptor::CPPTYPE_INT64:
            reflection->SetInt64(message, field, value2Int64(value));
            break;
        case  google::protobuf::FieldDescriptor::CPPTYPE_DOUBLE:
            reflection->SetDouble(message, field, value2Double(value));
            break;
        case google::protobuf::FieldDescriptor::CPPTYPE_STRING:
            reflection->SetString(message, field, value.asString());
            break;
        case google::protobuf::FieldDescriptor::CPPTYPE_MESSAGE:
            google::protobuf::Message* msg = reflection->MutableMessage(message, field);
            return parseMessage(msg, value);
            break;
        }
    }

    return true;
}


}}}}