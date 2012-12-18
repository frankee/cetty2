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

#include <cetty/config/ConfigReflection.h>
#include <cetty/config/ConfigDescriptor.h>

namespace cetty {
namespace config {

ConfigObject::ObjectDescriptorMap* ConfigObject::objects_ = NULL;
ConfigReflection* ConfigObject::reflection_ = NULL;

const ConfigReflection* ConfigObject::getReflection() const {
    if (!reflection_) {
        reflection_ = new ConfigReflection;
    }

    return reflection_;
}

const ConfigObjectDescriptor* ConfigObject::getDescriptor() const {
    ConfigObject::ObjectDescriptorMap& objects = objectDescriptorMap();

    ObjectDescriptorMap::const_iterator itr = objects.find(name_);

    if (itr != objects.end()) {
        return itr->second;
    }

    return NULL;
}

void ConfigObject::addDescriptor(const std::string& name,
                                 ConfigObjectDescriptor* desciptor,
                                 ConfigObject* object) {
    
}

void ConfigObject::addDescriptor(ConfigObjectDescriptor* descriptor) {
    BOOST_ASSERT(descriptor);

    ConfigObject::ObjectDescriptorMap& objects = objectDescriptorMap();
    const std::string& name = descriptor->name();
    ObjectDescriptorMap::const_iterator itr = objects.find(name);

    if (itr != objects.end()) {

    }
    else {
        objects[name] = descriptor;
    }
}

void ConfigObject::clear() {

}

void ConfigObject::copyFrom(const ConfigObject& from) {

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

}
}
