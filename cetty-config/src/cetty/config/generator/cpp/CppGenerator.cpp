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

#include <cetty/config/generator/cpp/CppGenerator.h>

#include <google/protobuf/io/printer.h>
#include <google/protobuf/io/zero_copy_stream.h>
#include <google/protobuf/descriptor.pb.h>

#include <cetty/config/config_options.pb.h>
#include <cetty/config/generator/cpp/FileGenerator.h>
#include <cetty/config/generator/cpp/CppHelpers.h>
#include <cetty/protobuf/service/ProtobufUtil.h>

namespace cetty {
namespace config {
namespace generator {
namespace cpp {

using namespace google::protobuf;
using namespace google::protobuf::compiler;
using namespace cetty::protobuf::service;

bool CppGenerator::Generate(const FileDescriptor* file,
                            const string& parameter,
                            GeneratorContext* generatorContext,
                            string* error) const {

    std::vector<std::pair<std::string, std::string> > options;
    ParseGeneratorParameter(parameter, &options);

    // -----------------------------------------------------------------
    // parse generator options

    // TODO(kenton):  If we ever have more options, we may want to create a
    //   class that encapsulates them which we can pass down to all the
    //   generator classes.  Currently we pass dllexport_decl down to all of
    //   them via the constructors, but we don't want to have to add another
    //   constructor parameter for every option.

    // If the dllexport_decl option is passed to the compiler, we need to write
    // it in front of every symbol that should be exported if this .proto is
    // compiled into a Windows DLL.  E.g., if the user invokes the protocol
    // compiler as:
    //   protoc --cpp_out=dllexport_decl=FOO_EXPORT:outdir foo.proto
    // then we'll define classes like this:
    //   class FOO_EXPORT Foo {
    //     ...
    //   }
    // FOO_EXPORT is a macro which should expand to __declspec(dllexport) or
    // __declspec(dllimport) depending on what is being compiled.
    std::string dllexport_decl;

    for (std::size_t i = 0; i < options.size(); i++) {
        if (options[i].first == "dllexport_decl") {
            dllexport_decl = options[i].second;
        }
        else {
            *error = "Unknown generator option: " + options[i].first;
            return false;
        }
    }
    
    std::string basename = StripProto(file->name());
    basename.append(".cnf");

    boost::optional<std::string> fileName =
        ProtobufUtil::getOption<std::string>(file, "config_file_options", "source_file_name");

    //bool hasExtension = file->options().HasExtension(config_file_options);
    //const ConfigFileOptions& op = file->options().GetExtension(config_file_options);

    FileGenerator fileGenerator(file, dllexport_decl);

    // Generate header.
    {
        scoped_ptr<io::ZeroCopyOutputStream> output(
            generatorContext->Open(basename + ".h"));
        io::Printer printer(output.get(), '$');
        fileGenerator.GenerateHeader(&printer);
    }

    // Generate cc file.
    {
        scoped_ptr<io::ZeroCopyOutputStream> output(
            generatorContext->Open(basename + ".cpp"));
        io::Printer printer(output.get(), '$');
        fileGenerator.GenerateSource(&printer);
    }

    return true;
}

}
}
}
}
