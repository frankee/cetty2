#if !defined(CETTY_CONFIG_CONFIGREFLECTIONIMPL_H)
#define CETTY_CONFIG_CONFIGREFLECTIONIMPL_H

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

#include <cetty/config/ConfigObject.h>
#include <cetty/config/ConfigReflection.h>

namespace cetty {
namespace config {

template<> inline
void ConfigReflection::copyField<std::string>(const ConfigObject& from,
        const ConfigFieldDescriptor* field,
        ConfigObject* to) const {
    const std::string& value = getField<std::string>(from, field);

    if (!value.empty()) {
        setField<std::string>(to, field, value);
    }
}

template<> inline
void ConfigReflection::copyField<ConfigObject*>(const ConfigObject& from,
        const ConfigFieldDescriptor* field,
        ConfigObject* to) const {
    ConfigObject* value = getField<ConfigObject*>(from, field);

    if (!value) {
        ConfigObject** toValue = mutableRaw<ConfigObject*>(to, field);

        if (toValue) {
            (*toValue)->copyFrom(*value);
        }
    }
}

template<> inline
void ConfigReflection::copyRepeatedField<ConfigObject*>(const ConfigObject& from,
        const ConfigFieldDescriptor* field,
        ConfigObject* to) const {
    std::vector<ConfigObject*> values =
        getField<std::vector<ConfigObject*> >(from, field);

    for (std::size_t i = 0; i < values.size(); ++i) {
        ConfigObject* toValue = addObject(to, field);
        toValue->copyFrom(*values[i]);
    }
}

template<> inline
void ConfigReflection::clearRepeatedField<ConfigObject*>(ConfigObject* object,
        const ConfigFieldDescriptor* field) const {

    std::vector<ConfigObject*>* raw = mutableRaw<std::vector<ConfigObject*> >(object, field);

    if (raw) {
        for (std::size_t i = 0; i < raw->size(); ++i) {
            delete raw->at(i);
        }

        raw->clear();
    }
}

template<> inline
void ConfigReflection::clearField<std::string>(ConfigObject* object,
        const ConfigFieldDescriptor* field) const {
    std::string* str = mutableRaw<std::string>(object, field);

    if (str) {
        str->clear();
    }
}

template<> inline
void ConfigReflection::clearField<ConfigObject*>(ConfigObject* object,
        const ConfigFieldDescriptor* field) const {
    ConfigObject** obj = mutableRaw<ConfigObject*>(object, field);

    if (obj && *obj) {
        (*obj)->clear();
    }
}

}
}


#endif //#if !defined(CETTY_CONFIG_CONFIGREFLECTIONIMPL_H)

// Local Variables:
// mode: c++
// End:
