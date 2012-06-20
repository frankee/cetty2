// code based on google/protobuf/compiler/cpp/cpp_service.cc
// Integrated with muduo by Shuo Chen.

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

#include <google/protobuf/compiler/cpp/cpp_generator.h>
#include <google/protobuf/compiler/plugin.h>
#include <google/protobuf/io/printer.h>

#include <cetty/protobuf/cpp_service.h>

#include <stdio.h>

namespace google {
namespace protobuf {

string SimpleItoa(int i);

namespace compiler {
namespace cpp {

string ClassName(const Descriptor* descriptor, bool qualified);

void ServiceGenerator::GenerateDeclarations(io::Printer* printer) {
    fprintf(stderr, "GenerateDeclarations\n");
    // Forward-declare the stub type.
    printer->Print(vars_,
                   "class $classname$_Stub;\n"
                   "\n");

    GenerateInterface(printer);
    GenerateStubDefinition(printer);
}

void ServiceGenerator::GenerateInterface(io::Printer* printer) {
    printer->Print(vars_,
                   "class $dllexport$$classname$ : public ::cetty::protobuf::Service {\n"
                   " protected:\n"
                   "  // This class should be treated as an abstract interface.\n"
                   "  inline $classname$() {};\n"
                   " public:\n"
                   "  virtual ~$classname$();\n");
    printer->Indent();

    printer->Print(vars_,
                   "\n"
                   "typedef $classname$_Stub Stub;\n"
                   "\n"
                   "static const ::google::protobuf::ServiceDescriptor* descriptor();\n"
                   "\n");

    GenerateMethodSignatures(NON_STUB, printer);

    printer->Print(
        "\n"
        "// implements Service ----------------------------------------------\n"
        "\n"
        "const ::google::protobuf::ServiceDescriptor* GetDescriptor();\n"
        "void CallMethod(const ::google::protobuf::MethodDescriptor* method,\n"
        "                const ::google::protobuf::Message* request,\n"
        "                const ::google::protobuf::Message* responsePrototype,\n"
        "                const DoneCallback& done);\n"
        "const ::google::protobuf::Message& GetRequestPrototype(\n"
        "  const ::google::protobuf::MethodDescriptor* method) const;\n"
        "const ::google::protobuf::Message& GetResponsePrototype(\n"
        "  const ::google::protobuf::MethodDescriptor* method) const;\n");

    printer->Outdent();
    printer->Print(vars_,
                   "\n"
                   " private:\n"
                   "  GOOGLE_DISALLOW_EVIL_CONSTRUCTORS($classname$);\n"
                   "};\n"
                   "\n");
}

void ServiceGenerator::GenerateStubDefinition(io::Printer* printer) {
    printer->Print(vars_,
                   "class $dllexport$$classname$_Stub : public $classname$ {\n"
                   " public:\n");

    printer->Indent();

    printer->Print(vars_,
                   "$classname$_Stub(const ::cetty::channel::ChannelPtr& channel);\n"
                   "~$classname$_Stub();\n"
                   "\n"
                   "inline const ::cetty::channel::ChannelPtr& channel() { return channel_; }\n"
                   "\n"
                   "// implements $classname$ ------------------------------------------\n"
                   "\n");

    GenerateMethodSignatures(STUB, printer);

    printer->Outdent();
    printer->Print(vars_,
                   " private:\n"
                   "  const ::cetty::channel::RpcChannelPtr channel_;\n"
                   "  bool owns_channel_;\n"
                   "  GOOGLE_DISALLOW_EVIL_CONSTRUCTORS($classname$_Stub);\n"
                   "};\n"
                   "\n");
}

void ServiceGenerator::GenerateFutureType(io::Printer* printer) {
        for (int i = 0; i < descriptor_->method_count(); i++) {
            const MethodDescriptor* method = descriptor_->method(i);
            map<string, string> sub_vars;
            sub_vars["classname"] = descriptor_->name();
            sub_vars["name"] = method->name();
            sub_vars["input_type"] = ClassName(method->input_type(), true);
            sub_vars["output_type"] = ClassName(method->output_type(), true);
            sub_vars["virtual"] = "virtual ";

            printer->Print(sub_vars,
                "typedef ServiceFuture<$output_type$> $output_type$Future\n"
                "typedef boost::intrusive<ServiceFuture<$output_type$> > $output_type$FuturePtr\n");
        }
}

void ServiceGenerator::GenerateMethodSignatures(
    StubOrNon stub_or_non, io::Printer* printer) {
    for (int i = 0; i < descriptor_->method_count(); i++) {
        const MethodDescriptor* method = descriptor_->method(i);
        map<string, string> sub_vars;
        sub_vars["classname"] = descriptor_->name();
        sub_vars["name"] = method->name();
        sub_vars["input_type"] = ClassName(method->input_type(), true);
        sub_vars["output_type"] = ClassName(method->output_type(), true);
        sub_vars["virtual"] = "virtual ";

        if (stub_or_non == NON_STUB) {
            printer->Print(sub_vars,
                           "$virtual$void $name$(const ::boost::shared_ptr<const $input_type$>& request,\n"
                           "                     const $output_type$* responsePrototype,\n"
                           "                     const DoneCallback& done);\n");
        }
        else {
            printer->Print(sub_vars,
                           "using $classname$::$name$;\n"
                           "$virtual$void $name$(const $input_type$& request,\n"
                           "                     const ::boost::function1<void,\n"
                           "                           const ::boost::shared_ptr< $output_type$>&>& done);\n");
        }
    }
}

// ===================================================================

void ServiceGenerator::GenerateDescriptorInitializer(
    io::Printer* printer, int index) {
    map<string, string> vars;
    vars["classname"] = descriptor_->name();
    vars["index"] = SimpleItoa(index);

    printer->Print(vars,
                   "$classname$_descriptor_ = file->service($index$);\n");
}

// ===================================================================

void ServiceGenerator::GenerateImplementation(io::Printer* printer) {
    printer->Print(vars_,
                   "$classname$::~$classname$() {}\n"
                   "\n"
                   "const ::google::protobuf::ServiceDescriptor* $classname$::descriptor() {\n"
                   "  protobuf_AssignDescriptorsOnce();\n"
                   "  return $classname$_descriptor_;\n"
                   "}\n"
                   "\n"
                   "const ::google::protobuf::ServiceDescriptor* $classname$::GetDescriptor() {\n"
                   "  protobuf_AssignDescriptorsOnce();\n"
                   "  return $classname$_descriptor_;\n"
                   "}\n"
                   "\n");

    // Generate methods of the interface.
    GenerateNotImplementedMethods(printer);
    GenerateCallMethod(printer);
    GenerateGetPrototype(REQUEST, printer);
    GenerateGetPrototype(RESPONSE, printer);

    // Generate stub implementation.
    printer->Print(vars_,
                   "$classname$_Stub::$classname$_Stub(::cetty::protobuf::ProtobufRpcChannel* channel__)\n"
                   "  : channel_(channel__), owns_channel_(false) {}\n"
                   "$classname$_Stub::~$classname$_Stub() {\n"
                   "}\n"
                   "\n");

    GenerateStubMethods(printer);
}

void ServiceGenerator::GenerateNotImplementedMethods(io::Printer* printer) {
    for (int i = 0; i < descriptor_->method_count(); i++) {
        const MethodDescriptor* method = descriptor_->method(i);
        map<string, string> sub_vars;
        sub_vars["classname"] = descriptor_->name();
        sub_vars["name"] = method->name();
        sub_vars["index"] = SimpleItoa(i);
        sub_vars["input_type"] = ClassName(method->input_type(), true);
        sub_vars["output_type"] = ClassName(method->output_type(), true);

        printer->Print(sub_vars,
                       "void $classname$::$name$(const ::boost::shared_ptr<const $input_type$>&,\n"
                       "                         const $output_type$*,\n"
                       "                         const DoneCallback& done) {\n"
                       // "  controller->SetFailed(\"Method $name$() not implemented.\");\n"
                       "  assert(0);\n"
                       "  done(NULL);\n"
                       "}\n"
                       "\n");
    }
}

void ServiceGenerator::GenerateCallMethod(io::Printer* printer) {
    printer->Print(vars_,
                   "void $classname$::CallMethod(const ::google::protobuf::MethodDescriptor* method,\n"
                   "                             const ::google::protobuf::MessagePtr& request,\n"
                   "                             const ::google::protobuf::Message* responsePrototype,\n"
                   "                             const DoneCallback& done) {\n"
                   "  GOOGLE_DCHECK_EQ(method->service(), $classname$_descriptor_);\n"
                   "  switch(method->index()) {\n");

    for (int i = 0; i < descriptor_->method_count(); i++) {
        const MethodDescriptor* method = descriptor_->method(i);
        map<string, string> sub_vars;
        sub_vars["name"] = method->name();
        sub_vars["index"] = SimpleItoa(i);
        sub_vars["input_type"] = ClassName(method->input_type(), true);
        sub_vars["output_type"] = ClassName(method->output_type(), true);

        // Note:  down_cast does not work here because it only works on pointers,
        //   not references.
        printer->Print(sub_vars,
                       "    case $index$:\n"
                       "      $name$(::google::protobuf::down_pointer_cast<const $input_type$>(request),\n"
                       "             ::google::protobuf::down_cast<const $output_type$*>(responsePrototype),\n"
                       "             done);\n"
                       "      break;\n");
    }

    printer->Print(vars_,
                   "    default:\n"
                   "      GOOGLE_LOG(FATAL) << \"Bad method index; this should never happen.\";\n"
                   "      break;\n"
                   "  }\n"
                   "}\n"
                   "\n");
}

void ServiceGenerator::GenerateGetPrototype(RequestOrResponse which,
        io::Printer* printer) {
    if (which == REQUEST) {
        printer->Print(vars_,
                       "const ::google::protobuf::Message& $classname$::GetRequestPrototype(\n");
    }
    else {
        printer->Print(vars_,
                       "const ::google::protobuf::Message& $classname$::GetResponsePrototype(\n");
    }

    printer->Print(vars_,
                   "    const ::google::protobuf::MethodDescriptor* method) const {\n"
                   "  GOOGLE_DCHECK_EQ(method->service(), descriptor());\n"
                   "  switch(method->index()) {\n");

    for (int i = 0; i < descriptor_->method_count(); i++) {
        const MethodDescriptor* method = descriptor_->method(i);
        const Descriptor* type =
            (which == REQUEST) ? method->input_type() : method->output_type();

        map<string, string> sub_vars;
        sub_vars["index"] = SimpleItoa(i);
        sub_vars["type"] = ClassName(type, true);

        printer->Print(sub_vars,
                       "    case $index$:\n"
                       "      return $type$::default_instance();\n");
    }

    printer->Print(vars_,
                   "    default:\n"
                   "      GOOGLE_LOG(FATAL) << \"Bad method index; this should never happen.\";\n"
                   "      return *reinterpret_cast< ::google::protobuf::Message*>(NULL);\n"
                   "  }\n"
                   "}\n"
                   "\n");
}

void ServiceGenerator::GenerateStubMethods(io::Printer* printer) {
    for (int i = 0; i < descriptor_->method_count(); i++) {
        const MethodDescriptor* method = descriptor_->method(i);
        map<string, string> sub_vars;
        sub_vars["classname"] = descriptor_->name();
        sub_vars["name"] = method->name();
        sub_vars["index"] = SimpleItoa(i);
        sub_vars["input_type"] = ClassName(method->input_type(), true);
        sub_vars["output_type"] = ClassName(method->output_type(), true);

        printer->Print(sub_vars,
                       "void $classname$_Stub::$name$(const $input_type$& request,\n"
                       "                              const ::boost::function1<void,\n"
                       "                                    const ::boost::shared_ptr< $output_type$>&>& done) {"
                       "  channel_->CallMethod(descriptor()->method($index$),\n"
                       "                       request, &$output_type$::default_instance(), done);\n"
                       "}\n");
    }
}


}  // namespace cpp
}  // namespace compiler
}  // namespace protobuf
}  // namespace google

namespace gpb = google::protobuf;
namespace gpbc = google::protobuf::compiler;

int main(int argc, char* argv[]) {
    GOOGLE_PROTOBUF_VERIFY_VERSION;
    gpbc::cpp::CppGenerator generator;
    return gpbc::PluginMain(argc, argv, &generator);
}
