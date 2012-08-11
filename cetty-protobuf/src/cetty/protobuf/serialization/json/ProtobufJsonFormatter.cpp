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

#include <boost/cstdint.hpp>
#include <google/protobuf/message.h>
#include <google/protobuf/descriptor.h>

#include <cetty/util/StringUtil.h>

namespace cetty {
namespace protobuf {
namespace serialization {
namespace json {

using namespace cetty::util;

using namespace cetty::protobuf::serialization;

struct ProtobufJsonFormatterRegister {
    ProtobufJsonFormatterRegister() {
        ProtobufFormatter::registerFormatter("json", new ProtobufJsonFormatter);
    }
} formatterRegister;

/**
 * An inner class for writing text to the output stream.
 */
class JsonPrinter {
public:
    JsonPrinter(std::string* output) : output(output), styled(false) {
    }

    ~JsonPrinter() {}

    /**
     * Indent text by two spaces. After calling Indent(), two spaces will be inserted at the
     * beginning of each line of text. Indent() may be called multiple times to produce deeper
     * indents.
     */
    void indent() {
        output->append("  ");
    }

    /**
     * Reduces the current indent level by two spaces, or crashes if the indent level is zero.
     */
    void outdent() {
        std::string::size_type length = output->size();

        if (length > 2) {
            output->resize(length - 2);
        }
    }

    void eatLastComma() {
        std::string::size_type size = output->size();

        if (output->at(size -1) == ',') {
            output->resize(size - 1);
        }
    }

    JsonPrinter& operator<<(const char* text) {
        append(text);
        return *this;
    }

    JsonPrinter& operator<<(const std::string& text) {
        append(text);
        return *this;
    }

    /**
     * Print text to the output stream.
     */
    void append(const char* text) {
        output->append(text);
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

    void append(const std::string& text) {
        append(text.c_str());
    }

    void append(boost::uint32_t value) {
        StringUtil::strprintf(output, "%u", value);
    }

    void append(boost::int32_t value) {
        StringUtil::strprintf(output, "%d", value);
    }

    void append(boost::int64_t value) {
        StringUtil::strprintf(output, "%lld", value);
    }

    void append(boost::uint64_t value) {
        StringUtil::strprintf(output, "%llu", value);
    }

    void append(double value) {
        StringUtil::strprintf(output, "%lf", value);
    }

    void append(bool value) {
        if (value) {
            output->append("true");
        }
        else {
            output->append("false");
        }
    }

    void append(const char* data, int size) {
        output->append(data, size);

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

private:
    bool styled;
    std::string* output;
};

void ProtobufJsonFormatter::format(boost::int64_t value, std::string* str) {
    StringUtil::strprintf(str, "%lld", value);
}

void ProtobufJsonFormatter::format(boost::int64_t value, const ChannelBufferPtr& buffer) {

}

void ProtobufJsonFormatter::format(const std::string& value, std::string* str) {
    if (utf8Check(value)) {

    }
    else {

    }
}

void ProtobufJsonFormatter::format(const std::string& value, const ChannelBufferPtr& buffer) {

}

void ProtobufJsonFormatter::format(const google::protobuf::Message& value, std::string* str) {
    if (str) {
        JsonPrinter printer(str);
        printer << "{";
        printMessage(value, printer);
        printer << "}";
    }
}

void ProtobufJsonFormatter::format(const google::protobuf::Message& value, const ChannelBufferPtr& buffer) {

}

void ProtobufJsonFormatter::format(std::vector<boost::int64_t>& value, std::string* str) {

}

void ProtobufJsonFormatter::format(std::vector<boost::int64_t>& value, const ChannelBufferPtr& buffer) {

}

void ProtobufJsonFormatter::format(std::vector<const std::string*>& value, std::string* str) {

}

void ProtobufJsonFormatter::format(std::vector<const std::string*>& value, const ChannelBufferPtr& buffer) {

}

void ProtobufJsonFormatter::format(std::vector<const google::protobuf::Message*>& value, std::string* str) {

}

void ProtobufJsonFormatter::format(std::vector<const google::protobuf::Message*>& value, const ChannelBufferPtr& buffer) {

}

void ProtobufJsonFormatter::printMessage(const google::protobuf::Message& message,
        JsonPrinter& printer) {

    const google::protobuf::Reflection* reflection = message.GetReflection();
    const google::protobuf::Descriptor* descriptor = message.GetDescriptor();

    int fieldCnt = descriptor->field_count();

    for (int i = 0; i < fieldCnt; ++i) {
        const google::protobuf::FieldDescriptor* field = descriptor->field(i);
        bool serialized = printField(message, field, printer);

        if (serialized) {
            printer.append(",");
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
        bool serialized = printField(message, extentionFields[i], printer);

        if (serialized) {
            printer.append(",");
        }
    }

    printer.eatLastComma();

    //TODO process UnknownFields
}

bool ProtobufJsonFormatter::printField(const google::protobuf::Message& message,
                                       const google::protobuf::FieldDescriptor* field,
                                       JsonPrinter& printer) {
    const google::protobuf::Reflection* reflection = message.GetReflection();

    //FIXME
    if (field->is_repeated() || reflection->HasField(message, field)) {
        printSingleField(message, field, printer);
        return true;
    }

    return false;
}

void ProtobufJsonFormatter::printSingleField(const google::protobuf::Message& message,
        const google::protobuf::FieldDescriptor* field,
        JsonPrinter& printer) {
    if (field->is_extension()) {
        printer << "\"" << field->camelcase_name() << "\"";
    }
    else if (field->type() == google::protobuf::FieldDescriptor::TYPE_GROUP) {
        // Groups must be serialized with their original capitalization.
        printer << "\"" << field->message_type()->name() << "\"";
    }
    else {
        printer << "\"" << field->camelcase_name() << "\"";
    }

    // Done with the name, on to the value
    printer.append(":");

    //if (field->cpp_type() == google::protobuf::FieldDescriptor::CPPTYPE_MESSAGE) {
    //    printer.indent();
    //}

    if (field->is_repeated()) {
        // Repeated field-> append each element.
        printer.append("[");
        printFieldRepeatedValue(message, field, printer);
        printer.append("]");
    }
    else {
        printFieldValue(message, field, printer);

        if (field->cpp_type() == google::protobuf::FieldDescriptor::CPPTYPE_MESSAGE) {
            //printer.outdent();
        }
    }
}

void ProtobufJsonFormatter::printFieldRepeatedValue(const google::protobuf::Message& message, const google::protobuf::FieldDescriptor* field, JsonPrinter& printer) {
    const google::protobuf::Reflection* reflection = message.GetReflection();
    int fieldsize = reflection->FieldSize(message, field);

    for (int i = 0; i < fieldsize; ++i) {
        switch (field->cpp_type()) {
        case google::protobuf::FieldDescriptor::CPPTYPE_INT32:
            printer.append(reflection->GetRepeatedInt32(message, field, i));
            break;

        case google::protobuf::FieldDescriptor::CPPTYPE_INT64:
            printer.append(reflection->GetRepeatedInt64(message, field, i));
            break;

        case google::protobuf::FieldDescriptor::CPPTYPE_FLOAT:
            printer.append(reflection->GetRepeatedFloat(message, field, i));
            break;

        case google::protobuf::FieldDescriptor::CPPTYPE_DOUBLE:
            printer.append(reflection->GetRepeatedDouble(message, field, i));
            break;

        case google::protobuf::FieldDescriptor::CPPTYPE_BOOL:
            printer.append(reflection->GetRepeatedBool(message, field, i));
            break;

        case google::protobuf::FieldDescriptor::CPPTYPE_UINT32:
            printer.append(reflection->GetRepeatedUInt32(message, field, i));
            break;

        case google::protobuf::FieldDescriptor::CPPTYPE_UINT64:
            printer.append(reflection->GetRepeatedUInt64(message, field, i));
            break;

        case google::protobuf::FieldDescriptor::CPPTYPE_STRING: {
            std::string value = reflection->GetRepeatedString(message, field, i);

            if (value.empty()) {
                printer.append("null");
            }
            else {
                printer << "\"" << value << "\"";
            }

            break;
        }

        case google::protobuf::FieldDescriptor::CPPTYPE_ENUM: {
            printer << "\""
                    << reflection->GetRepeatedEnum(message, field, i)->name()
                    << "\"";
            break;
        }

        case google::protobuf::FieldDescriptor::CPPTYPE_MESSAGE: {
            printer.append("{");
            printMessage(reflection->GetRepeatedMessage(message, field, i), printer);
            printer.append("}");
            break;
        }

        default:
            break;
        }

        if (i < fieldsize -1) {
            printer.append(",");
        }
    }
}

void ProtobufJsonFormatter::printFieldValue(const google::protobuf::Message& message,
    const google::protobuf::FieldDescriptor* field,
    JsonPrinter& printer) {
    const google::protobuf::Reflection* reflection = message.GetReflection();

    switch (field->cpp_type()) {
    case google::protobuf::FieldDescriptor::CPPTYPE_INT32:
        printer.append(reflection->GetInt32(message, field));
        break;

    case google::protobuf::FieldDescriptor::CPPTYPE_INT64:
        printer.append(reflection->GetInt64(message, field));
        break;

    case google::protobuf::FieldDescriptor::CPPTYPE_FLOAT:
        printer.append(reflection->GetFloat(message, field));
        break;

    case google::protobuf::FieldDescriptor::CPPTYPE_DOUBLE:
        printer.append(reflection->GetDouble(message, field));
        break;

    case google::protobuf::FieldDescriptor::CPPTYPE_BOOL:
        printer.append(reflection->GetBool(message, field));
        break;

    case google::protobuf::FieldDescriptor::CPPTYPE_UINT32:
        printer.append(reflection->GetUInt32(message, field));
        break;

    case google::protobuf::FieldDescriptor::CPPTYPE_UINT64:
        printer.append(reflection->GetUInt64(message, field));
        break;

    case google::protobuf::FieldDescriptor::CPPTYPE_STRING: {
        std::string value = reflection->GetString(message, field);

        if (value.empty()) {
            printer.append("null");
        }
        else {
            printer << "\"" << value << "\"";
        }

        break;
    }

    case google::protobuf::FieldDescriptor::CPPTYPE_ENUM: {
        printer << "\""
                << reflection->GetEnum(message, field)->name()
                << "\"";
        break;
    }

    case google::protobuf::FieldDescriptor::CPPTYPE_MESSAGE: {
        printer.append("{");
        printMessage(reflection->GetMessage(message, field), printer);
        printer.append("}");
        break;
    }

    default:
        break;
    }
}

bool ProtobufJsonFormatter::utf8Check(const std::string& str) {
    return true;
}

}
}
}
}