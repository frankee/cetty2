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
#include <boost/scoped_ptr.hpp>
#include <boost/program_options.hpp>
#include <cetty/util/SimpleTrie.h>

namespace cetty {
namespace config {

class ConfigObject;

using namespace cetty::util;

class ConfigCenter {
public:
    typedef SimpleTrie<std::string> CmdlineTrie;

public:
    static ConfigCenter& instance();

public:
    ConfigCenter();
    ~ConfigCenter();

    bool load(int argc, char* argv[]);

    bool load(const char* str);
    bool load(const std::string& str);
    bool loadFromFile(const std::string& file);

    bool configure(ConfigObject* object) const;
    bool configure(const std::string& name, ConfigObject* object) const;

    bool addCmdlineName(const std::string& fieldName,
                        const std::string& cmdline);

    void addOptions(const boost::program_options::options_description& desc);

public:
    static bool configureFromString(const char* str, ConfigObject* object);
    static bool configureFromString(const std::string& str, ConfigObject* object);
    static bool configureFromFile(const std::string& file, ConfigObject* object);

private:
    static YAML::Node parseFromFile(const std::string& file);
    static void mergeNode(const YAML::Node& from, YAML::Node* to);
    static bool getFileContent(const std::string& file, std::string* content);

private:
    static ConfigCenter* center_;

private:
    int argc_;
    char** argv_;

    YAML::Node root_;

    CmdlineTrie cmdlineTrie_;
    boost::program_options::variables_map vm_;
    boost::program_options::options_description description_;
};

inline
bool ConfigCenter::addCmdlineName(const std::string& fieldName,
                                  const std::string& cmdline) {
    cmdlineTrie_.addKey(fieldName, new std::string(cmdline));
    return true;
}

inline
void ConfigCenter::addOptions(const boost::program_options::options_description& desc) {
    description_.add(desc);
}

}
}

#endif //#if !defined(CETTY_CONFIG_CONFIGCENTER_H)

// Local Variables:
// mode: c++
// End:
