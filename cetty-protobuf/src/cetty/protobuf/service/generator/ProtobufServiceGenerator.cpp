#include <stdio.h>
#include <fstream>
#include <list>
#include <vector>
#include <boost/filesystem.hpp>

#include <google/protobuf/compiler/cpp/cpp_generator.h>
#include <google/protobuf/compiler/command_line_interface.h>
#include <google/protobuf/compiler/plugin.h>
#include <cetty/protobuf/service/generator/ServiceGenerator.h>

namespace gpb = google::protobuf;
namespace gpbc = google::protobuf::compiler;
namespace gpbcc = google::protobuf::compiler::cpp;

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

void changeGeneratedFiles(int argc, char* argv[], bool moveSource, bool hasService) {
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
        std::string source;

        if (hasService) {
            changeHeader(path + files[i]);
            source = changeSource(path + files[i]);
        }
        else {
            source = path + files[i];
            source.replace(source.end() - 6, source.end(), ".pb.cc");
        }
        
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
            changeGeneratedFiles(argc, argv, true, gpbcc::ServiceGenerator::protoHasService);
        }
    }
    else {
        if (!cli.Run(argc, argv)) {
            changeGeneratedFiles(argc, argv, false, gpbcc::ServiceGenerator::protoHasService);
        }
    }
}
