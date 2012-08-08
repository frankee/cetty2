#if !defined(CETTY_CONFIG_CONFIGCENTER_H)
#define CETTY_CONFIG_CONFIGCENTER_H

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

#include <string>
#include <yaml-cpp/node/node.h>

namespace cetty {
    namespace logging {
        class InternalLogger;
    }
}

namespace cetty {
namespace config {

class ConfigObject;

class ConfigCenter {
public:
    static ConfigCenter& instance();

public:
    ConfigCenter();

    int load(int argc, char* argv[]);

    int load(const char* str);
    int load(const std::string& str);
    int loadFromFile(const std::string& file);

    int configure(ConfigObject* object) const;
    int configure(const std::string& name, ConfigObject* object) const;
	
private:
	int getFileContent(const std::string& file, std::string* content);

private:
    static ConfigCenter* center;
    static cetty::logging::InternalLogger* logger;

private:
    int argc;
    char** argv;

    YAML::Node root;
};

}
}

#endif //#if !defined(CETTY_CONFIG_CONFIGCENTER_H)

// Local Variables:
// mode: c++
// End:
