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

#include <cetty/config/ConfigDescriptor.h>

#include <cstdarg>
#include <cstdlib>
#include <boost/assert.hpp>
#include <cetty/util/StringUtil.h>
#include <cetty/config/ConfigObject.h>

namespace cetty {
namespace config {

ConfigObjectDescriptor::ConfigObjectDescriptor(ConfigObject* defaultInstance,
        int count,
        ConstConfigFieldDescriptorPtr descriptor,
        ...) {
    BOOST_ASSERT(defaultInstance);

    defaultInstance_ = defaultInstance;

    if (count <= 0 || !descriptor) {
        return;
    }

    va_list valist;
    va_start(valist, descriptor);

    fields_.push_back(descriptor);
    fieldMap_.insert(std::make_pair(descriptor->name, descriptor));

    for (int i = 1; i < count; ++i) {
        ConstConfigFieldDescriptorPtr field
            = va_arg(valist, ConstConfigFieldDescriptorPtr);

        if (!field) {
            break;
        }

        fields_.push_back(field);
        fieldMap_.insert(std::make_pair(field->name, field));
    }

    va_end(valist);
}

ConfigObjectDescriptor::~ConfigObjectDescriptor() {
    std::size_t j = fields_.size();

    for (std::size_t i = 0; i < j; ++i) {
        ConstConfigFieldDescriptorPtr& field = fields_[i];

        if (field) {
            delete field;
            field = NULL;
        }
    }

    if (defaultInstance_) {
        delete defaultInstance_;
    }
}

const std::string& ConfigObjectDescriptor::className() const {
    return defaultInstance_->className();
}

std::string ConfigFieldDescriptor::toString() const {
    std::string tmp;
    StringUtil::printf(&tmp, "field: %s(%s)", name.c_str(), className.c_str());
    return tmp;
}

}
}
