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
#include <cetty/config/ConfigDescriptor.h>

namespace cetty {
namespace config {

ConfigObject::ObjectDescriptors* ConfigObject::objectDescriptors_ = NULL;

ConfigObject::ConfigObject(const std::string& name)
    : name_(name),
      className_(name) {
}

ConfigObject::~ConfigObject() {
}

ConfigObject::ObjectDescriptors& ConfigObject::objectDescriptors() {
    if (!objectDescriptors_) {
        objectDescriptors_ = new ObjectDescriptors;
    }

    return *objectDescriptors_;
}

const ConfigObjectDescriptor* ConfigObject::descriptor() const {
    ObjectDescriptors& descriptors = objectDescriptors();
    ObjectDescriptors::const_iterator itr = descriptors.find(className_);

    if (itr != descriptors.end()) {
        return itr->second;
    }
    else {
        LOG_WARN << "this object has not register to the objectDescriptors.";
        return NULL;
    }
}

void ConfigObject::addDescriptor(ConfigObjectDescriptor* descriptor) {
    BOOST_ASSERT(descriptor && "descriptor should not be NULL");

    const std::string& name = descriptor->className();
    ObjectDescriptors& descriptors = objectDescriptors();
    ObjectDescriptors::iterator itr = descriptors.find(name);

    if (itr != descriptors.end()) {
        LOG_WARN << "the " << name
                 << " descriptor has already registered, will override older.";

        itr->second = descriptor;
    }
    else {
        descriptors[name] = descriptor;
    }
}

const ConfigObject* ConfigObject::defaultObject(const std::string& name) {
    if (name.empty()) {
        return NULL;
    }

    ObjectDescriptors& objects = objectDescriptors();
    ObjectDescriptors::const_iterator itr = objects.find(name);

    if (itr != objects.end()) {
        return itr->second->defaultInstance();
    }

    return NULL;
}

void ConfigObject::clear() {
    std::vector<const ConfigFieldDescriptor*> fields;
    list(&fields);

    for (std::size_t i = 0; i < fields.size(); ++i) {
        const ConfigFieldDescriptor* field = fields[i];

        switch (field->repeatedType) {
        case ConfigFieldDescriptor::NO_REPEATED:
            switch (field->cppType) {
            case ConfigFieldDescriptor::CPPTYPE_BOOL:
                clearField<bool>(field);
                break;

            case ConfigFieldDescriptor::CPPTYPE_INT32:
                clearField<int>(field);
                break;

            case ConfigFieldDescriptor::CPPTYPE_INT64:
                clearField<int64_t>(field);
                break;

            case ConfigFieldDescriptor::CPPTYPE_DOUBLE:
                clearField<double>(field);
                break;

            case ConfigFieldDescriptor::CPPTYPE_STRING:
                clearField<std::string>(field);
                break;

            case ConfigFieldDescriptor::CPPTYPE_OBJECT:
                clearField<ConfigObject*>(field);
                break;
            }

            break;

        case ConfigFieldDescriptor::LIST:
            switch (field->cppType) {
            case ConfigFieldDescriptor::CPPTYPE_BOOL:
                clearRepeatedField<std::vector<bool> >(field);
                break;

            case ConfigFieldDescriptor::CPPTYPE_INT32:
                clearRepeatedField<std::vector<int> >(field);
                break;

            case ConfigFieldDescriptor::CPPTYPE_INT64:
                clearRepeatedField<std::vector<int64_t> >(field);
                break;

            case ConfigFieldDescriptor::CPPTYPE_DOUBLE:
                clearRepeatedField<std::vector<double> >(field);
                break;

            case ConfigFieldDescriptor::CPPTYPE_STRING:
                clearRepeatedField<std::vector<std::string> >(field);
                break;

            case ConfigFieldDescriptor::CPPTYPE_OBJECT:
                clearRepeatedField<std::vector<ConfigObject*> >(field);
                break;
            }

            break;

        case ConfigFieldDescriptor::MAP:
            switch (field->cppType) {
            case ConfigFieldDescriptor::CPPTYPE_BOOL:
                clearRepeatedField<std::map<std::string, bool> >(field);
                break;

            case ConfigFieldDescriptor::CPPTYPE_INT32:
                clearRepeatedField<std::map<std::string, int> >(field);
                break;

            case ConfigFieldDescriptor::CPPTYPE_INT64:
                clearRepeatedField<std::map<std::string, int64_t> >(field);
                break;

            case ConfigFieldDescriptor::CPPTYPE_DOUBLE:
                clearRepeatedField<std::map<std::string, double> >(field);
                break;

            case ConfigFieldDescriptor::CPPTYPE_STRING:
                clearRepeatedField<std::map<std::string, std::string> >(field);
                break;

            case ConfigFieldDescriptor::CPPTYPE_OBJECT:
                clearRepeatedField<std::map<std::string, ConfigObject*> >(field);
                break;
            }

            break;

        default:
            break;
        }
    }
}

void ConfigObject::copyFrom(const ConfigObject& from) {
    std::vector<const ConfigFieldDescriptor*> fields;
    list(&fields);

    for (std::size_t i = 0; i < fields.size(); ++i) {
        const ConfigFieldDescriptor* field = fields[i];

        switch (field->repeatedType) {
        case ConfigFieldDescriptor::NO_REPEATED:
            switch (field->cppType) {
            case ConfigFieldDescriptor::CPPTYPE_BOOL:
                copyField<bool>(field, from);
                break;

            case ConfigFieldDescriptor::CPPTYPE_INT32:
                copyField<int>(field, from);
                break;

            case ConfigFieldDescriptor::CPPTYPE_INT64:
                copyField<int64_t>(field, from);
                break;

            case ConfigFieldDescriptor::CPPTYPE_DOUBLE:
                copyField<double>(field, from);
                break;

            case ConfigFieldDescriptor::CPPTYPE_STRING:
                copyField<std::string>(field, from);
                break;

            case ConfigFieldDescriptor::CPPTYPE_OBJECT:
                copyField<ConfigObject*>(field, from);
                break;
            }

            break;

        case ConfigFieldDescriptor::LIST:
            switch (field->cppType) {
            case ConfigFieldDescriptor::CPPTYPE_BOOL:
                copyRepeatedField<std::vector<bool> >(field, from);
                break;

            case ConfigFieldDescriptor::CPPTYPE_INT32:
                copyRepeatedField<std::vector<int> >(field, from);
                break;

            case ConfigFieldDescriptor::CPPTYPE_INT64:
                copyRepeatedField<std::vector<int64_t> >(field, from);
                break;

            case ConfigFieldDescriptor::CPPTYPE_DOUBLE:
                copyRepeatedField<std::vector<double> >(field, from);
                break;

            case ConfigFieldDescriptor::CPPTYPE_STRING:
                copyRepeatedField<std::vector<std::string> >(field, from);
                break;

            case ConfigFieldDescriptor::CPPTYPE_OBJECT:
                copyRepeatedField<std::vector<ConfigObject*> >(field, from);
                break;
            }

            break;

        case ConfigFieldDescriptor::MAP:
            switch (field->cppType) {
            case ConfigFieldDescriptor::CPPTYPE_BOOL:
                clearRepeatedField<std::map<std::string, bool> >(field);
                break;

            case ConfigFieldDescriptor::CPPTYPE_INT32:
                clearRepeatedField<std::map<std::string, int> >(field);
                break;

            case ConfigFieldDescriptor::CPPTYPE_INT64:
                clearRepeatedField<std::map<std::string, int64_t> >(field);
                break;

            case ConfigFieldDescriptor::CPPTYPE_DOUBLE:
                clearRepeatedField<std::map<std::string, double> >(field);
                break;

            case ConfigFieldDescriptor::CPPTYPE_STRING:
                clearRepeatedField<std::map<std::string, std::string> >(field);
                break;

            case ConfigFieldDescriptor::CPPTYPE_OBJECT:
                clearRepeatedField<std::map<std::string, ConfigObject*> >(field);
                break;
            }

            break;

        default:
            break;
        }
    }
}

void ConfigObject::list(std::vector<const ConfigFieldDescriptor*>* fields) const {
    BOOST_ASSERT(fields && "fields should not be NULL when list field.");

    ObjectDescriptors& objects = objectDescriptors();
    ObjectDescriptors::const_iterator itr = objects.find(className_);
    BOOST_ASSERT(itr != objects.end());

    const ConfigObjectDescriptor* descriptor = itr->second;
    fields->insert(fields->end(),
                   descriptor->begin(),
                   descriptor->end());
}

bool ConfigObject::has(const ConfigFieldDescriptor* field) const {
    BOOST_ASSERT(field && "field should not be NULL when test has it");

    if (field->optional) {
        switch (field->cppType) {
        case ConfigFieldDescriptor::CPPTYPE_BOOL:
            return constRaw<boost::optional<bool> >(field)->is_initialized();

        case ConfigFieldDescriptor::CPPTYPE_INT32:
            return constRaw<boost::optional<int> >(field)->is_initialized();

        case ConfigFieldDescriptor::CPPTYPE_INT64:
            return constRaw<boost::optional<int64_t> >(field)->is_initialized();

        case ConfigFieldDescriptor::CPPTYPE_DOUBLE:
            return constRaw<boost::optional<double> >(field)->is_initialized();

        default:
            return false;
        }
    }

    switch (field->repeatedType) {
    case ConfigFieldDescriptor::NO_REPEATED:
        switch (field->cppType) {
        case ConfigFieldDescriptor::CPPTYPE_STRING:
            return !constRaw<std::string>(field)->empty();

        case ConfigFieldDescriptor::CPPTYPE_OBJECT:
            return !*constRaw<ConfigObject*>(field);

        default:
            return true;
        }

        break;

    case ConfigFieldDescriptor::LIST:
        switch (field->cppType) {
        case ConfigFieldDescriptor::CPPTYPE_BOOL:
            return !constRaw<std::vector<bool> >(field)->empty();

        case ConfigFieldDescriptor::CPPTYPE_INT32:
            return !constRaw<std::vector<int> >(field)->empty();

        case ConfigFieldDescriptor::CPPTYPE_INT64:
            return !constRaw<std::vector<int64_t> >(field)->empty();

        case ConfigFieldDescriptor::CPPTYPE_DOUBLE:
            return !constRaw<std::vector<double> >(field)->empty();

        case ConfigFieldDescriptor::CPPTYPE_STRING:
            return !constRaw<std::vector<std::string> >(field)->empty();

        case ConfigFieldDescriptor::CPPTYPE_OBJECT:
            return !constRaw<std::vector<ConfigObject*> >(field)->empty();
        }

        break;

    case ConfigFieldDescriptor::MAP:
        switch (field->cppType) {
        case ConfigFieldDescriptor::CPPTYPE_BOOL:
            return !constRaw<std::map<std::string, bool> >(field)->empty();

        case ConfigFieldDescriptor::CPPTYPE_INT32:
            return !constRaw<std::map<std::string, int> >(field)->empty();

        case ConfigFieldDescriptor::CPPTYPE_INT64:
            return !constRaw<std::map<std::string, int64_t> >(field)->empty();

        case ConfigFieldDescriptor::CPPTYPE_DOUBLE:
            return !constRaw<std::map<std::string, double> >(field)->empty();

        case ConfigFieldDescriptor::CPPTYPE_STRING:
            return !constRaw<std::map<std::string, std::string> >(field)->empty();

        case ConfigFieldDescriptor::CPPTYPE_OBJECT:
            return !constRaw<std::map<std::string, ConfigObject*> >(field)->empty();
        }

        break;
    }

    return false;
}

ConfigObject* ConfigObject::mutableObject(const ConfigFieldDescriptor* field) {
    BOOST_ASSERT(field && "mutableObject field should not be NULL");

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
    BOOST_ASSERT(field &&
                 field->repeatedType == ConfigFieldDescriptor::LIST &&
                 "addObject field type must be list");

    std::vector<ConfigObject*>* repeated =
        mutableRaw<std::vector<ConfigObject*> >(field);

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

ConfigObject* ConfigObject::addObject(const ConfigFieldDescriptor* field,
                                      const std::string& key) {
    BOOST_ASSERT(field &&
                 field->repeatedType == ConfigFieldDescriptor::MAP &&
                 "addObject field type must be map");

    if (key.empty()) {
        LOG_ERROR << "can not add an empty key to the map.";
        return NULL;
    }

    std::map<std::string, ConfigObject*>* repeated =
        mutableRaw<std::map<std::string, ConfigObject*> >(field);

    // We must allocate a new object.
    const ConfigObject* prototype =
        ConfigObject::defaultObject(field->className);

    if (NULL == prototype) {
        LOG_ERROR << "can not find such object "
                  << field->className;

        return NULL;
    }

    ConfigObject* newObject = prototype->create();
    newObject->setName(key);
    repeated->insert(std::make_pair(key, newObject));

    return newObject;
}

}
}
