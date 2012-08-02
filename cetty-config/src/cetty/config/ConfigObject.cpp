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

namespace cetty {
namespace config {

ConfigObject::ObjectDescriptorMap ConfigObject::objects;

const ConfigReflection* ConfigObject::getreflection() const {
    ObjectDescriptorMap::const_iterator itr = objects.find(name);

    if (itr != objects.end()) {
        //return itr->second.first->
    }

    return (const ConfigReflection*)(8);
}

const ConfigDescriptor* ConfigObject::getdescriptor() const {
    ObjectDescriptorMap::const_iterator itr = objects.find(name);

    if (itr != objects.end()) {
        return itr->second.descriptor;
    }
}

void ConfigObject::addDescriptor(const std::string& name, ConfigDescriptor* desciptor, ConfigObject* object) {
    BOOST_ASSERT(!name.empty() && desciptor && object);

    if (name.empty() || NULL == desciptor || NULL == object) {
        return;
    }

    ObjectDescriptorMap::const_iterator itr = objects.find(name);

    if (itr != objects.end()) {

    }
    else {
        //objects[name] = std::make_pair(desciptor, objects);
    }
}

}
}
