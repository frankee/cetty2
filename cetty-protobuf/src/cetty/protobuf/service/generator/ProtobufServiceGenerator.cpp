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
#include <google/protobuf/compiler/command_line_interface.h>
#include <google/protobuf/compiler/plugin.h>
#include <google/protobuf/io/printer.h>

#include <cetty/protobuf/service/generator/cpp_service.h>

#include <stdio.h>
#include <fstream>
#include <list>
#include <vector>
#include <boost/filesystem.hpp>

namespace google {
namespace protobuf {

string SimpleItoa(int i);

namespace compiler {
namespace cpp {

string ClassName(const Descriptor* descriptor, bool qualified);

ServiceGenerator::ServiceGenerator(const ServiceDescriptor* descriptor,
                                   const string& dllexport_decl)
    : descriptor_(descriptor) {
    vars_["classname"] = descriptor_->name();
    vars_["full_name"] = descriptor_->full_name();

    if (dllexport_decl.empty()) {
        vars_["dllexport"] = "";
    }
    else {
        vars_["dllexport"] = dllexport_decl + " ";
    }
}

ServiceGenerator::~ServiceGenerator() {}

void ServiceGenerator::GenerateDeclarations(io::Printer* printer) {
    fprintf(stderr, "GenerateDeclarations\n");
    // Forward-declare the stub type.
    printer->Print(vars_,
                   "class $classname$_Stub;\n"
                   "\n");

    std::vector<std::pair<std::string, std::string> > input_types;
    std::vector<std::pair<std::string, std::string> > output_types;
    std::vector<std::string> method_names;
    int j = descriptor_->method_count();

    for (int i = 0; i < j; ++i) {
        const MethodDescriptor* method = descriptor_->method(i);
        input_types.push_back(std::make_pair(ClassName(method->input_type(), true),
                                             ClassName(method->input_type(), false)));
        output_types.push_back(std::make_pair(ClassName(method->output_type(), true),
                                              ClassName(method->output_type(), false)));
        method_names.push_back(method->name());
    }

    for (int i = 0; i < j; ++i) {
        map<string, string> sub_vars;
        sub_vars["type"] = input_types[i].first;
        sub_vars["typedef"] = input_types[i].second;
        sub_vars["out_type"] = output_types[i].first;
        sub_vars["out_typedef"] = output_types[i].second;
        sub_vars["method"] = method_names[i];
        printer->Print(sub_vars,
                       "typedef $type$* $typedef$Ptr;\n"
                       "typedef $type$ const* Const$typedef$Ptr;\n"
                       "typedef $out_type$* $out_typedef$Ptr;\n"
                       "typedef ::cetty::service::ServiceFuture<$out_typedef$Ptr> $method$ServiceFuture;\n"
                       "typedef boost::intrusive_ptr<$method$ServiceFuture> $method$ServiceFuturePtr;\n"
                      );
    }

    printer->Print("\n");

    GenerateInterface(printer);
    GenerateStubDefinition(printer);
}

void ServiceGenerator::GenerateInterface(io::Printer* printer) {
    printer->Print(vars_,
                   "class $dllexport$$classname$ : public ::cetty::protobuf::service::ProtobufService {\n"
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
        "                const ::cetty::protobuf::service::ConstMessagePtr& request,\n"
        "                const ::cetty::protobuf::service::MessagePtr& response,\n"
        "                const DoneCallback& done);\n\n"
        "const ::google::protobuf::Message* GetRequestPrototype(\n"
        "  const ::google::protobuf::MethodDescriptor* method) const;\n"
        "const ::google::protobuf::Message* GetResponsePrototype(\n"
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
                   "$classname$_Stub(const ::cetty::service::ClientServicePtr& service);\n"
                   "~$classname$_Stub();\n"
                   "\n"
                   "inline const ::cetty::service::ClientServicePtr& channel() {\n"
                   "    return channel_.getService();\n"
                   "}\n"
                   "\n"
                   "// implements $classname$ ------------------------------------------\n"
                   "\n");

    GenerateMethodSignatures(STUB, printer);

    printer->Outdent();
    printer->Print(vars_,
                   " private:\n"
                   "  cetty::protobuf::service::ProtobufClientServiceAdaptor channel_;\n"
                   "  bool owns_channel_;\n"
                   "  GOOGLE_DISALLOW_EVIL_CONSTRUCTORS($classname$_Stub);\n"
                   "};\n"
                   "\n");
}

void ServiceGenerator::GenerateMethodSignatures(
    StubOrNon stub_or_non, io::Printer* printer) {
    for (int i = 0; i < descriptor_->method_count(); i++) {
        const MethodDescriptor* method = descriptor_->method(i);
        map<string, string> sub_vars;
        sub_vars["classname"] = descriptor_->name();
        sub_vars["name"] = method->name();
        sub_vars["input_type"] = ClassName(method->input_type(), false);
        sub_vars["output_type"] = ClassName(method->output_type(), false);
        sub_vars["virtual"] = "virtual ";

        if (stub_or_non == NON_STUB) {
            printer->Print(sub_vars,
                           "$virtual$void $name$(const Const$input_type$Ptr& request,\n"
                           "                     const $output_type$Ptr& response,\n"
                           "                     const DoneCallback& done);\n");
        }
        else {
            printer->Print(sub_vars,
                           "using $classname$::$name$;\n"

                           "$virtual$void $name$(const Const$input_type$Ptr& request,\n"
                           "                     const $name$ServiceFuturePtr& future);\n");
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
                   "$classname$_Stub::$classname$_Stub(const cetty::service::ClientServicePtr& service)\n"
                   "  : channel_(service), owns_channel_(false) {\n"
                   "    static int init = 0;\n"
                   "    if (!init) {\n"
                   "        ::cetty::protobuf::service::ProtobufServiceRegister& serviceRegister =\n"
                   "            ::cetty::protobuf::service::ProtobufServiceRegister::instance();\n"
                   "\n");

    // create response register
    for (int i = 0; i < descriptor_->method_count(); i++) {
        const MethodDescriptor* method = descriptor_->method(i);
        map<string, string> sub_vars;
        sub_vars["classname"] = descriptor_->full_name();
        sub_vars["name"] = method->name();
        sub_vars["response"] = method->output_type()->name();

        printer->Print(sub_vars,

                       "       serviceRegister.registerResponsePrototype(\"$classname$\",\n"
                       "                                                 \"$name$\",\n"
                       "                                                 &$response$::default_instance());\n"
                       "\n");
    }

    printer->Print(vars_,
                   "        init = 1;\n"
                   "    }\n"
                   "}\n"
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
        sub_vars["input_type"] = ClassName(method->input_type(), false);
        sub_vars["output_type"] = ClassName(method->output_type(), false);

        printer->Print(sub_vars,
                       "void $classname$::$name$(const Const$input_type$Ptr& request,\n"
                       "                         const $output_type$Ptr& response,\n"
                       "                         const DoneCallback& done) {\n"
                       // "  controller->SetFailed(\"Method $name$() not implemented.\");\n"
                       "  assert(0);\n"
                       //"  done($output_type$Ptr());\n"
                       "}\n"
                       "\n");
    }
}

void ServiceGenerator::GenerateCallMethod(io::Printer* printer) {
    printer->Print(vars_,
                   "void $classname$::CallMethod(const ::google::protobuf::MethodDescriptor* method,\n"
                   "                             const ::cetty::protobuf::service::ConstMessagePtr& request,\n"
                   "                             const ::cetty::protobuf::service::MessagePtr& response,\n"
                   "                             const DoneCallback& done) {\n"
                   "  GOOGLE_DCHECK_EQ(method->service(), $classname$_descriptor_);\n"
                   "  switch(method->index()) {\n");

    for (int i = 0; i < descriptor_->method_count(); i++) {
        const MethodDescriptor* method = descriptor_->method(i);
        map<string, string> sub_vars;
        sub_vars["name"] = method->name();
        sub_vars["index"] = SimpleItoa(i);
        sub_vars["input_type"] = ClassName(method->input_type(), false);
        sub_vars["output_type"] = ClassName(method->output_type(), false);

        // Note:  down_cast does not work here because it only works on pointers,
        //   not references.
        printer->Print(sub_vars,
                       "    case $index$:\n"
                       "      $name$(static_cast< Const$input_type$Ptr>(request),\n"
                       "             static_cast< $output_type$Ptr>(response),\n"
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
                       "const ::google::protobuf::Message* $classname$::GetRequestPrototype(\n");
    }
    else {
        printer->Print(vars_,
                       "const ::google::protobuf::Message* $classname$::GetResponsePrototype(\n");
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
                       "      return &$type$::default_instance();\n");
    }

    printer->Print(vars_,
                   "    default:\n"
                   "      GOOGLE_LOG(FATAL) << \"Bad method index; this should never happen.\";\n"
                   "      return NULL;\n"
                   //"      return *reinterpret_cast< ::google::protobuf::Message*>(NULL);\n"
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
        sub_vars["input_type"] = ClassName(method->input_type(), false);
        sub_vars["output_type"] = ClassName(method->output_type(), false);

        printer->Print(sub_vars,
                       "void $classname$_Stub::$name$(const Const$input_type$Ptr& request,\n"
                       "                              const $name$ServiceFuturePtr& future) {\n"
                       "  channel_.CallMethod<Const$input_type$Ptr, $output_type$Ptr>(descriptor()->method($index$),\n"
                       "                                                              request,\n"
                       "                                                              future);\n"
                       "}\n");
    }
}


}  // namespace cpp
}  // namespace compiler
}  // namespace protobuf
}  // namespace google

namespace gpb = google::protobuf;
namespace gpbc = google::protobuf::compiler;

void findProtoFiles(int argc,
                    char* argv[],
                    std::vector<std::string>* files) {
    if (NULL == files || argc < 2) { return; }

    for (int i = 1; i < argc; ++i) {
        std::string str(argv[i]);

        if (str.rfind(".proto") == str.size() - strlen(".proto")) { //end with ".proto"
            files->push_back(str);
        }
    }
}

void findCmdParam(int argc, char* argv[], const char* cmd, std::string* param) {
    if (NULL == param || NULL == cmd || argc < 2) { return; }

    for (int i = 1; i < argc; ++i) {
        std::string str(argv[i]);
        std::string::size_type pos = str.find(cmd);

        if (pos != str.npos) {
            *param = str.substr(strlen(cmd));
        }
    }
}

void changeHeader(const std::string& proto) {
    std::string hfile(proto);
    hfile.replace(hfile.end() - 6, hfile.end(), ".pb.h");

    std::vector<std::string> lines;
    std::fstream file;
    file.open(hfile.c_str(), std::fstream::in);

    while (!file.eof()) {
        std::string line;
        std::getline(file, line);

        if (line.find("google/protobuf/service.h") != line.npos) {
            lines.push_back("#include <cetty/protobuf/service/ProtobufService.h>");
            lines.push_back("#include <cetty/protobuf/service/ProtobufServiceFuture.h>");
            lines.push_back("#include <cetty/protobuf/service/ProtobufServiceMessagePtr.h>");
            lines.push_back("#include <cetty/protobuf/service/ProtobufClientServiceAdaptor.h>");
        }
        else {
            lines.push_back(line);
        }
    }

    file.close();

    std::fstream wfile(hfile.c_str(), std::fstream::out);

    for (std::size_t i = 0; i < lines.size(); ++i) {
        wfile << lines[i] << "\n";
    }

    wfile << "\n";
    wfile << "// Local Variables:\n";
    wfile << "// mode: c++\n";
    wfile << "// End:\n";
    wfile << "\n";

    wfile.close();
}

std::string changeSource(const std::string& proto) {
    std::string hfile(proto);
    hfile.replace(hfile.end() - 6, hfile.end(), ".pb.cc");

    std::vector<std::string> lines;
    std::fstream file;
    file.open(hfile.c_str(), std::fstream::in);

    while (!file.eof()) {
        std::string line;
        std::getline(file, line);

        if (line.find("google/protobuf/wire_format.h") != line.npos) {
            lines.push_back("#include <google/protobuf/wire_format.h>");
            lines.push_back("#include <cetty/protobuf/service/ProtobufServiceRegister.h>");
        }
        else {
            lines.push_back(line);
        }
    }

    file.close();

    std::fstream wfile(hfile.c_str(), std::fstream::out);

    for (std::size_t i = 0; i < lines.size(); ++i) {
        wfile << lines[i] << "\n";
    }

    wfile << "\n";
    wfile.close();

    return hfile;
}

void changeGeneratedFiles(int argc, char* argv[], bool moveSource) {
    std::vector<std::string> files;

    std::string path;
    std::string sourcePath;

    findProtoFiles(argc, argv, &files);
    findCmdParam(argc, argv, "--cpp_out=", &path);

    if (moveSource) {
        findCmdParam(argc, argv, "--src_out=", &sourcePath);
    }

    if (path[path.size() - 1] == '\\' || path[path.size() - 1] == '/') {
        path[path.size() - 1] = '/';
    }
    else {
        path.append("/");
    }

    if (sourcePath.empty()) {
        sourcePath = "./";
    }
    else {
        char& last = sourcePath[sourcePath.size() - 1];
        if (last == '\\' || last == '/') {
            last = '/';
        }
        else {
            sourcePath.append("/");
        }
    }

    int j = files.size();

    for (int i = 0; i < j; ++i) {
        changeHeader(path + files[i]);
        std::string source = changeSource(path + files[i]);

        if (moveSource) {
            std::string hfile(files[i]);
            hfile.replace(hfile.end() - 6, hfile.end(), ".pb.cc");

            std::string::size_type pos = hfile.find_last_of('/');
            if (pos == hfile.npos) {
                pos = hfile.find_last_of('\\');
            }
            if (pos != hfile.npos) {
                std::string hfilePath = hfile.substr(0, pos+1);
                hfilePath = sourcePath + hfilePath;

                boost::filesystem::path p(hfilePath);
                while(!boost::filesystem::exists(p)) {
                    boost::filesystem::path parent = p;
                    while(!boost::filesystem::exists(parent.parent_path())) {
                        parent = parent.parent_path();
                    }
                    printf("path(%s) does not exist, create it\n", parent.c_str());
                    boost::filesystem::create_directory(parent);
                }
            }
            else {
                printf("file(%s) does not contain any path\n", hfile.c_str());
            }

            hfile = sourcePath + hfile;
            boost::filesystem::rename(boost::filesystem::path(source),
                boost::filesystem::path(hfile));
        }
    }
}

bool findCmd(int argc, char* argv[], const char* shortCmd, const char* cmd) {
    for (int i = 0; i < argc; ++i) {
        if ((shortCmd && strstr(argv[i], shortCmd))
                || (cmd && strstr(argv[i], cmd))) {
            return true;
        }
    }

    return false;
}

int main(int argc, char* argv[]) {
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    google::protobuf::compiler::CommandLineInterface cli;
    cli.AllowPlugins("protoc-");

    // Proto2 C++
    gpbc::cpp::CppGenerator generator;
    cli.RegisterGenerator("--cpp_out", &generator,
                          "Generate C++ header and source.");

    if (findCmd(argc, argv, "-h", "--help")) {
        cli.Run(argc, argv);
        printf("  --src_out=SOURCE_DIR            move the *.pb.cc to SOURCE_DIR\n");
        return 0;
    }

    if (findCmd(argc, argv, NULL, "--src_out")) {
        int newArgc = argc - 1;
        const char* newArgv[128];
        for (int i = 0, j = 0; i < argc; ++i) {
            if (strstr(argv[i], "--src_out")) {
                continue;
            }

            newArgv[j] = strdup(argv[i]);
            ++j;
        }

        if (!cli.Run(newArgc, newArgv)) {
            changeGeneratedFiles(argc, argv, true);
        }
    }
    else {
        if (!cli.Run(argc, argv)) {
            changeGeneratedFiles(argc, argv, false);
        }
    }
}
