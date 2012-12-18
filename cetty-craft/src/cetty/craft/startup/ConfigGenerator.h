#if !defined(CETTY_CRAFT_STARTUP_CONFIGGENERATOR_H)
#define CETTY_CRAFT_STARTUP_CONFIGGENERATOR_H

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

#include <google/protobuf/descriptor.h>
#include <google/protobuf/descriptor.pb.h>
#include <google/protobuf/io/printer.h>
#include <google/protobuf/compiler/code_generator.h>

namespace cetty {
namespace craft {
namespace startup {

using namespace google::protobuf;
using namespace google::protobuf::compiler;

class CraftMappingConfigGenerator : public CodeGenerator {
public:
    CraftMappingConfigGenerator();
    virtual ~CraftMappingConfigGenerator();

    std::string findStringOption(const Message& options,
        const std::string& name,
        const std::string subName) {

            std::string fullName(name);
            fullName += ".";
            fullName += subName;

            ProtobufUtil::FieldValue field = ProtobufUtil::getMessageFieldValue(options, fullName);
            return boost::get<std::string>(field);
    }

    void getRequestParams(const Descriptor* request) {
        int fieldCnt = request->field_count();
        for (int i = 0; i < fieldCnt; ++i) {
            const FieldDescriptor* field = request->field(i);
            const FieldOptions& options = field->options();
            if (field->type() == FieldDescriptor::TYPE_MESSAGE) {

            }
            else {
                std::string param = findStringOption(options,
                    "craft",
                    "path_param");

                if (param.empty()) {
                    param = findStringOption(options,
                        "craft",
                        "query_param");
                }
                
                if (param.empty()) {
                    param = findStringOption(options,
                        "craft",
                        "head_param");
                }

                if (param.empty()) {
                    param = findStringOption(options,
                        "craft",
                        "cookie_param");
                }
            }
        }
    }

    // implements CodeGenerator ----------------------------------------
    bool Generate(const FileDescriptor* file,
                  const string& parameter,
                  GeneratorContext* generator_context,
                  string* error) const {

                      int serviceCnt = file->service_count();
                      for (int i = 0; i < serviceCnt; ++i) {
                          const ServiceDescriptor* service = file->service(i);

                          std::string servicePath = findStringOption(
                              service->options(),
                              "craft",
                              "path");

                          int methodCnt = service->method_count();
                          for (int j = 0; j < methodCnt; ++j) {
                              const MethodDescriptor* method = service->method(j);

                              const MethodOptions& options = method->options();
                              std::string httpMethod = findStringOption(options, "craft", "method");
                              std::string httpPath = findStringOption(options, "craft", "path");

                              const Descriptor* request = method->input_type();
                              const Descriptor* response = method->output_type();

                              request->field_count();
                          }                         
                      }
                      
                      // Generate header.
                      {
                          scoped_ptr<io::ZeroCopyOutputStream> output(
                              generator_context->Open(basename + "ServiceMapping.conf"));

                          io::Printer printer(output.get(), '$');
                          file_generator.GenerateHeader(&printer);
                      }



                      return true;
    }
    }

private:
    GOOGLE_DISALLOW_EVIL_CONSTRUCTORS(CraftMappingConfigGenerator);
};

}
}
}

#endif //#if !defined(CETTY_CRAFT_STARTUP_CONFIGGENERATOR_H)

// Local Variables:
// mode: c++
// End:
