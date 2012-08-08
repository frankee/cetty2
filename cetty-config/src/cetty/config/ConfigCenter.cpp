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

#include <boost/program_options.hpp>
#include <yaml-cpp/yaml.h>

#include <cetty/config/ConfigObject.h>
#include <cetty/logging/LoggerHelper.h>

namespace cetty {
namespace config {

using namespace boost::program_options;

ConfigCenter* ConfigCenter::center = NULL;

ConfigCenter& ConfigCenter::instance() {
    if (NULL == center) {
        center = new ConfigCenter;
    }

    return *center;
}

ConfigCenter::ConfigCenter() : argc(0), argv(NULL) {

}

int ConfigCenter::load(int argc, char* argv[]) {
    this->argc = argc;
    this->argv = argv;

    options_description desc("Allowed options");
    desc.add_options()
    ("help", "produce this help message")
    ("conf", "the main configure file");

    variables_map vm;
    store(parse_command_line(argc, argv, desc), vm);
    notify(vm);

    if (vm.count("help")) {
        std::cout << desc << "\n";
        return -1;
    }

    const variable_value& option = vm["conf"];

    if (option.empty()) {
        return -1;
    }

    return loadFromFile(option.as<std::string>());
}

int ConfigCenter::load(const char* str) {
    if (!str) {
        return -1;
    }

    root = YAML::Load(str);

    if (!root) {
        return -2;
    }

    return 0;
}

int ConfigCenter::load(const std::string& str) {
    return load(str.c_str());
}

int ConfigCenter::loadFromFile(const std::string& file) {
    return 0;
}

int ConfigCenter::configure(ConfigObject* object) const {
    if (!object) {
        return -1;
    }

    return configure(object->getName(), object);
}

int parseConfigObject(const YAML::Node& node, ConfigObject* object);

int ConfigCenter::configure(const std::string& name,
                            ConfigObject* object) const {
    if (!object) {
        return -1;
    }

    YAML::Node node = root[name];

    if (node) {
        return parseConfigObject(node, object);
    }

    return -1;
}

}
}
