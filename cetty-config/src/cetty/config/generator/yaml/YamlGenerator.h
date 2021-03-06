#if !defined(CETTY_CONFIG_GENERATOR_YAML_YAMLGENERATOR_H)
#define CETTY_CONFIG_GENERATOR_YAML_YAMLGENERATOR_H

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

#include <google/protobuf/compiler/code_generator.h>

namespace cetty {
namespace config {
namespace generator {
namespace yaml {

using namespace google::protobuf;
using namespace google::protobuf::compiler;

class YamlGenerator : public google::protobuf::compiler::CodeGenerator {
public:
    YamlGenerator() {}
    virtual ~YamlGenerator() {}

    virtual bool Generate(const FileDescriptor* file,
                          const string& parameter,
                          GeneratorContext* generatorContext,
                          string* error) const;
};

}
}
}
}


#endif //#if !defined(CETTY_CONFIG_GENERATOR_YAML_YAMLGENERATOR_H)

// Local Variables:
// mode: c++
// End:
