#include <stdio.h>
#include <fstream>
#include <list>
#include <vector>
#include <string>
#include <boost/filesystem.hpp>

#include <google/protobuf/descriptor.h>
#include <google/protobuf/descriptor.pb.h>
#include <google/protobuf/compiler/cpp/cpp_generator.h>
#include <google/protobuf/compiler/command_line_interface.h>
#include <google/protobuf/compiler/plugin.h>

#include <cetty/protobuf/service/generator/ServiceGenerator.h>

using namespace google::protobuf;
using namespace google::protobuf::compiler;
using namespace google::protobuf::compiler::cpp;

class ProtobufServiceGenerator : public CodeGenerator {
public:
    ProtobufServiceGenerator() {}
    ~ProtobufServiceGenerator() {}

    bool Generate(const FileDescriptor* file,
                  const string& parameter,
                  GeneratorContext* generator_context,
                  string* error) const {
        if (generator_.Generate(file, parameter, generator_context, error)) {
            if (file->options().cc_generic_services()) {
                const std::string& fileName = file->name();
                std::string::size_type pos = fileName.find_last_of('.');

                std::string baseName = fileName.substr(0, pos);
                std::string headerName = baseName + ".pb.h";
                std::string sourceName = baseName + ".pb.cpp";
                changeHeader(headerName);
                changeSource(sourceName);
            }

            return true;
        }

        return false;
    }

private:
    void changeHeader(const std::string& headerFile) const {
        std::vector<std::string> lines;
        std::fstream file;
        file.open(headerFile.c_str(), std::fstream::in);

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

        std::fstream wfile(headerFile.c_str(), std::fstream::out);

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

    void changeSource(const std::string& sourceFile) const {
        std::vector<std::string> lines;
        std::fstream file;
        file.open(sourceFile.c_str(), std::fstream::in);

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

        std::fstream wfile(sourceFile.c_str(), std::fstream::out);

        for (std::size_t i = 0; i < lines.size(); ++i) {
            wfile << lines[i] << "\n";
        }

        wfile << "\n";
        wfile.close();
    }

private:
    CppGenerator generator_;
};

int main(int argc, char* argv[]) {
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    ProtobufServiceGenerator generator;

#if defined(GENERATOR_NOT_PLUGIN)
    google::protobuf::compiler::CommandLineInterface cli;
    cli.RegisterGenerator("--cpp_out", &generator,
                          "Generate C++ header and source.");

    return cli.Run(argc, argv);
#else
    PluginMain(argc, argv, &generator);
#endif
}
