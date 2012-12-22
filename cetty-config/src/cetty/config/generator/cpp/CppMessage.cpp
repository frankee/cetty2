// Protocol Buffers - Google's data interchange format
// Copyright 2008 Google Inc.  All rights reserved.
// http://code.google.com/p/protobuf/
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//     * Neither the name of Google Inc. nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

// Author: kenton@google.com (Kenton Varda)
//  Based on original Protocol Buffers design by
//  Sanjay Ghemawat, Jeff Dean, and others.
#include <cetty/config/generator/cpp/CppMessage.h>

#include <algorithm>
#include <map>
#include <utility>
#include <vector>

#include <google/protobuf/io/printer.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/wire_format.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/descriptor.pb.h>

#include <cetty/config/generator/cpp/CppHelpers.h>

namespace cetty {
namespace config {
namespace generator {
namespace cpp {

std::string simpleI2A(int i) {
    char buf[64] = {0};
    sprintf(buf, "%d", i);
    return std::string(buf);
}

namespace {

void PrintFieldComment(io::Printer* printer, const FieldDescriptor* field) {
    // Print the field's proto-syntax definition as a comment.  We don't want to
    // print group bodies so we cut off after the first line.
    string def = field->DebugString();
    printer->Print("// $def$\n",
                   "def", def.substr(0, def.find_first_of('\n')));
}

struct FieldOrderingByNumber {
    inline bool operator()(const FieldDescriptor* a,
                           const FieldDescriptor* b) const {
        return a->number() < b->number();
    }
};

const char* kWireTypeNames[] = {
    "VARINT",
    "FIXED64",
    "LENGTH_DELIMITED",
    "START_GROUP",
    "END_GROUP",
    "FIXED32",
};

// Sort the fields of the given Descriptor by number into a new[]'d array
// and return it.
const FieldDescriptor** SortFieldsByNumber(const Descriptor* descriptor) {
    const FieldDescriptor** fields =
        new const FieldDescriptor*[descriptor->field_count()];

    for (int i = 0; i < descriptor->field_count(); i++) {
        fields[i] = descriptor->field(i);
    }

    sort(fields, fields + descriptor->field_count(),
         FieldOrderingByNumber());
    return fields;
}

// Functor for sorting extension ranges by their "start" field number.
struct ExtensionRangeSorter {
    bool operator()(const Descriptor::ExtensionRange* left,
                    const Descriptor::ExtensionRange* right) const {
        return left->start < right->start;
    }
};

}

// ===================================================================

MessageGenerator::MessageGenerator(const Descriptor* descriptor,
                                   const string& dllexport_decl)
    : descriptor_(descriptor),
      classname_(ClassName(descriptor, false)),
      dllexport_decl_(dllexport_decl),
      nested_generators_(new scoped_ptr<MessageGenerator>[
                             descriptor->nested_type_count()]) {

    for (int i = 0; i < descriptor->nested_type_count(); i++) {
        nested_generators_[i].reset(
            new MessageGenerator(descriptor->nested_type(i), dllexport_decl));
    }
}

MessageGenerator::~MessageGenerator() {}

void MessageGenerator::
GenerateForwardDeclaration(io::Printer* printer) {
    printer->Print("class $classname$;\n",
                   "classname", classname_);
}

bool MessageGenerator::isNestType(const Descriptor* descriptor, const FieldDescriptor* fieldDescriptor) {
    if (descriptor->nested_type_count() > 0) {
        for (int i = 0; i < descriptor->nested_type_count(); i++) {
            if (descriptor->nested_type(i)->full_name() ==  fieldDescriptor->message_type()->full_name()) {
                return true;
            }
        }
    }

    return false;
}

void MessageGenerator::
GenerateClassDefinition(io::Printer* printer) {
    for (int i = 0; i < descriptor_->nested_type_count(); i++) {
        nested_generators_[i]->GenerateClassDefinition(printer);
        printer->Print("\n");
        printer->Print(kThinSeparator);
        printer->Print("\n");
    }

    map<string, string> vars;
    vars["classname"] = classname_;
    vars["field_count"] = simpleI2A(descriptor_->field_count());

    if (dllexport_decl_.empty()) {
        vars["dllexport"] = "";
    }
    else {
        vars["dllexport"] = dllexport_decl_ + " ";
    }

    vars["superclass"] = "::cetty::config::ConfigObject";

    printer->Print(vars,
                   "class $dllexport$$classname$ : public $superclass$ {\n"
                   "public:\n");
    Indent(printer);

    if (descriptor_->nested_type_count() > 0) {
        // Import all nested message classes into this class's scope with typedefs.
        for (int i = 0; i < descriptor_->nested_type_count(); i++) {
            const Descriptor* nested_type = descriptor_->nested_type(i);
            printer->Print("typedef $nested_full_name$ $nested_name$;\n",
                           "nested_name", nested_type->name(),
                           "nested_full_name", ClassName(nested_type, false));
        }

        printer->Print("\n");
        Outdent(printer);
        printer->Print("public:\n");
        Indent(printer);
    }

    // Field members:

    vector<const FieldDescriptor*> fields;

    for (int i = 0; i < descriptor_->field_count(); i++) {
        const FieldDescriptor* field = descriptor_->field(i);

        if (field->is_repeated()) {
            switch (field->cpp_type()) {
            case FieldDescriptor::CPPTYPE_INT32:
                printer->Print("std::vector<int> $field_name$;\n", "field_name", field->camelcase_name());
                break;

            case FieldDescriptor::CPPTYPE_INT64:
                printer->Print("std::vector<int64_t> $field_name$;\n", "field_name", field->camelcase_name());
                break;

            case FieldDescriptor::CPPTYPE_BOOL:
                printer->Print("std::vector<bool> $field_name$;\n", "field_name", field->camelcase_name());
                break;

            case FieldDescriptor::CPPTYPE_DOUBLE:
                printer->Print("std::vector<double> $field_name$;\n", "field_name", field->camelcase_name());
                break;

            case FieldDescriptor::CPPTYPE_STRING:
                printer->Print("std::vector<std::string> $field_name$;\n", "field_name", field->camelcase_name());
                break;

            case FieldDescriptor::CPPTYPE_MESSAGE:
                if (isNestType(descriptor_, field)) {
                    printer->Print("std::vector<$class_name$*> $field_name$;\n",
                                   "class_name", field->message_type()->name(),
                                   "field_name", field->camelcase_name());
                }
                else {
                    printer->Print("std::vector<$class_name$*> $field_name$;\n",
                                   "class_name", ClassName(field->message_type(), true),
                                   "field_name", field->camelcase_name());
                }

                break;

            default:
                break;
            }
        }
        else {
            switch (field->cpp_type()) {
            case FieldDescriptor::CPPTYPE_INT32:
                printer->Print("boost::optional<int> $field_name$;\n", "field_name", field->camelcase_name());
                break;

            case FieldDescriptor::CPPTYPE_INT64:
                printer->Print("boost::optional<int64_t> $field_name$;\n", "field_name", field->camelcase_name());
                break;

            case FieldDescriptor::CPPTYPE_BOOL:
                printer->Print("boost::optional<bool> $field_name$;\n", "field_name", field->camelcase_name());
                break;

            case FieldDescriptor::CPPTYPE_DOUBLE:
                printer->Print("boost::optional<double> $field_name$;\n", "field_name", field->camelcase_name());
                break;

            case FieldDescriptor::CPPTYPE_STRING:
                printer->Print("std::string $field_name$;\n", "field_name", field->camelcase_name());
                break;

            case FieldDescriptor::CPPTYPE_MESSAGE:
                if (isNestType(descriptor_, field)) {
                    printer->Print("$class_name$* $field_name$;\n",
                                   "class_name", field->message_type()->name(),
                                   "field_name", field->camelcase_name());
                }
                else {
                    printer->Print("$class_name$* $field_name$;\n",
                                   "class_name", ClassName(field->message_type(), true),
                                   "field_name", field->camelcase_name());
                }

                break;

            default:
                break;
            }
        }
    }

    printer->Print("\n");
    Outdent(printer);
    printer->Print("public:\n");
    Indent(printer);

    printer->Print(vars,
                   "$classname$();\n"
                   "virtual ~$classname$() {}\n"
                   "\n"
                   "$classname$(const $classname$& from);\n"
                   "\n"
                   "$classname$& operator=(const $classname$& from) {\n"
                   "    clear();\n"
                   "    copyFrom(from);\n"
                   "    return *this;\n"
                   "}\n"
                   "\n");

    printer->Print(vars,
                   "virtual $classname$* create() const {\n"
                   "    return new $classname$;\n"
                   "}\n");

    printer->Print("\n");

    Outdent(printer);
    printer->Print(vars, "};");
}

void MessageGenerator::
GenerateTypeRegistrations(io::Printer* printer) {
    char fieldCnt[128] = {0};
    sprintf(fieldCnt, "%d", descriptor_->field_count());
    // Register this message type with the message factory.
    printer->Print(
        "CETTY_CONFIG_ADD_DESCRIPTOR($classname$,\n"
        "                            $count$,\n",
        "classname", classname_,
        "count", simpleI2A(descriptor_->field_count()));

    for (int i = 0; i < descriptor_->field_count(); i++) {
        const FieldDescriptor* field = descriptor_->field(i);
        if (field->is_repeated()) {
            switch (field->cpp_type()) {
            case FieldDescriptor::CPPTYPE_INT32:
                printer->Print(
                    "                            CETTY_CONFIG_REPEATED_FIELD($classname$, $field_name$, INT32)",
                    "classname", classname_,
                    "field_name", field->camelcase_name());
                break;

            case FieldDescriptor::CPPTYPE_INT64:
                printer->Print(
                    "                            CETTY_CONFIG_REPEATED_FIELD($classname$, $field_name$, INT64)",
                    "classname", classname_,
                    "field_name", field->camelcase_name());
                break;

            case FieldDescriptor::CPPTYPE_BOOL:
                printer->Print(
                    "                            CETTY_CONFIG_REPEATED_FIELD($classname$, $field_name$, BOOL)",
                    "classname", classname_,
                    "field_name", field->camelcase_name());
                break;

            case FieldDescriptor::CPPTYPE_DOUBLE:
                printer->Print(
                    "                            CETTY_CONFIG_REPEATED_FIELD($classname$, $field_name$, DOUBLE)",
                    "classname", classname_,
                    "field_name", field->camelcase_name());
                break;

            case FieldDescriptor::CPPTYPE_STRING:
                printer->Print(
                    "                            CETTY_CONFIG_REPEATED_FIELD($classname$, $field_name$, STRING)",
                    "classname", classname_,
                    "field_name", field->camelcase_name());
                break;

            case FieldDescriptor::CPPTYPE_MESSAGE:
                printer->Print(
                    "                            CETTY_CONFIG_REPEATED_OBJECT_FIELD($classname$, $field_name$, $field_class$)",
                    "classname", classname_,
                    "field_name", field->camelcase_name(),
                    "field_class", field->message_type()->full_name());
                break;
            }
        }
        else {
            switch (field->cpp_type()) {
            case FieldDescriptor::CPPTYPE_INT32:
                printer->Print(
                    "                            CETTY_CONFIG_FIELD($classname$, $field_name$, INT32)",
                    "classname", classname_,
                    "field_name", field->camelcase_name());
                break;

            case FieldDescriptor::CPPTYPE_INT64:
                printer->Print(
                    "                            CETTY_CONFIG_FIELD($classname$, $field_name$, INT64)",
                    "classname", classname_,
                    "field_name", field->camelcase_name());
                break;

            case FieldDescriptor::CPPTYPE_BOOL:
                printer->Print(
                    "                            CETTY_CONFIG_FIELD($classname$, $field_name$, BOOL)",
                    "classname", classname_,
                    "field_name", field->camelcase_name());
                break;

            case FieldDescriptor::CPPTYPE_DOUBLE:
                printer->Print(
                    "                            CETTY_CONFIG_FIELD($classname$, $field_name$, DOUBLE)",
                    "classname", classname_,
                    "field_name", field->camelcase_name());
                break;

            case FieldDescriptor::CPPTYPE_STRING:
                printer->Print(
                    "                            CETTY_CONFIG_FIELD($classname$, $field_name$, STRING)",
                    "classname", classname_,
                    "field_name", field->camelcase_name());
                break;

            case FieldDescriptor::CPPTYPE_MESSAGE:
                printer->Print(
                    "                            CETTY_CONFIG_OBJECT_FIELD($classname$, $field_name$, $field_class$)",
                    "classname", classname_,
                    "field_name", field->camelcase_name(),
                    "field_class", field->message_type()->full_name());
                break;
            }
        }

        if (i == descriptor_->field_count() - 1) {
            printer->PrintRaw(");\n\n");
        }
        else {
            printer->PrintRaw(",\n");
        }
    }

    // Handle nested types.
    for (int i = 0; i < descriptor_->nested_type_count(); i++) {
        nested_generators_[i]->GenerateTypeRegistrations(printer);
    }
}

void MessageGenerator::
GenerateStructors(io::Printer* printer) {
    // Generate the default constructor.
    printer->Print(
        "$classname$::$classname$()\n"
        "  : ::cetty::config::ConfigObject(\"$class_full_name$\") {\n"
        "}\n",
        "classname", classname_,
        "class_full_name", descriptor_->full_name());

#if 0
    // The default instance needs all of its embedded message pointers
    // cross-linked to other default instances.  We can't do this initialization
    // in the constructor because some other default instances may not have been
    // constructed yet at that time.
    // TODO(kenton):  Maybe all message fields (even for non-default messages)
    //   should be initialized to point at default instances rather than NULL?
    for (int i = 0; i < descriptor_->field_count(); i++) {
        const FieldDescriptor* field = descriptor_->field(i);

        if (!field->is_repeated() &&
                field->cpp_type() == FieldDescriptor::CPPTYPE_MESSAGE) {
            printer->Print(
                "  $name$_ = const_cast< $type$*>(&$type$::default_instance());\n",
                "name", FieldName(field),
                "type", FieldMessageTypeName(field));
        }
    }

    printer->Print(
        "}\n"
        "\n");

#endif

    // Generate the copy constructor.
    printer->Print(
        "$classname$::$classname$(const $classname$& from)\n"
        "  : ::cetty::config::ConfigObject(\"$class_full_name$\") {\n"
        "    copyFrom(from);\n"
        "}\n"
        "\n",
        "classname", classname_,
        "class_full_name", descriptor_->full_name());

    // Handle nested types.
    for (int i = 0; i < descriptor_->nested_type_count(); i++) {
        nested_generators_[i]->GenerateStructors(printer);
    }
}

void MessageGenerator::Indent(io::Printer* printer) {
    printer->Indent();
    printer->Indent();
}

void MessageGenerator::Outdent(io::Printer* printer) {
    printer->Outdent();
    printer->Outdent();
}

void MessageGenerator::GenerateCommandLines(io::Printer* printer) {
#if 0
    class KeyValuePairCmdline {
    public:
        KeyValuePairCmdline() {
            options_description options("ServerBuiderConfig");
            op.add_options()
                ("deamon, d", value<bool>, "xxxxx");

            ConfigCenter::instance().addOptions(options);
            ConfigCenter::instance().addCmdlineName("", "");
        }
    };

    static KeyValuePairCmdline keyValuePairCmdline;
#endif
}

}
}
}
}
