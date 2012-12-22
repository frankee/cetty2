
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

#include <boost/program_options.hpp>

#include <cetty/util/SimpleTrie.h>
#include <cetty/config/ConfigObject.h>
#include <cetty/config/ConfigCenter.h>
#include <cetty/config/ConfigDescriptor.h>

#include <cetty/logging/LoggerHelper.h>

namespace cetty {
namespace config {

using namespace boost::program_options;
using namespace cetty::util;

static int parseField(const ConfigFieldDescriptor* field,
                      const variable_value& option,
                      const ConfigCenter::CmdlineTrie& cmdline,
                      ConfigObject* object) {

    if (field->repeated) {
        switch (field->type) {
        case ConfigFieldDescriptor::CPPTYPE_INT32:
            object->addInt32(field, option.as<std::vector<int> >());
            break;

        case ConfigFieldDescriptor::CPPTYPE_INT64:
            object->addInt64(field, option.as<std::vector<int64_t> >());
            break;

        case  ConfigFieldDescriptor::CPPTYPE_DOUBLE:
            object->addDouble(field, option.as<std::vector<double> >());
            break;

        case ConfigFieldDescriptor::CPPTYPE_STRING:
            object->addString(field, option.as<std::vector<std::string> >());
            break;

        case ConfigFieldDescriptor::CPPTYPE_OBJECT:
            //                    ConfigObject* obj = object->addObject(field);
            //
            //                     if (!parseConfigObject(*itr, obj)) {
            //                         return false;
            //                     }

            break;
        }
    }
    else {
        switch (field->type) {
        case ConfigFieldDescriptor::CPPTYPE_INT32:
            object->setInt32(field, option.as<int>());
            break;

        case ConfigFieldDescriptor::CPPTYPE_INT64:
            object->setInt64(field, option.as<int64_t>());
            break;

        case  ConfigFieldDescriptor::CPPTYPE_DOUBLE:
            object->setDouble(field, option.as<double>());
            break;

        case ConfigFieldDescriptor::CPPTYPE_STRING:
            object->setString(field, option.as<std::string>());
            break;

        case ConfigFieldDescriptor::CPPTYPE_OBJECT:
            ConfigObject* obj = object->mutableObject(field);
            //return parseConfigObject(, obj);
            break;
        }
    }

    return true;
}

bool parseConfigObject(const variables_map& vm,
                       const ConfigCenter::CmdlineTrie& cmdline,
                       ConfigObject* object) {
    if (!object) {
        LOG_ERROR << "parsed object is NULL.";
        return false;
    }

    const ConfigObjectDescriptor* descriptor = object->descriptor();
    ConfigObjectDescriptor::ConstIterator itr = descriptor->begin();

    if (cmdline.countPrefix(descriptor->className()) == 0) {
        LOG_INFO << "there is no filed set with cmdline in "
                 << descriptor->className();

        return true;
    }

    for (; itr != descriptor->end(); ++itr) {
        const ConfigFieldDescriptor* field = *itr;

        std::string* value = cmdline.getValue(field->name);

        if (!value) {
            continue;
        }

        if (vm.count(*value) == 0) {
            LOG_INFO << "field " << field->name << " has none value, skip it.";
            continue;
        }

        if (!parseField(field, vm[*value], cmdline, object)) {
            return false;
        }
    }

    return true;
}

}
}
