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

#include <cetty/config/ConfigReflectionImpl.h>
#include <cetty/config/ConfigDescriptor.h>

namespace cetty {
namespace config {

ConfigObject::ObjectDescriptorMap* ConfigObject::objects_ = NULL;
ConfigReflection* ConfigObject::reflection_ = NULL;

ConfigObject::ConfigObject(const std::string& name)
    : name_(name),
      className_(name) {
    reflection();
}

ConfigObject::~ConfigObject() {
    clear();
}

const ConfigReflection* ConfigObject::reflection() const {
    if (!reflection_) {
        reflection_ = new ConfigReflection;
    }

    return reflection_;
}

const ConfigObjectDescriptor* ConfigObject::descriptor() const {
    ConfigObject::ObjectDescriptorMap& objects = objectDescriptorMap();

    ObjectDescriptorMap::const_iterator itr = objects.find(className_);

    if (itr != objects.end()) {
        return itr->second;
    }

    return NULL;
}

void ConfigObject::addDescriptor(ConfigObjectDescriptor* descriptor) {
    BOOST_ASSERT(descriptor);

    ConfigObject::ObjectDescriptorMap& objects = objectDescriptorMap();
    const std::string& name = descriptor->className();
    ObjectDescriptorMap::const_iterator itr = objects.find(name);

    if (itr != objects.end()) {

    }
    else {
        objects[name] = descriptor;
    }
}

void ConfigObject::clear() {
    std::vector<const ConfigFieldDescriptor*> fields;
    listFields(&fields);

    for (std::size_t i = 0; i < fields.size(); ++i) {
        const ConfigFieldDescriptor* field = fields[i];

        if (field->repeated) {
            switch (field->type) {
            case ConfigFieldDescriptor::CPPTYPE_BOOL:
                reflection_->clearRepeatedField<bool>(this, field);
                break;

            case ConfigFieldDescriptor::CPPTYPE_INT32:
                reflection_->clearRepeatedField<int>(this, field);
                break;

            case ConfigFieldDescriptor::CPPTYPE_INT64:
                reflection_->clearRepeatedField<int64_t>(this, field);
                break;

            case ConfigFieldDescriptor::CPPTYPE_DOUBLE:
                reflection_->clearRepeatedField<double>(this, field);
                break;

            case ConfigFieldDescriptor::CPPTYPE_STRING:
                reflection_->clearRepeatedField<std::string>(this, field);
                break;

            case ConfigFieldDescriptor::CPPTYPE_OBJECT:
                reflection_->clearRepeatedField<ConfigObject*>(this, field);
                break;
            }
        }
        else {
            switch (field->type) {
            case ConfigFieldDescriptor::CPPTYPE_BOOL:
                reflection_->clearField<boost::optional<bool> >(this, field);
                break;

            case ConfigFieldDescriptor::CPPTYPE_INT32:
                reflection_->clearField<boost::optional<int> >(this, field);
                break;

            case ConfigFieldDescriptor::CPPTYPE_INT64:
                reflection_->clearField<boost::optional<int64_t> >(this, field);
                break;

            case ConfigFieldDescriptor::CPPTYPE_DOUBLE:
                reflection_->clearField<boost::optional<double> >(this, field);
                break;

            case ConfigFieldDescriptor::CPPTYPE_STRING:
                reflection_->clearField<std::string>(this, field);
                break;

            case ConfigFieldDescriptor::CPPTYPE_OBJECT:
                reflection_->clearField<ConfigObject*>(this, field);
                break;
            }
        }
    }
}

void ConfigObject::copyFrom(const ConfigObject& from) {
    std::vector<const ConfigFieldDescriptor*> fields;
    listFields(&fields);

    for (std::size_t i = 0; i < fields.size(); ++i) {
        const ConfigFieldDescriptor* field = fields[i];

        if (field->repeated) {
            switch (field->type) {
            case ConfigFieldDescriptor::CPPTYPE_BOOL:
                reflection_->copyRepeatedField<bool>(from, field, this);
                break;

            case ConfigFieldDescriptor::CPPTYPE_INT32:
                reflection_->copyRepeatedField<int>(from, field, this);
                break;

            case ConfigFieldDescriptor::CPPTYPE_INT64:
                reflection_->copyRepeatedField<int64_t>(from, field, this);
                break;

            case ConfigFieldDescriptor::CPPTYPE_DOUBLE:
                reflection_->copyRepeatedField<double>(from, field, this);
                break;

            case ConfigFieldDescriptor::CPPTYPE_STRING:
                reflection_->copyRepeatedField<std::string>(from, field, this);
                break;

            case ConfigFieldDescriptor::CPPTYPE_OBJECT:
                reflection_->copyRepeatedField<ConfigObject*>(from, field, this);
                break;
            }
        }
        else {
            switch (field->type) {
            case ConfigFieldDescriptor::CPPTYPE_BOOL:
                reflection_->copyField<boost::optional<bool> >(from, field, this);
                break;

            case ConfigFieldDescriptor::CPPTYPE_INT32:
                reflection_->copyField<boost::optional<int> >(from, field, this);
                break;

            case ConfigFieldDescriptor::CPPTYPE_INT64:
                reflection_->copyField<boost::optional<int64_t> >(from, field, this);
                break;

            case ConfigFieldDescriptor::CPPTYPE_DOUBLE:
                reflection_->copyField<boost::optional<double> >(from, field, this);
                break;

            case ConfigFieldDescriptor::CPPTYPE_STRING:
                reflection_->copyField<std::string>(from, field, this);
                break;

            case ConfigFieldDescriptor::CPPTYPE_OBJECT:
                reflection_->copyField<ConfigObject*>(from, field, this);
                break;
            }
        }
    }
}

ConfigObject::ObjectDescriptorMap& ConfigObject::objectDescriptorMap() {
    if (!objects_) {
        objects_ = new ObjectDescriptorMap;
    }

    return *objects_;
}

const ConfigObject* ConfigObject::getDefaultObject(const std::string& name) {
    if (name.empty()) {
        return NULL;
    }

    ConfigObject::ObjectDescriptorMap& objects = objectDescriptorMap();
    ObjectDescriptorMap::const_iterator itr = objects.find(name);

    if (itr != objects.end()) {
        return itr->second->defaultInstance();
    }

    return NULL;
}

void ConfigObject::listFields(std::vector<const ConfigFieldDescriptor*>* output) const {
    reflection_->listFields(*this, output);
}

bool ConfigObject::hasInt(const ConfigFieldDescriptor* field) const {
    return reflection_->getInt(*this, field);
}

int ConfigObject::getInt(const ConfigFieldDescriptor* field) const {
    boost::optional<int> value = reflection_->getInt(*this, field);

    if (value) {
        return boost::get<int>(value);
    }

    return 0;
}

bool ConfigObject::hasInt64(const ConfigFieldDescriptor* field) const {
    return reflection_->getInt64(*this, field);
}

int64_t ConfigObject::getInt64(const ConfigFieldDescriptor* field) const {
    boost::optional<int64_t> value = reflection_->getInt64(*this, field);

    if (value) {
        return boost::get<int64_t>(value);
    }

    return 0;
}

bool ConfigObject::hasDouble(const ConfigFieldDescriptor* field) const {
    return reflection_->getDouble(*this, field);
}

double ConfigObject::getDouble(const ConfigFieldDescriptor* field) const {
    boost::optional<double> value = reflection_->getDouble(*this, field);

    if (value) {
        return boost::get<double>(value);
    }

    return 0;
}

bool ConfigObject::hasBool(const ConfigFieldDescriptor* field) const {
    return reflection_->getBool(*this, field);
}

bool ConfigObject::getBool(const ConfigFieldDescriptor* field) const {
    boost::optional<bool> value = reflection_->getBool(*this, field);

    if (value) {
        return boost::get<bool>(value);
    }

    return false;
}

std::string ConfigObject::getString(const ConfigFieldDescriptor* field) const {
    return reflection_->getString(*this, field);
}

void ConfigObject::setInt32(const ConfigFieldDescriptor* field, int value) {
    reflection_->setInt32(this, field, value);
}

void ConfigObject::setInt64(const ConfigFieldDescriptor* field, int64_t value) {
    reflection_->setInt64(this, field, value);
}

void ConfigObject::setDouble(const ConfigFieldDescriptor* field, double value) {
    reflection_->setDouble(this, field, value);
}

void ConfigObject::setBool(const ConfigFieldDescriptor* field, bool value) {
    reflection_->setBool(this, field, value);
}

void ConfigObject::setString(const ConfigFieldDescriptor* field, const std::string& value) {
    reflection_->setString(this, field, value);
}

ConfigObject* ConfigObject::mutableObject(const ConfigFieldDescriptor* field) {
    return reflection_->mutableObject(this, field);
}

void ConfigObject::addInt32(const ConfigFieldDescriptor* field, int value) {
    reflection_->addInt32(this, field, value);
}

void ConfigObject::addInt32(const ConfigFieldDescriptor* field, const std::vector<int>& value) {
    reflection_->addInt32(this, field, value);
}

void ConfigObject::addInt64(const ConfigFieldDescriptor* field, int64_t value) {
    reflection_->addInt64(this, field, value);
}

void ConfigObject::addInt64(const ConfigFieldDescriptor* field, const std::vector<int64_t>& value) {
    reflection_->addInt64(this, field, value);
}

void ConfigObject::addDouble(const ConfigFieldDescriptor* field, double value) {
    reflection_->addDouble(this, field, value);
}

void ConfigObject::addDouble(const ConfigFieldDescriptor* field, const std::vector<double>& value) {
    reflection_->addDouble(this, field, value);
}

void ConfigObject::addBool(const ConfigFieldDescriptor* field, bool value) {
    reflection_->addBool(this, field, value);
}

void ConfigObject::addBool(const ConfigFieldDescriptor* field, const std::vector<bool>& value) {
    reflection_->addBool(this, field, value);
}

void ConfigObject::addString(const ConfigFieldDescriptor* field, const std::string& value) {
    reflection_->addString(this, field, value);
}

void ConfigObject::addString(const ConfigFieldDescriptor* field, const std::vector<std::string>& value) {
    reflection_->addString(this, field, value);
}

ConfigObject* ConfigObject::addObject(const ConfigFieldDescriptor* field) {
    return reflection_->addObject(this, field);
}

}
}
