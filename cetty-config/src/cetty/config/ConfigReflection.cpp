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

#include <cetty/config/ConfigReflection.h>
#include <cetty/config/ConfigObject.h>

#include <cetty/logging/LoggerHelper.h>

namespace cetty {
namespace config {

ConfigReflection::ConfigReflection() {
}

void ConfigReflection::listFields(const ConfigObject& object,
                                  std::vector<const ConfigFieldDescriptor*>* output) const {

}

ConfigObject* ConfigReflection::mutableObject(ConfigObject* object,
    const ConfigFieldDescriptor* field) const {
    ConfigObject** obj = mutableRaw<ConfigObject*>(object, field);
    if (!*obj) {
        const ConfigObject* prototype = 
            ConfigObject::getDefaultObject(field->className);

        if (NULL == prototype) {
            LOG_ERROR << "can not find such object "
                << field->className;

            return NULL;
        }

        ConfigObject* newObject = prototype->create();

        *obj = newObject;
    }

    return *obj;
}

ConfigObject* ConfigReflection::addObject(ConfigObject* object,
        const ConfigFieldDescriptor* field) const {
    std::vector<ConfigObject*>* repeated =
        mutableRaw<std::vector<ConfigObject*> >(object, field);

    // We must allocate a new object.
    const ConfigObject* prototype =
        ConfigObject::getDefaultObject(field->className);

    if (NULL == prototype) {
        LOG_ERROR << "can not find such object "
                  << field->className;

        return NULL;
    }

    ConfigObject* newObject = prototype->create();
    repeated->push_back(newObject);

    return newObject;
}

}
}
