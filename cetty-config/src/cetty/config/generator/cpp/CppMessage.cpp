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

#include <algorithm>
#include <map>
#include <utility>
#include <vector>
#include <cetty/config/generator/cpp/CppMessage.h>
#include <cetty/config/generator/cpp/CppHelpers.h>
#include <google/protobuf/io/printer.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/wire_format.h>
#include <google/protobuf/descriptor.pb.h>

namespace cetty {
namespace config {
namespace generator {
namespace cpp {

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
      field_generators_(descriptor),
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
    vars["field_count"] = SimpleItoa(descriptor_->field_count());

    if (dllexport_decl_.empty()) {
        vars["dllexport"] = "";
    }
    else {
        vars["dllexport"] = dllexport_decl_ + " ";
    }

    vars["superclass"] = "::cetty::config::ConfigObject";

    printer->Print(vars,
                   "class $dllexport$$classname$ : public $superclass$ {\n"
                   " public:\n");
    printer->Indent();

    printer->Print(vars,
                   "$classname$();\n"
                   "virtual ~$classname$();\n"
                   "\n"
                   "$classname$(const $classname$& from);\n"
                   "\n"
                   "inline $classname$& operator=(const $classname$& from) {\n"
                   "  CopyFrom(from);\n"
                   "  return *this;\n"
                   "}\n"
                   "\n");

    printer->Print(vars,
                   "static const $classname$& default_instance();\n"
                   "\n");


    printer->Print(vars,
                   "void Swap($classname$* other);\n"
                   "\n"
                   "// implements Message ----------------------------------------------\n"
                   "\n"
                   "$classname$* create() const;\n");

    printer->Print(
        "// nested types ----------------------------------------------------\n"
        "\n");

    // Import all nested message classes into this class's scope with typedefs.
    for (int i = 0; i < descriptor_->nested_type_count(); i++) {
        const Descriptor* nested_type = descriptor_->nested_type(i);
        printer->Print("typedef $nested_full_name$ $nested_name$;\n",
                       "nested_name", nested_type->name(),
                       "nested_full_name", ClassName(nested_type, false));
    }

    if (descriptor_->nested_type_count() > 0) {
        printer->Print("\n");
    }

    printer->Print("\n");

    // Field members:

    vector<const FieldDescriptor*> fields;

    for (int i = 0; i < descriptor_->field_count(); i++) {
        fields.push_back(descriptor_->field(i));
    }

    for (int i = 0; i < fields.size(); ++i) {
        field_generators_.get(fields[i]).GeneratePrivateMembers(printer);
    }

    printer->Outdent();
    printer->Print(vars, "};");
}

void MessageGenerator::
GenerateDescriptorInitializer(io::Printer* printer, int index) {
    // TODO(kenton):  Passing the index to this method is redundant; just use
    //   descriptor_->index() instead.
    map<string, string> vars;
    vars["classname"] = classname_;
    vars["index"] = SimpleItoa(index);

    // Obtain the descriptor from the parent's descriptor.
    if (descriptor_->containing_type() == NULL) {
        printer->Print(vars,
                       "$classname$_descriptor_ = file->message_type($index$);\n");
    }
    else {
        vars["parent"] = ClassName(descriptor_->containing_type(), false);
        printer->Print(vars,
                       "$classname$_descriptor_ = "
                       "$parent$_descriptor_->nested_type($index$);\n");
    }


    // Construct the reflection object.
    printer->Print(vars,
                   "$classname$_reflection_ =\n"
                   "  new ::google::protobuf::internal::GeneratedMessageReflection(\n"
                   "    $classname$_descriptor_,\n"
                   "    $classname$::default_instance_,\n"
                   "    $classname$_offsets_,\n"
                   "    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET($classname$, _has_bits_[0]),\n"
                   "    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET("
                   "$classname$, _unknown_fields_),\n");

    if (descriptor_->extension_range_count() > 0) {
        printer->Print(vars,
                       "    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET("
                       "$classname$, _extensions_),\n");
    }
    else {
        // No extensions.
        printer->Print(vars,
                       "    -1,\n");
    }

    printer->Print(vars,
                   "    ::google::protobuf::DescriptorPool::generated_pool(),\n"
                   "    ::google::protobuf::MessageFactory::generated_factory(),\n"
                   "    sizeof($classname$));\n");

    // Handle nested types.
    for (int i = 0; i < descriptor_->nested_type_count(); i++) {
        nested_generators_[i]->GenerateDescriptorInitializer(printer, i);
    }
}

void MessageGenerator::
GenerateTypeRegistrations(io::Printer* printer) {
    // Register this message type with the message factory.
    printer->Print(
        "::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(\n"
        "  $classname$_descriptor_, &$classname$::default_instance());\n",
        "classname", classname_);

    // Handle nested types.
    for (int i = 0; i < descriptor_->nested_type_count(); i++) {
        nested_generators_[i]->GenerateTypeRegistrations(printer);
    }
}

void MessageGenerator::
GenerateDefaultInstanceAllocator(io::Printer* printer) {
    // Construct the default instance.  We can't call InitAsDefaultInstance() yet
    // because we need to make sure all default instances that this one might
    // depend on are constructed first.
    printer->Print(
        "$classname$::default_instance_ = new $classname$();\n",
        "classname", classname_);

    // Handle nested types.
    for (int i = 0; i < descriptor_->nested_type_count(); i++) {
        nested_generators_[i]->GenerateDefaultInstanceAllocator(printer);
    }
}

void MessageGenerator::
GenerateDefaultInstanceInitializer(io::Printer* printer) {
    printer->Print(
        "$classname$::default_instance_->InitAsDefaultInstance();\n",
        "classname", classname_);

    // Register extensions.
    for (int i = 0; i < descriptor_->extension_count(); i++) {
        extension_generators_[i]->GenerateRegistration(printer);
    }

    // Handle nested types.
    for (int i = 0; i < descriptor_->nested_type_count(); i++) {
        nested_generators_[i]->GenerateDefaultInstanceInitializer(printer);
    }
}

void MessageGenerator::
GenerateShutdownCode(io::Printer* printer) {
    printer->Print(
        "delete $classname$::default_instance_;\n",
        "classname", classname_);

    if (HasDescriptorMethods(descriptor_->file())) {
        printer->Print(
            "delete $classname$_reflection_;\n",
            "classname", classname_);
    }

    // Handle nested types.
    for (int i = 0; i < descriptor_->nested_type_count(); i++) {
        nested_generators_[i]->GenerateShutdownCode(printer);
    }
}


void MessageGenerator::
GenerateStructors(io::Printer* printer) {
    string superclass = SuperClassName(descriptor_);

    // Generate the default constructor.
    printer->Print(
        "$classname$::$classname$()\n"
        "  : $superclass$() {\n"
        "  SharedCtor();\n"
        "}\n",
        "classname", classname_,
        "superclass", superclass);

    printer->Print(
        "\n"
        "void $classname$::InitAsDefaultInstance() {\n",
        "classname", classname_);

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

    // Generate the copy constructor.
    printer->Print(
        "$classname$::$classname$(const $classname$& from)\n"
        "  : $superclass$() {\n"
        "  SharedCtor();\n"
        "  MergeFrom(from);\n"
        "}\n"
        "\n",
        "classname", classname_,
        "superclass", superclass);

    // Generate the shared constructor code.
    GenerateSharedConstructorCode(printer);

    // Generate the destructor.
    printer->Print(
        "$classname$::~$classname$() {\n"
        "  SharedDtor();\n"
        "}\n"
        "\n",
        "classname", classname_);

    // Generate the shared destructor code.
    GenerateSharedDestructorCode(printer);

    // Generate SetCachedSize.
    printer->Print(
        "void $classname$::SetCachedSize(int size) const {\n"
        "  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();\n"
        "  _cached_size_ = size;\n"
        "  GOOGLE_SAFE_CONCURRENT_WRITES_END();\n"
        "}\n",
        "classname", classname_);

    // Only generate this member if it's not disabled.
    if (HasDescriptorMethods(descriptor_->file()) &&
            !descriptor_->options().no_standard_descriptor_accessor()) {
        printer->Print(
            "const ::google::protobuf::Descriptor* $classname$::descriptor() {\n"
            "  protobuf_AssignDescriptorsOnce();\n"
            "  return $classname$_descriptor_;\n"
            "}\n"
            "\n",
            "classname", classname_,
            "adddescriptorsname",
            GlobalAddDescriptorsName(descriptor_->file()->name()));
    }

    printer->Print(
        "const $classname$& $classname$::default_instance() {\n"
        "  if (default_instance_ == NULL) $adddescriptorsname$();"
        "  return *default_instance_;\n"
        "}\n"
        "\n"
        "$classname$* $classname$::default_instance_ = NULL;\n"
        "\n"
        "$classname$* $classname$::New() const {\n"
        "  return new $classname$;\n"
        "}\n",
        "classname", classname_,
        "adddescriptorsname",
        GlobalAddDescriptorsName(descriptor_->file()->name()));

}

}
}
}
}