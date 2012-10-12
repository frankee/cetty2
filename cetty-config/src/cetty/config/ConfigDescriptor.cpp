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

namespace cetty {
namespace config {
    
ConfigDescriptor::ConfigDescriptor(int count, ConstConfigFieldDescriptorPtr descriptor, ...) {
    va_list valist;

    if (!descriptor) {
        return;
    }

    va_start(valist, descriptor);

    fields.push_back(descriptor);
    for (int i = 1; i < count; ++i) {
        ConstConfigFieldDescriptorPtr field
            = va_arg(valist, ConstConfigFieldDescriptorPtr);
        
        if (!field) {
            break;
        }

        fields.push_back(field);
    }

    va_end(valist);
}

ConfigDescriptor::~ConfigDescriptor() {
    std::size_t j = fields.size();
    for (std::size_t i = 0; i < j; ++i) {
        ConstConfigFieldDescriptorPtr& field = fields[i];
        if (field) {
            delete field;
            field = NULL;
        }
    }
}

ConfigDescriptor::ConstIterator ConfigDescriptor::begin() const {
    return fields.begin();
}

ConfigDescriptor::ConstIterator ConfigDescriptor::end() const {
    return fields.end();
}

}
}
