
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

#include <cetty/config/generator/yaml/YamlGenerator.h>

#include <google/protobuf/io/printer.h>
#include <google/protobuf/io/zero_copy_stream.h>
#include <google/protobuf/descriptor.pb.h>

namespace cetty {
namespace config {
namespace generator {
namespace yaml {

using namespace google::protobuf;
using namespace google::protobuf::compiler;

bool YamlGenerator::Generate(const FileDescriptor* file,
                            const string& parameter,
                            GeneratorContext* generatorContext,
                            string* error) const {

    std::vector<std::pair<std::string, std::string> > options;
    ParseGeneratorParameter(parameter, &options);

    // -----------------------------------------------------------------
    // parse generator options

    std::string basename = StripProto(file->name());

    // Generate header.
    {
        scoped_ptr<io::ZeroCopyOutputStream> output(
            generatorContext->Open(basename + ".conf"));
        io::Printer printer(output.get(), '$');

        //fileGenerator.GenerateHeader(&printer);
    }

    return true;
}

}
}
}
}
