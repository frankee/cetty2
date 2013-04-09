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

#include <cetty/config/ConfigCenter.h>

#include <fstream>
#include <yaml-cpp/yaml.h>
#include <boost/filesystem.hpp>

#include <cetty/logging/LoggerHelper.h>

#include <cetty/config/ConfigObject.h>
#include <cetty/config/ConfigDescriptor.h>
#include <cetty/config/ConfigFileImporter.h>

namespace cetty {
namespace config {

using namespace boost::program_options;

ConfigCenter* ConfigCenter::center_ = NULL;

ConfigCenter& ConfigCenter::instance() {
    if (NULL == center_) {
        center_ = new ConfigCenter;
    }

    return *center_;
}

ConfigCenter::ConfigCenter()
    : argc_(0),
      argv_(NULL),
      description_("Allowed options") {

    description_.add_options()
    ("help", "produce this help message")
    ("conf", value<std::string>(), "the main configure file");
}

ConfigCenter::~ConfigCenter() {
    cmdlineTrie_.freeData();
}

bool ConfigCenter::load(int argc, char* argv[]) {
    argc_ = argc;
    argv_ = argv;

    store(parse_command_line(argc, argv, description_), vm_);
    notify(vm_);

    if (vm_.count("help")) {
        std::cout << description_ << "\n";
        return false;
    }

    const variable_value& option = vm_["conf"];

    if (option.empty()) {
        std::string program = argv[0];
        std::string::size_type slashPos = program.find_last_of('/');

        if (slashPos == program.npos) {
            slashPos = program.find_last_of('\\');
        }

        if (slashPos != program.npos) {
            program = program.substr(slashPos + 1);
        }

        std::string::size_type dotPos = program.find_last_of('.');

        if (dotPos != program.npos) {
            program = program.substr(0, dotPos);
        }

        program += ".conf";

        std::vector<std::string> candidateFiles;
        candidateFiles.push_back(program);

        // /usr/local/bin  /usr/local/etc(or conf)
        // /opt/your_folder/bin  /opt/your_folder/etc(or conf)
        candidateFiles.push_back(std::string("../etc/") + program);
        candidateFiles.push_back(std::string("../conf/") + program);
        candidateFiles.push_back(std::string("/usr/local/etc/") + program);

        for (std::size_t i = 0; i < candidateFiles.size(); ++i) {
            if (boost::filesystem::exists(
                        boost::filesystem::path(candidateFiles[i]))) {
                LOG_INFO << "not specify the conf parameter,"
                         " auto load the configure file: "
                         << candidateFiles[i];
                return loadFromFile(candidateFiles[i]);
            }
        }

        std::cout << description_ << "\n";
        return false;
    }

    return loadFromFile(option.as<std::string>());
}

bool ConfigCenter::load(const char* str) {
    if (!str) {
        return false;
    }

    try {
        root_ = YAML::Load(str);
    }
    catch (const std::exception& e) {
        LOG_ERROR << "parse the yaml configure file error:" << e.what();
        return false;
    }

    if (!root_) {
        return false;
    }

    return true;
}

bool ConfigCenter::load(const std::string& str) {
    return load(str.c_str());
}

bool ConfigCenter::loadFromFile(const std::string& file) {
    root_ = parseFromFile(file);

    if (!root_ || root_.IsNull()) {
        return false;
    }

    return true;
}

YAML::Node ConfigCenter::parseFromFile(const std::string& file) {
    std::vector<std::string> files;
    ConfigFileImporter importer;

    importer.find(file, &files);

    YAML::Node root;
    std::string content;

    for (std::size_t i = 0; i < files.size(); ++i) {
        content.clear();

        if (getFileContent(files[i], &content)) {
            try {
                YAML::Node node = YAML::Load(content);
                mergeNode(node, &root);
            }
            catch (const std::exception& e) {
                LOG_ERROR << "parse the yaml configure file error: "
                          << e.what();
                return YAML::Node();
            }
        }
    }

    return root;
}

bool ConfigCenter::getFileContent(const std::string& file, std::string* content) {
    std::fstream filestream;
    filestream.open(file.c_str(), std::fstream::in);

    if (!filestream.is_open()) {
        filestream.close();
        LOG_ERROR << "can not open the configure file: " << file;
        return false;
    }

    std::string line;

    while (!filestream.eof()) {
        std::getline(filestream, line);
        content->append(line);
        content->append("\n");
    }

    filestream.close();

    return true;
}

void ConfigCenter::mergeNode(const YAML::Node& from, YAML::Node* to) {
    BOOST_ASSERT(to && from.IsMap());

    if (!to->IsDefined()) {
        *to = from;
        return;
    }

    YAML::Node::const_iterator itr = from.begin();

    for (; itr != from.end(); ++itr) {
        YAML::Node node = (*to)[itr->first.Scalar()];

        if (!node.IsDefined()) {
            node = itr->second;
            continue;
        }

        YAML::Node mergeFrom = itr->second;
        YAML::Node::const_iterator j = mergeFrom.begin();

        if (node.IsMap() && mergeFrom.IsMap()) {
            for (; j != mergeFrom.end(); ++j) {
                node[j->first.Scalar()] = j->second;
            }
        }
        else if (node.IsSequence() && mergeFrom.IsSequence()) {
            for (; j != mergeFrom.end(); ++j) {
                node.push_back(j->second);
            }
        }
        else {
            LOG_ERROR << "node can not merge.";
        }
    }
}

bool ConfigCenter::configure(ConfigObject* object) const {
    if (!object) {
        return false;
    }

    return configure(object->descriptor()->className(), object);
}

bool parseConfigObject(const YAML::Node& node, ConfigObject* object);
bool parseConfigObject(const variables_map& vm,
                       const ConfigCenter::CmdlineTrie& cmdline,
                       ConfigObject* object);

bool ConfigCenter::configure(const std::string& name,
                             ConfigObject* object) const {
    if (!object) {
        return false;
    }

    YAML::Node node = root_[name];
    bool result = false;

    if (node && !node.IsNull()) {
        result = parseConfigObject(node, object);
    }

    if (cmdlineTrie_.countPrefix(name) > 0) {
        result = result || parseConfigObject(vm_, cmdlineTrie_, object);
    }

    return result;
}

bool ConfigCenter::configureFromString(const char* str, ConfigObject* object) {
    if (!str || !object) {
        return false;
    }

    YAML::Node root;

    try {
        root = YAML::Load(str);
    }
    catch (const std::exception& e) {
        LOG_ERROR << "parse the yaml configure file error: " << e.what();
    }

    if (!root) {
        return false;
    }

    return parseConfigObject(root, object);
}

bool ConfigCenter::configureFromString(const std::string& str,
                                       ConfigObject* object) {
    return configureFromString(str.c_str(), object);
}

bool ConfigCenter::configureFromFile(const std::string& file,
                                     ConfigObject* object) {
    if (file.empty() || !object) {
        return false;
    }

    YAML::Node root;

    try {
        root = parseFromFile(file);
    }
    catch (const std::exception& e) {
        LOG_ERROR << "parse the yaml configure file error: " << e.what();
    }

    if (!root || root.IsNull()) {
        return false;
    }

    return parseConfigObject(root, object);
}

}
}
