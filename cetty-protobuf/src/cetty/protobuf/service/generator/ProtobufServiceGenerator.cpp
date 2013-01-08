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

                headerName_ = baseName + ".pb.h";
                sourceName_ = baseName + ".pb.cc";
            }

            return true;
        }

        return false;
    }

    void postProcess() {
        if (!headerName_.empty()) {
            printf("service proto file, changing the %s and %s.\n", headerName_.c_str(), sourceName_.c_str());

            changeHeader(headerName_);
            changeSource(sourceName_);
        }
    }

private:
    void changeHeader(const std::string& headerFile) const {
        std::vector<std::string> lines;
        std::fstream file;
        file.open(headerFile.c_str(), std::fstream::in);
        if (!file.is_open()) {
            printf("can not find the the header : %s\n", headerFile.c_str());
            return;
        }

        while (!file.eof()) {
            std::string line;
            std::getline(file, line);

            if (line.find("google/protobuf/service.h") != line.npos) {
                printf("add cetty protobuf dependency in header file.\n");
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

        if (!wfile.is_open()) {
            printf("can't open the file to write.\n");
        }

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
        if (!file.is_open()) {
            printf("can not find the the source : %s\n", sourceFile.c_str());
            return;
        }

        while (!file.eof()) {
            std::string line;
            std::getline(file, line);

            if (line.find("google/protobuf/wire_format.h") != line.npos) {
                printf("add cetty protobuf dependency in source file.\n");
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

    mutable std::string headerName_;
    mutable std::string sourceName_;
};

int main(int argc, char* argv[]) {
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    ProtobufServiceGenerator generator;

    google::protobuf::compiler::CommandLineInterface cli;
    cli.RegisterGenerator("--service_out", &generator,
                          "Generate C++ header and source.");

    int result = cli.Run(argc, argv);
    if (!result) {
        generator.postProcess();
    }

    return result;

    //PluginMain(argc, argv, &generator);
}
