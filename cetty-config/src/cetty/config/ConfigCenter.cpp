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

namespace cetty {
namespace config {

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
    return 0;
}

int ConfigCenter::load(const char* str) {
    return 0;
}

int ConfigCenter::load(const std::string& str) {
    return load(str.c_str());
}

int ConfigCenter::loadFromFile(const std::string& file) {
    return 0;
}

int ConfigCenter::configure(ConfigObject* object) const {
    return 0;
}

}
}
