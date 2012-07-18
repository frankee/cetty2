#include "cetty/handler/rpc/protobuf/ProtobufJsonFormatter.h"

#include <google/protobuf/message.h>
#include <google/protobuf/descriptor.h>

namespace cetty { namespace handler { namespace rpc { namespace protobuf { 

void ProtobufJsonFormatter::serializeToString(const google::protobuf::Message& message, std::string* str) {
    if (str) {
        JsonGenerator json(*str);
        json.append("{");
        serializeMessage(message, json);
        json.append("}");
    }
}

void ProtobufJsonFormatter::serializeMessage(const google::protobuf::Message& message, JsonGenerator& json) {
    const google::protobuf::Reflection* reflection = message.GetReflection();
    const google::protobuf::Descriptor* descriptor = message.GetDescriptor();

    int fieldCnt = descriptor->field_count();
    for (int i = 0; i < fieldCnt; ++i) {
        const google::protobuf::FieldDescriptor* field = descriptor->field(i);
        bool serialized = serializeField(message, field, json);

        if (serialized) {
            json.append(",");
        }
    }

    std::vector<const google::protobuf::FieldDescriptor*> fields;
    std::vector<const google::protobuf::FieldDescriptor*> extentionFields;
    reflection->ListFields(message, &fields);

    std::size_t j = fields.size();
    for (std::size_t i = 0; i < j; ++i) {
        const google::protobuf::FieldDescriptor* field = fields[i];
        if (field->is_extension()) {
            extentionFields.push_back(field);
        }
    }

    j = extentionFields.size();
    
    for (std::size_t i = 0; i < j; ++i) {
        bool serialized = serializeField(message, extentionFields[i], json);
        if (serialized) {
            json.append(",");
        }
    }
    json.eatLastComma();

    //TODO process UnknownFields
}

bool ProtobufJsonFormatter::serializeField(const google::protobuf::Message& message, const google::protobuf::FieldDescriptor* field, JsonGenerator& json) {
    const google::protobuf::Reflection* reflection = message.GetReflection();
    //FIXME
    if (field->is_repeated() || reflection->HasField(message, field)) {
        serializeSingleField(message, field, json);
        return true;
    }
    return false;
}

void ProtobufJsonFormatter::serializeSingleField(const google::protobuf::Message& message, const google::protobuf::FieldDescriptor* field, JsonGenerator& json) {
    if (field->is_extension()) {
        json.append("\"");
        json.append(field->camelcase_name());
        json.append("\"");
    }
    else if (field->type() == google::protobuf::FieldDescriptor::TYPE_GROUP) {
        // Groups must be serialized with their original capitalization.
        json.append("\"");
        json.append(field->message_type()->name());
        json.append("\"");
    }
    else {
        json.append("\"");
        json.append(field->camelcase_name());
        json.append("\"");
    }

    // Done with the name, on to the value
    json.append(":");

    //if (field->cpp_type() == google::protobuf::FieldDescriptor::CPPTYPE_MESSAGE) {
    //    json.indent();
    //}

    if (field->is_repeated()) {
        // Repeated field-> append each element.
        json.append("[");
        printFieldRepeatedValue(message, field, json);
        json.append("]");
    }
    else {
        printFieldValue(message, field, json);
        if (field->cpp_type() == google::protobuf::FieldDescriptor::CPPTYPE_MESSAGE) {
            //json.outdent();
        }
    }
}

void ProtobufJsonFormatter::printFieldRepeatedValue(const google::protobuf::Message& message, const google::protobuf::FieldDescriptor* field, JsonGenerator& json) {
    const google::protobuf::Reflection* reflection = message.GetReflection();
    int fieldsize = reflection->FieldSize(message, field);

    for (int i = 0; i < fieldsize; ++i) {
        switch (field->cpp_type()) {
        case google::protobuf::FieldDescriptor::CPPTYPE_INT32:
            json.append(reflection->GetRepeatedInt32(message, field, i));
            break;
        case google::protobuf::FieldDescriptor::CPPTYPE_INT64:
            json.append(reflection->GetRepeatedInt64(message, field, i));
            break;
        case google::protobuf::FieldDescriptor::CPPTYPE_FLOAT:
            json.append(reflection->GetRepeatedFloat(message, field, i));
            break;
        case google::protobuf::FieldDescriptor::CPPTYPE_DOUBLE:
            json.append(reflection->GetRepeatedDouble(message, field, i));
            break;
        case google::protobuf::FieldDescriptor::CPPTYPE_BOOL:
            json.append(reflection->GetRepeatedBool(message, field, i));
            break;
        case google::protobuf::FieldDescriptor::CPPTYPE_UINT32:
            json.append(reflection->GetRepeatedUInt32(message, field, i));
            break;
        case google::protobuf::FieldDescriptor::CPPTYPE_UINT64:
            json.append(reflection->GetRepeatedUInt64(message, field, i));
            break;
        case google::protobuf::FieldDescriptor::CPPTYPE_STRING: {
            std::string value = reflection->GetRepeatedString(message, field, i);
            if (value.empty()) {
                json.append("null");
            }
            else {
                json.append("\"");
                json.append(value);
                json.append("\"");
            }
            break;
                                                                }
        case google::protobuf::FieldDescriptor::CPPTYPE_ENUM: {
            json.append("\"");
            json.append(reflection->GetRepeatedEnum(message, field, i)->name());
            json.append("\"");
            break;
                                                              }
        case google::protobuf::FieldDescriptor::CPPTYPE_MESSAGE: {
            json.append("{");
            serializeMessage(reflection->GetRepeatedMessage(message, field, i), json);
            json.append("}");
            break;
                                                                 }
        default:
            break;
        }

        if (i < fieldsize -1) {
            json.append(",");
        }
    }
}

void ProtobufJsonFormatter::printFieldValue(const google::protobuf::Message& message, const google::protobuf::FieldDescriptor* field, JsonGenerator json) {
    const google::protobuf::Reflection* reflection = message.GetReflection();

    switch (field->cpp_type()) {
    case google::protobuf::FieldDescriptor::CPPTYPE_INT32:
        json.append(reflection->GetInt32(message, field));
        break;
    case google::protobuf::FieldDescriptor::CPPTYPE_INT64:
        json.append(reflection->GetInt64(message, field));
        break;
    case google::protobuf::FieldDescriptor::CPPTYPE_FLOAT:
        json.append(reflection->GetFloat(message, field));
        break;
    case google::protobuf::FieldDescriptor::CPPTYPE_DOUBLE:
        json.append(reflection->GetDouble(message, field));
        break;
    case google::protobuf::FieldDescriptor::CPPTYPE_BOOL:
        json.append(reflection->GetBool(message, field));
        break;
    case google::protobuf::FieldDescriptor::CPPTYPE_UINT32:
        json.append(reflection->GetUInt32(message, field));
        break;
    case google::protobuf::FieldDescriptor::CPPTYPE_UINT64:
        json.append(reflection->GetUInt64(message, field));
        break;
    case google::protobuf::FieldDescriptor::CPPTYPE_STRING: {
        std::string value = reflection->GetString(message, field);
        if (value.empty()) {
            json.append("null");
        }
        else {
            json.append("\"");
            json.append(value);
            json.append("\"");
        }
        break;
                                                            }
    case google::protobuf::FieldDescriptor::CPPTYPE_ENUM: {
        json.append("\"");
        json.append(reflection->GetEnum(message, field)->name());
        json.append("\"");
        break;
                                                          }
    case google::protobuf::FieldDescriptor::CPPTYPE_MESSAGE: {
        json.append("{");
        serializeMessage(reflection->GetMessage(message, field), json);
        json.append("}");
        break;
                                                             }
    default:
        break;
    }
}


void ProtobufJsonFormatter::JsonGenerator::indent() {
    output.append("  ");
}

void ProtobufJsonFormatter::JsonGenerator::outdent() {
    std::string::size_type length = output.size();
    if (length > 2) {
        output.resize(length - 2);
    }
}

void ProtobufJsonFormatter::JsonGenerator::append(const char* text) {
    output.append(text);
    //             int size = text.length();
    //             int pos = 0;
    // 
    //             for (int i = 0; i < size; i++) {
    //                 if (text.charAt(i) == '\n') {
    //                     write(text.subSequence(pos, size), i - pos + 1);
    //                     pos = i + 1;
    //                     atStartOfLine = true;
    //                 }
    //             }
    //             write(text.subSequence(pos, size), size - pos);
}

void ProtobufJsonFormatter::JsonGenerator::append(const std::string& text) {
    output.append(text);
}

void ProtobufJsonFormatter::JsonGenerator::append(int32 value) {
    char buf[128] = {0};
    sprintf(buf, "%d", value);
    output.append(buf);
}

void ProtobufJsonFormatter::JsonGenerator::append(uint32 value) {
    char buf[128] = {0};
    sprintf(buf, "%u", value);
    output.append(buf);
}

void ProtobufJsonFormatter::JsonGenerator::append(int64 value) {
    char buf[128] = {0};

#if defined(_WIN32)
    sprintf(buf, "%I64d", value);
#else
    sprintf(buf, "%lld", value);
#endif

    output.append(buf);
}

void ProtobufJsonFormatter::JsonGenerator::append(uint64 value) {
    char buf[128] = {0};

#if defined(_WIN32)
    sprintf(buf, "%I64u", value);
#else
    sprintf(buf, "%llu", value);
#endif

    output.append(buf);
}

void ProtobufJsonFormatter::JsonGenerator::append(double value) {
    char buf[128] = {0};
    sprintf(buf, "%lf", value);
    output.append(buf);
}

void ProtobufJsonFormatter::JsonGenerator::append(bool value) {
    if (value) {
        output.append("true");
    }
    else {
        output.append("false");
    }
}

void ProtobufJsonFormatter::JsonGenerator::append(const char* data, int size) {
    output.append(data, size);

    //             if (size == 0) {
    //                 return;
    //             }
    // 
    //             if (atStartOfLine) {
    //                 atStartOfLine = false;
    //                 output.append(indent);
    //             }
    //             output.append(data);
}

void ProtobufJsonFormatter::JsonGenerator::eatLastComma() {
    if (output.back() == ',') {
        output.resize(output.size()-1);
    }
}

}}}}
