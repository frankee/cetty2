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
#include <boost/program_options.hpp>
#include <yaml-cpp/yaml.h>

#include <cetty/logging/LoggerHelper.h>
#include <cetty/logging/InternalLoggerFactory.h>

#include <cetty/config/ConfigObject.h>
#include <cetty/config/ConfigIncludeFileFinder.h>

namespace cetty {
namespace config {

using namespace boost::program_options;
using namespace cetty::logging;

ConfigCenter* ConfigCenter::center = NULL;
InternalLogger* ConfigCenter::logger = NULL;

ConfigCenter& ConfigCenter::instance() {
    if (NULL == center) {
        center = new ConfigCenter;
    }

    return *center;
}

ConfigCenter::ConfigCenter() : argc(0), argv(NULL), finder() {
    if (!logger) {
        logger = InternalLoggerFactory::getInstance("ConfigCenter");
    }

    finder = new ConfigIncludeFileFinder(logger);
}

ConfigCenter::~ConfigCenter() {
    if (finder) {
        delete finder;
    }
}

bool ConfigCenter::load(int argc, char* argv[]) {
    this->argc = argc;
    this->argv = argv;

    options_description desc("Allowed options");
    desc.add_options()
    ("help", "produce this help message")
    ("conf", value<std::string>(), "the main configure file");

    variables_map vm;
    store(parse_command_line(argc, argv, desc), vm);
    notify(vm);

    if (vm.count("help")) {
        std::cout << desc << "\n";
        return false;
    }

    const variable_value& option = vm["conf"];

    if (option.empty()) {
        return false;
    }

    return loadFromFile(option.as<std::string>());
}

bool ConfigCenter::load(const char* str) {
    if (!str) {
        return false;
    }

    try {
        root = YAML::Load(str);
    }
    catch (const std::exception& e) {
        LOG_ERROR(logger, "parse the yaml configure file error: %s", e.what());
    }

    if (!root) {
        return false;
    }

    return true;
}

bool ConfigCenter::load(const std::string& str) {
    return load(str.c_str());
}

bool ConfigCenter::loadFromFile(const std::string& file) {
    std::vector<std::string> files;
    std::string content;

    if (finder->find(file, &files) > 0) {
        if (getFileContent(files, &content)) {
            return load(content.c_str());
        }
    }

    return true;
}

bool ConfigCenter::configure(ConfigObject* object) const {
    if (!object) {
        return false;
    }

    return configure(object->getName(), object);
}

bool parseConfigObject(const YAML::Node& node, ConfigObject* object);

bool ConfigCenter::configure(const std::string& name,
                            ConfigObject* object) const {
    if (!object) {
        return false;
    }

    YAML::Node node = root[name];

    if (node) {
        return parseConfigObject(node, object);
    }

    return false;
}

bool ConfigCenter::getFileContent(const std::vector<std::string>& files, std::string* content) {
    std::vector<std::string>::const_iterator itr = files.begin();
    for (; itr != files.end(); ++itr) {
        getFileContent(*itr, content);
    }
    return !content->empty();
}

bool ConfigCenter::getFileContent(const std::string& file, std::string* content) {
    std::fstream filestream;
    filestream.open(file, std::fstream::in);

    if (!filestream.is_open()) {
        filestream.close();
        LOG_ERROR(logger, "can not open the configure file: %s", file.c_str());
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

bool ConfigCenter::configureFromString(const char* str, ConfigObject* object) {
    if (!str || !object) {
        return false;
    }

    YAML::Node root;

    try {
        root = YAML::Load(str);
    }
    catch (const std::exception& e) {
        LOG_ERROR(logger, "parse the yaml configure file error: %s", e.what());
    }

    if (!root) {
        return false;
    }

    return parseConfigObject(root, object);
}

bool ConfigCenter::configureFromString(const std::string& str, ConfigObject* object) {
    return configureFromString(str.c_str(), object);
}

bool ConfigCenter::configureFromFile(const std::string& file, ConfigObject* object) {
    if (file.empty() || !object) {
        return false;
    }

    YAML::Node root;

    try {
        root = YAML::LoadFile(file);
    }
    catch (const std::exception& e) {
        LOG_ERROR(logger, "parse the yaml configure file error: %s", e.what());
    }

    if (!root) {
        return false;
    }

    return parseConfigObject(root, object);
}

}
}
