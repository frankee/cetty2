#if !defined(CETTY_CONFIG_CONFIGOBJECT_H)
#define CETTY_CONFIG_CONFIGOBJECT_H

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

#include <map>

namespace cetty {
namespace config {

class ConfigReflection;
class ConfigDescriptor;

class ConfigObject {
public:
    ConfigObject() {}
    virtual ~ConfigObject() {}

    const ConfigReflection& getreflection() const { }
    const ConfigDescriptor& getdescriptor() const { }

    virtual ConfigObject* create() const = 0;

protected:
    void addDescriptor(const std::string& name, ConfigDescriptor* desciptor, ConfigObject* object) {

    }

private:
    static std::map<std::string, std::pair<ConfigDescriptor*, ConfigObject*> > objects;
};

}
}

#endif //#if !defined(CETTY_CONFIG_CONFIGOBJECT_H)

// Local Variables:
// mode: c++
// End:
