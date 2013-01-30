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

#include <boost/assert.hpp>

#include <cetty/logging/LoggerHelper.h>
#include <cetty/config/ConfigReflectionImpl.h>
#include <cetty/config/ConfigDescriptor.h>

namespace cetty {
namespace config {

ConfigObject::ObjectDescriptors* ConfigObject::objectDescriptors_ = NULL;

ConfigObject::ConfigObject(const std::string& name)
    : name_(name),
      className_(name) {
}

ConfigObject::~ConfigObject() {
    clear();
}

const ConfigObjectDescriptor* ConfigObject::descriptor() const {
    ConfigObject::ObjectDescriptors& objects = objectDescriptors();

    ObjectDescriptors::const_iterator itr = objects.find(className_);

    if (itr != objects.end()) {
        return itr->second;
    }
    else {
        LOG_WARN << "";
        return NULL;
    }
}

void ConfigObject::clear() {
    std::vector<const ConfigFieldDescriptor*> fields;
    list(&fields);

    for (std::size_t i = 0; i < fields.size(); ++i) {
        const ConfigFieldDescriptor* field = fields[i];

        if (field->repeated) {
            switch (field->type) {
            case ConfigFieldDescriptor::CPPTYPE_BOOL:
                clearRepeatedField<bool>(field);
                break;

            case ConfigFieldDescriptor::CPPTYPE_INT32:
                clearRepeatedField<int>(field);
                break;

            case ConfigFieldDescriptor::CPPTYPE_INT64:
                clearRepeatedField<int64_t>(field);
                break;

            case ConfigFieldDescriptor::CPPTYPE_DOUBLE:
                clearRepeatedField<double>(field);
                break;

            case ConfigFieldDescriptor::CPPTYPE_STRING:
                clearRepeatedField<std::string>(field);
                break;

            case ConfigFieldDescriptor::CPPTYPE_OBJECT:
                clearRepeatedField<ConfigObject*>(field);
                break;
            }
        }
        else {
            switch (field->type) {
            case ConfigFieldDescriptor::CPPTYPE_BOOL:
                clearField<boost::optional<bool> >(field);
                break;

            case ConfigFieldDescriptor::CPPTYPE_INT32:
                clearField<boost::optional<int> >(field);
                break;

            case ConfigFieldDescriptor::CPPTYPE_INT64:
                clearField<boost::optional<int64_t> >(field);
                break;

            case ConfigFieldDescriptor::CPPTYPE_DOUBLE:
                clearField<boost::optional<double> >(field);
                break;

            case ConfigFieldDescriptor::CPPTYPE_STRING:
                clearField<std::string>(field);
                break;

            case ConfigFieldDescriptor::CPPTYPE_OBJECT:
                clearField<ConfigObject*>(field);
                break;
            }
        }
    }
}

void ConfigObject::copyFrom(const ConfigObject& from) {
    std::vector<const ConfigFieldDescriptor*> fields;
    list(&fields);

    for (std::size_t i = 0; i < fields.size(); ++i) {
        const ConfigFieldDescriptor* field = fields[i];

        if (field->repeated) {
            switch (field->type) {
            case ConfigFieldDescriptor::CPPTYPE_BOOL:
                copyRepeatedField<bool>(from, field);
                break;

            case ConfigFieldDescriptor::CPPTYPE_INT32:
                copyRepeatedField<int>(from, field);
                break;

            case ConfigFieldDescriptor::CPPTYPE_INT64:
                copyRepeatedField<int64_t>(from, field);
                break;

            case ConfigFieldDescriptor::CPPTYPE_DOUBLE:
                copyRepeatedField<double>(from, field);
                break;

            case ConfigFieldDescriptor::CPPTYPE_STRING:
                copyRepeatedField<std::string>(from, field);
                break;

            case ConfigFieldDescriptor::CPPTYPE_OBJECT:
                copyRepeatedField<ConfigObject*>(from, field);
                break;
            }
        }
        else {
            switch (field->type) {
            case ConfigFieldDescriptor::CPPTYPE_BOOL:
                copyField<boost::optional<bool> >(from, field);
                break;

            case ConfigFieldDescriptor::CPPTYPE_INT32:
                copyField<boost::optional<int> >(from, field);
                break;

            case ConfigFieldDescriptor::CPPTYPE_INT64:
                copyField<boost::optional<int64_t> >(from, field);
                break;

            case ConfigFieldDescriptor::CPPTYPE_DOUBLE:
                copyField<boost::optional<double> >(from, field);
                break;

            case ConfigFieldDescriptor::CPPTYPE_STRING:
                copyField<std::string>(from, field);
                break;

            case ConfigFieldDescriptor::CPPTYPE_OBJECT:
                copyField<ConfigObject*>(from, field);
                break;
            }
        }
    }
}

ConfigObject::ObjectDescriptors& ConfigObject::objectDescriptors() {
    if (!objectDescriptors_) {
        objectDescriptors_ = new ObjectDescriptors;
    }

    return *objectDescriptors_;
}

void ConfigObject::addDescriptor(ConfigObjectDescriptor* descriptor) {
    BOOST_ASSERT(descriptor);

    ConfigObject::ObjectDescriptors& objects = objectDescriptors();
    const std::string& name = descriptor->className();
    ObjectDescriptors::const_iterator itr = objects.find(name);

    if (itr != objects.end()) {

    }
    else {
        objects[name] = descriptor;
    }
}

const ConfigObject* ConfigObject::defaultObject(const std::string& name) {
    if (name.empty()) {
        return NULL;
    }

    ConfigObject::ObjectDescriptors& objects = objectDescriptors();
    ObjectDescriptors::const_iterator itr = objects.find(name);

    if (itr != objects.end()) {
        return itr->second->defaultInstance();
    }

    return NULL;
}

void ConfigObject::list(std::vector<const ConfigFieldDescriptor*>* fields) const {

}

bool ConfigObject::has(const ConfigFieldDescriptor* field) const {
    if (field->optional) {
        
    }

    return false;
}

int ConfigObject::getInt(const ConfigFieldDescriptor* field) const {
    if (field->optional) {
        const IntOption& value = *constRaw<IntOption>(field);
        if (value) {
            return boost::get<int>(value);
        }

        return 0;
    }
    else {
        return get<int>(field);
    }
}

int64_t ConfigObject::getInt64(const ConfigFieldDescriptor* field) const {
    if (field->optional) {
        const Int64Option& value = *constRaw<Int64Option>(field);
        if (value) {
            return boost::get<int64_t>(value);
        }

        return 0;
    }
    else {
        return get<int64_t>(field);
    }
}

double ConfigObject::getDouble(const ConfigFieldDescriptor* field) const {
    if (field->optional) {
        const DoubleOption& value = *constRaw<DoubleOption>(field);
        if (value) {
            return boost::get<double>(value);
        }

        return 0;
    }
    else {
        return get<double>(field);
    }
}

bool ConfigObject::getBool(const ConfigFieldDescriptor* field) const {
    if (field->optional) {
        const BoolOption& value = *constRaw<BoolOption>(field);
        if (value) {
            return boost::get<bool>(value);
        }

        return 0;
    }
    else {
        return get<bool>(field);
    }
}

ConfigObject* ConfigObject::mutableObject(const ConfigFieldDescriptor* field) {
        ConfigObject** obj = mutableRaw<ConfigObject*>(field);
        if (!*obj) {
            const ConfigObject* prototype = defaultObject(field->className);

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

ConfigObject* ConfigObject::addObject(const ConfigFieldDescriptor* field) {
        std::vector<ConfigObject*>* repeated =
            mutableRaw<std::vector<ConfigObject*> >(object, field);

        // We must allocate a new object.
        const ConfigObject* prototype =
            ConfigObject::defaultObject(field->className);

        if (NULL == prototype) {
            LOG_ERROR << "can not find such object "
                << field->className;

            return NULL;
        }

        ConfigObject* newObject = prototype->create();
        repeated->push_back(newObject);

        return newObject;
}

ConfigObject* ::addObject(const ConfigFieldDescriptor* field) {

}

ConfigObject* ConfigObject::addObject(const ConfigFieldDescriptor* field,
    const std::string& key) {

}

}
}
