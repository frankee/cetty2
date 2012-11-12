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

#include <cetty/protobuf/serialization/json/ProtobufJsonFormatter.h>

#include <google/protobuf/descriptor.h>

#include <cetty/Types.h>
#include <cetty/buffer/ChannelBuffer.h>
#include <cetty/util/StringUtil.h>
#include <cetty/logging/LoggerHelper.h>

namespace cetty {
namespace protobuf {
namespace serialization {
namespace json {

using namespace cetty::buffer;
using namespace cetty::util;

using namespace cetty::protobuf::serialization;

template<typename T>
class OutputStream {
public:
    OutputStream& append(const char* str);
    OutputStream& append(const std::string& str);

    template<typename V>
    OutputStream& appendValue(V value);

    OutputStream& appendValue(const char* text);
    OutputStream& appendValue(const std::string& text);
    OutputStream& appendValue(const std::string* value);
    OutputStream& appendValue(int32_t value);
    OutputStream& appendValue(uint32_t value);
    OutputStream& appendValue(int64_t value);
    OutputStream& appendValue(uint64_t value);
    OutputStream& appendValue(float value);
    OutputStream& appendValue(double value);
    OutputStream& appendValue(bool value);
    OutputStream& backward(int cnt = 1);
};

template<>
class OutputStream<std::string*> {
public:
    OutputStream(std::string* output)
        : output(output) {}

    OutputStream& append(const char* str) {
        if (str) {
            output->append(str);
        }

        return *this;
    }

    OutputStream& append(const std::string& str) {
        output->append(str);
        return *this;
    }

    template<typename V>
    OutputStream& appendValue(V value) {
        //output->append()
        return *this;
    }

    OutputStream& appendValue(const char* value) {
        if (value) {
            int length = strlen(value);

            if (length > 1 && value[0] == '\"' && value[length-1] == '\"') {
                output->append(value);
            }
            else {
                output->append("\"");
                output->append(value);
                output->append("\"");
            }
        }
        else {
            output->append("null");
        }

        return *this;
    }

    OutputStream& appendValue(const std::string& value) {
        if (value.size() > 1
                && *value.begin() == '\"'
                && *value.rbegin() == '\"') {
            output->append(value);
        }
        else {
            output->append("\"");
            output->append(value);
            output->append("\"");
        }

        return *this;
    }

    OutputStream& appendValue(const std::string* value) {
        if (value) {
            appendValue(*value);
        }
        else {
            output->append("null");
        }

        return *this;
    }

    OutputStream& appendValue(int32_t value) {
        StringUtil::printf(output, "%d", value);
        return *this;
    }

    OutputStream& appendValue(uint32_t value) {
        StringUtil::printf(output, "%u", value);
        return *this;
    }

    OutputStream& appendValue(int64_t value) {
        StringUtil::printf(output, "%lld", value);
        return *this;
    }

    OutputStream& appendValue(uint64_t value) {
        StringUtil::printf(output, "%llu", value);
        return *this;
    }

    OutputStream& appendValue(float value) {
        StringUtil::printf(output, "%f", value);
        return *this;
    }

    OutputStream& appendValue(double value) {
        StringUtil::printf(output, "%lf", value);
        return *this;
    }

    OutputStream& appendValue(bool value) {
        if (value) {
            output->append("true");
        }
        else {
            output->append("false");
        }

        return *this;
    }

    OutputStream& backward(int cnt = 1) {
        output->resize(output->size() - 1);
        return *this;
    }

private:
    std::string* output;
};

template<>
class OutputStream<ChannelBufferPtr> {
public:
    OutputStream(const ChannelBufferPtr& output) : output(output) {}

    OutputStream& append(const char* str) {
        if (str) {
            output->writeBytes(str);
        }

        return *this;
    }

    OutputStream& append(const std::string& str) {
        output->writeBytes(str);
        return *this;
    }

    template<typename V>
    OutputStream& appendValue(V value) {
        //output->append()
        return *this;
    }

    OutputStream& appendValue(const char* value) {
        if (value) {
            int length = strlen(value);

            if (length > 1 && value[0] == '\"' && value[length-1] == '\"') {
                output->writeBytes(value, length);
            }
            else {
                output->writeByte('\"');
                output->writeBytes(value, length);
                output->writeByte('\"');
            }
        }
        else {
            output->writeBytes("null");
        }

        return *this;
    }

    OutputStream& appendValue(const std::string& value) {
        if (value.size() > 1
                && *value.begin() == '\"'
                && *value.rbegin() == '\"') {
            output->writeBytes(value);
        }
        else {
            output->writeByte('\"');
            output->writeBytes(value);
            output->writeByte('\"');
        }

        return *this;
    }

    OutputStream& appendValue(const std::string* value) {
        if (value) {
            appendValue(*value);
        }
        else {
            output->writeBytes("null");
        }

        return *this;
    }

    OutputStream& appendValue(uint32_t value) {
        output->writeBytes(StringUtil::printf("%u", value));
        return *this;
    }

    OutputStream& appendValue(int32_t value) {
        output->writeBytes(StringUtil::printf("%d", value));
        return *this;
    }

    OutputStream& appendValue(int64_t value) {
        output->writeBytes(StringUtil::printf("%lld", value));
        return *this;
    }

    OutputStream& appendValue(uint64_t value) {
        output->writeBytes(StringUtil::printf("%llu", value));
        return *this;
    }

    OutputStream& appendValue(double value) {
        output->writeBytes(StringUtil::printf("%f", value));
        return *this;
    }

    OutputStream& appendValue(bool value) {
        if (value) {
            output->writeBytes("true");
        }
        else {
            output->writeBytes("false");
        }

        return *this;
    }

    OutputStream& backward(int cnt = 1) {
        output->offsetWriterIndex(0 - cnt);
        return *this;
    }

private:
    ChannelBufferPtr output;
};


template<typename T, int Style>
class JsonPrinter {
public:
    JsonPrinter(const T& output)
        : stream(output), inArray(false), indentLevel(0), objectLevel(0) {
        indent.reserve(INDENT_SIZE * 16);
    }

    JsonPrinter& beginObject() {
        ++objectLevel;

        if (Style) {
            ++indentLevel;
            indent.assign(indentLevel*INDENT_SIZE, ' ');
        }

        stream.append("{");

        return *this;
    }
    JsonPrinter& endObject(bool empty = false) {
        if (!empty) {
            stream.backward(1);
        }

        --objectLevel;

        if (Style) {
            --indentLevel;

            if (!empty) {
                indent.assign(indentLevel*INDENT_SIZE, ' ');
                stream.append("\n").append(indent);
            }
        }

        if (0 == objectLevel) { // root object
            stream.append("}");
        }
        else {
            stream.append("},");
        }

        return *this;
    }

    JsonPrinter& beginArray() {
        inArray = true;

        if (Style) {
            ++indentLevel;
            indent.assign(indentLevel*INDENT_SIZE, ' ');
        }

        stream.append("[");

        return *this;
    }

    JsonPrinter& endArray(bool empty = false) {
        inArray = false;

        if (!empty) {
            stream.backward(1);
        }

        if (Style) {
            --indentLevel;

            if (!empty) {
                indent.assign(indentLevel*INDENT_SIZE, ' ');
                stream.append("\n").append(indent);
            }
        }

        if (0 == objectLevel) { // no object, only array.
            stream.append("]");
        }
        else {
            stream.append("],");
        }

        return *this;
    }

    JsonPrinter& printKey(const std::string& name) {
        if (Style) {
            stream.append("\n").append(indent);
        }

        stream.appendValue(name).append(":");
        return *this;
    }

    template<typename V>
    JsonPrinter& printValue(V value) {
        if (Style) {
            if (inArray) {
                stream.append("\n").append(indent);
            }
        }

        stream.appendValue(value);
        stream.append(",");

        return *this;
    }

    JsonPrinter& printRawValue(const std::string& value) {
        if (Style) {
            if (inArray) {
                stream.append("\n").append(indent);
            }
        }

        stream.append(value);
        stream.append(",");

        return *this;
    }

    JsonPrinter& print(const std::string& value) {
        stream.append(value);
        return *this;
    }

    JsonPrinter& print(const char* value) {
        stream.append(value);
        return *this;
    }

    // print values
    template<typename V>
    JsonPrinter& operator<<(V value) {
        return printValue(value);
    }

private:
    static const int INDENT_SIZE = 4;

private:
    OutputStream<T> stream;

    bool inArray;

    std::string indent;
    int indentLevel;

    int  objectLevel;
};

template<typename T, int U> inline
void printFieldValue(const google::protobuf::Message& message,
                     const google::protobuf::FieldDescriptor* field,
                     JsonPrinter<T, U>& printer) {
    const google::protobuf::Reflection* reflection = message.GetReflection();

    switch (field->cpp_type()) {
    case google::protobuf::FieldDescriptor::CPPTYPE_INT32:
        printer << reflection->GetInt32(message, field);
        break;

    case google::protobuf::FieldDescriptor::CPPTYPE_INT64:
        printer << reflection->GetInt64(message, field);
        break;

    case google::protobuf::FieldDescriptor::CPPTYPE_FLOAT:
        printer << reflection->GetFloat(message, field);
        break;

    case google::protobuf::FieldDescriptor::CPPTYPE_DOUBLE:
        printer << reflection->GetDouble(message, field);
        break;

    case google::protobuf::FieldDescriptor::CPPTYPE_BOOL:
        printer << reflection->GetBool(message, field);
        break;

    case google::protobuf::FieldDescriptor::CPPTYPE_UINT32:
        printer << reflection->GetUInt32(message, field);
        break;

    case google::protobuf::FieldDescriptor::CPPTYPE_UINT64:
        printer << reflection->GetUInt64(message, field);
        break;

    case google::protobuf::FieldDescriptor::CPPTYPE_STRING:
        printer << reflection->GetString(message, field);
        break;

    case google::protobuf::FieldDescriptor::CPPTYPE_ENUM:
        printer << reflection->GetEnum(message, field)->name();
        break;

    case google::protobuf::FieldDescriptor::CPPTYPE_MESSAGE:
        printer.beginObject();
        printer.endObject(!printMessage(reflection->GetMessage(message, field), printer));
        break;

    default:
        break;
    }
}

template<typename T, int U> inline
void printSingleField(const google::protobuf::Message& message,
                      const google::protobuf::FieldDescriptor* field,
                      JsonPrinter<T, U>& printer) {
    if (field->is_extension()) {
        printer.printKey(field->camelcase_name());
    }
    else if (field->type() == google::protobuf::FieldDescriptor::TYPE_GROUP) {
        // Groups must be serialized with their original capitalization.
        printer.printKey(field->message_type()->name());
    }
    else {
        printer.printKey(field->camelcase_name());
    }

    if (field->is_repeated()) {
        // Repeated field-> append each element.
        printer.beginArray();
        printer.endArray(!printFieldRepeatedValue(message, field, printer));
    }
    else {
        printFieldValue(message, field, printer);
    }
}

template<typename T, int U> inline
bool printFieldRepeatedValue(const google::protobuf::Message& message,
                             const google::protobuf::FieldDescriptor* field,
                             JsonPrinter<T, U>& printer) {
    const google::protobuf::Reflection* reflection = message.GetReflection();
    int fieldsize = reflection->FieldSize(message, field);

    for (int i = 0; i < fieldsize; ++i) {
        switch (field->cpp_type()) {
        case google::protobuf::FieldDescriptor::CPPTYPE_INT32:
            printer << reflection->GetRepeatedInt32(message, field, i);
            break;

        case google::protobuf::FieldDescriptor::CPPTYPE_INT64:
            printer << reflection->GetRepeatedInt64(message, field, i);
            break;

        case google::protobuf::FieldDescriptor::CPPTYPE_FLOAT:
            printer << reflection->GetRepeatedFloat(message, field, i);
            break;

        case google::protobuf::FieldDescriptor::CPPTYPE_DOUBLE:
            printer << reflection->GetRepeatedDouble(message, field, i);
            break;

        case google::protobuf::FieldDescriptor::CPPTYPE_BOOL:
            printer << reflection->GetRepeatedBool(message, field, i);
            break;

        case google::protobuf::FieldDescriptor::CPPTYPE_UINT32:
            printer << reflection->GetRepeatedUInt32(message, field, i);
            break;

        case google::protobuf::FieldDescriptor::CPPTYPE_UINT64:
            printer << reflection->GetRepeatedUInt64(message, field, i);
            break;

        case google::protobuf::FieldDescriptor::CPPTYPE_STRING:
            printer << reflection->GetRepeatedString(message, field, i);
            break;

        case google::protobuf::FieldDescriptor::CPPTYPE_ENUM:
            printer << reflection->GetRepeatedEnum(message, field, i)->name();
            break;

        case google::protobuf::FieldDescriptor::CPPTYPE_MESSAGE:
            printer.beginObject();
            printer.endObject(!printMessage(
                                  reflection->GetRepeatedMessage(message, field, i),
                                  printer));
            break;

        default:
            break;
        }
    }

    return fieldsize > 0;
}

template<typename T, int U> inline
bool printField(const google::protobuf::Message& message,
                const google::protobuf::FieldDescriptor* field,
                JsonPrinter<T, U>& printer) {
    const google::protobuf::Reflection* reflection = message.GetReflection();

    //FIXME
    if (field->is_repeated() || reflection->HasField(message, field)) {
        printSingleField(message, field, printer);
        return true;
    }

    return false;
}

const std::string& getEncodedField(const google::protobuf::Message& message) {
    static std::string empty;

    const google::protobuf::Reflection* reflection = message.GetReflection();
    const google::protobuf::Descriptor* descriptor = message.GetDescriptor();

    int fieldCnt = descriptor->field_count();
    const google::protobuf::FieldDescriptor* field = NULL;

    for (int i = 0; i < fieldCnt; ++i) {
        field = descriptor->field(i);

        if (field->type() == google::protobuf::FieldDescriptor::TYPE_BYTES
                && field->name() == "encoded") {
            return reflection->GetStringReference(message, field, NULL);
        }
    }

    return empty;
}

template<typename T, int U> inline
bool printMessage(const google::protobuf::Message& message,
                  JsonPrinter<T, U>& printer) {

    bool printed = false;
    const google::protobuf::Reflection* reflection = message.GetReflection();
    const google::protobuf::Descriptor* descriptor = message.GetDescriptor();

    int fieldCnt = descriptor->field_count();
    const google::protobuf::FieldDescriptor* field = NULL;

    // if has encoded field as whole message, just use it.
    const std::string& value = getEncodedField(message);

    if (!value.empty()) {
        printer.printRawValue(value);
        return true;
    }

    for (int i = 0; i < fieldCnt; ++i) {
        field = descriptor->field(i);
        bool serialized = printField(message, field, printer);

        if (serialized) {
            printed = true;
        }
    }

    // append extension fields.
    std::vector<const google::protobuf::FieldDescriptor*> fields;
    std::vector<const google::protobuf::FieldDescriptor*> extensionFields;
    reflection->ListFields(message, &fields);

    std::size_t j = fields.size();

    for (std::size_t i = 0; i < j; ++i) {
        const google::protobuf::FieldDescriptor* field = fields[i];

        if (field->is_extension()) {
            extensionFields.push_back(field);
        }
    }

    j = extensionFields.size();

    for (std::size_t i = 0; i < j; ++i) {
        bool serialized = printField(message, extensionFields[i], printer);

        if (serialized) {
            printed = true;
        }
    }

    return printed;
}

template<typename T, int U>
void doFormat(const std::string& key,
              const std::string& value,
              const T& output) {
    if (output) {
        JsonPrinter<T, U> printer(output);
        printer.beginObject().printKey(key).printValue(value).endObject();
    }
}

template<typename T, int U>
void doFormat(const google::protobuf::Message& value,
              const T& output) {
    if (output) {
        JsonPrinter<T, U> printer(output);

        const std::string& encodedValue = getEncodedField(value);

        if (!encodedValue.empty()) {
            printer.print(encodedValue);
        }
        else {
            printer.beginObject();
            printer.endObject(!printMessage(value, printer));
        }
    }
}

template<typename T, int U>
void doFormat(const std::string& key,
              const std::vector<const std::string*>& value,
              const T& output) {
    if (output) {
        JsonPrinter<T, U> printer(output);

        bool isObject = !key.empty();

        if (isObject) {
            printer.beginObject().printKey(key);
        }

        printer.beginArray();

        for (std::size_t i = 0, j = value.size(); i < j; ++i) {
            printer.printValue(value[i]);
        }

        printer.endArray();

        if (isObject) {
            printer.endObject();
        }
    }
}

template<typename T, int U>
void doFormat(const std::string& key,
              const std::vector<const google::protobuf::Message*>& value,
              const T& output) {
    if (output) {
        JsonPrinter<T, U> printer(output);

        bool isObject = !key.empty();

        if (isObject) {
            printer.beginObject().printKey(key);
        }

        printer.beginArray();

        for (std::size_t i = 0, j = value.size(); i < j; ++i) {
            const google::protobuf::Message* msg = value[i];

            if (msg) {
                printMessage(*value[i], printer);
            }
            else {
                printer.printValue("null");
            }
        }

        printer.endArray();

        if (isObject) {
            printer.endObject();
        }
    }
}

void ProtobufJsonFormatter::format(const google::protobuf::Message& value,
                                   std::string* str) {
    if (style) {
        doFormat<std::string*, 1>(value, str);
    }
    else {
        doFormat<std::string*, 0>(value, str);
    }
}

void ProtobufJsonFormatter::format(const google::protobuf::Message& value,
                                   const ChannelBufferPtr& buffer) {
    if (style) {
        doFormat<ChannelBufferPtr, 1>(value, buffer);
    }
    else {
        doFormat<ChannelBufferPtr, 0>(value, buffer);
    }
}

void ProtobufJsonFormatter::format(const std::string& key,
                                   const std::string& value,
                                   std::string* str) {
    if (style) {
        doFormat<std::string*, 1>(key, value, str);
    }
    else {
        doFormat<std::string*, 0>(key, value, str);
    }
}

void ProtobufJsonFormatter::format(const std::string& key,
                                   const std::string& value,
                                   const ChannelBufferPtr& buffer) {
    if (style) {
        doFormat<ChannelBufferPtr, 1>(key, value, buffer);
    }
    else {
        doFormat<ChannelBufferPtr, 0>(key, value, buffer);
    }
}

void ProtobufJsonFormatter::format(const std::string& key,
                                   const std::vector<const std::string*>& value,
                                   std::string* str) {
    if (style) {
        doFormat<std::string*, 1>(key, value, str);
    }
    else {
        doFormat<std::string*, 0>(key, value, str);
    }
}

void ProtobufJsonFormatter::format(const std::string& key,
                                   const std::vector<const std::string*>& value,
                                   const ChannelBufferPtr& buffer) {
    if (style) {
        doFormat<ChannelBufferPtr, 1>(key, value, buffer);
    }
    else {
        doFormat<ChannelBufferPtr, 0>(key, value, buffer);
    }
}

void ProtobufJsonFormatter::format(const std::string& key,
                                   const std::vector<const google::protobuf::Message*>& value,
                                   std::string* str) {
    if (style) {
        doFormat<std::string*, 1>(key, value, str);
    }
    else {
        doFormat<std::string*, 0>(key, value, str);
    }

}

void ProtobufJsonFormatter::format(const std::string& key,
                                   const std::vector<const google::protobuf::Message*>& value,
                                   const ChannelBufferPtr& buffer) {

    if (style) {
        doFormat<ChannelBufferPtr, 1>(key, value, buffer);
    }
    else {
        doFormat<ChannelBufferPtr, 0>(key, value, buffer);
    }
}

}
}
}
}
