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

#include <yaml-cpp/yaml.h>
#include <cetty/config/ConfigObject.h>
#include <cetty/config/ConfigDescriptor.h>
#include <cetty/config/ConfigReflection.h>
#include <cetty/config/KeyValuePair.cnf.h>

#include <cetty/logging/LoggerHelper.h>

namespace cetty {
namespace config {

int parseField(const ConfigFieldDescriptor* field,
               const YAML::Node& node,
               ConfigObject* object);

bool parseConfigObject(const YAML::Node& node, ConfigObject* object) {
    if (!object) {
        LOG_ERROR << "parsed object is NULL.";
        return false;
    }

    const ConfigObjectDescriptor* descriptor = object->descriptor();

    ConfigObjectDescriptor::ConstIterator itr = descriptor->begin();

    for (; itr != descriptor->end(); ++itr) {
        const ConfigFieldDescriptor* field = *itr;
        const YAML::Node& fieldNode = node[field->name];

        if (fieldNode.Type() == YAML::NodeType::Null) {
            LOG_INFO << "field " << field->name << " has none value, skip it.";
            continue;
        }

        if (!fieldNode) {
            // in the following situation:
            //
            // // in config header file:
            //
            // class SingleFiledConfig : public cetty::config::ConfigObject {
            //     std::vector<int> numbers;
            // }
            //
            // // in YAML config file:
            //
            // SingleFiledConfig :
            //   - 5
            //
            // will set the '5' to numbers filed in the SingleFiledConfig object.
            if (descriptor->fieldCnt() == 1 && field->repeatedType) {
                if (!parseField(field, node, object)) {
                    return false;
                }
            }
            else {
                LOG_INFO << "has no such field " << field->name << " , skip it.";
                continue;
            }
        }
        else {
            if (!parseField(field, fieldNode, object)) {
                return false;
            }
        }
    }

    return true;
}

int parseField(const ConfigFieldDescriptor* field,
               const YAML::Node& node,
               ConfigObject* object) {
    YAML::Node::const_iterator itr = node.begin();

    switch (field->repeatedType) {
    case ConfigFieldDescriptor::NO_REPEATED:
        switch (field->type) {
        case ConfigFieldDescriptor::CPPTYPE_BOOL:
            if (node.Scalar() == "true" ||
                    node.Scalar() == "1" ||
                    node.Scalar() == "yes") {
                object->set<bool>(field, true);
            }
            else {
                object->set<bool>(field, false);
            }

            break;

        case ConfigFieldDescriptor::CPPTYPE_INT32:
            object->set<int>(field, node.as<int>());
            break;

        case ConfigFieldDescriptor::CPPTYPE_INT64:
            object->set<int64_t>(field, node.as<int64_t>());
            break;

        case  ConfigFieldDescriptor::CPPTYPE_DOUBLE:
            object->set<double>(field, node.as<double>());
            break;

        case ConfigFieldDescriptor::CPPTYPE_STRING:
            object->set<std::string>(field, node.Scalar());
            break;

        case ConfigFieldDescriptor::CPPTYPE_OBJECT:
            ConfigObject* obj = object->mutableObject(field);
            return parseConfigObject(node, obj);
        }

        break;

    case ConfigFieldDescriptor::LIST:
        for (; itr != node.end(); ++itr) {
            switch (field->type) {
            case ConfigFieldDescriptor::CPPTYPE_BOOL:
                break;

            case ConfigFieldDescriptor::CPPTYPE_INT32:
                object->add<int>(field, itr->as<int>());
                break;

            case ConfigFieldDescriptor::CPPTYPE_INT64:
                object->add<int64_t>(field, itr->as<int64_t>());
                break;

            case  ConfigFieldDescriptor::CPPTYPE_DOUBLE:
                object->add<double>(field, itr->as<double>());
                break;

            case ConfigFieldDescriptor::CPPTYPE_STRING:
                object->add<std::string>(field, itr->Scalar());
                break;

            case ConfigFieldDescriptor::CPPTYPE_OBJECT:
                ConfigObject* obj = object->addObject(field);

                // case-1
                // items:
                //   - objectName :
                //       field1 : value
                //       field2 : value
                //
                // case-2
                // items:
                //   - field1 : value
                //     field2 : value

                const std::string& objectName = itr->begin()->first.Scalar();

                if (!objectName.empty() && !obj->descriptor()->hasField(objectName)) {
                    obj->setName(objectName);
                    return parseConfigObject(itr->begin()->second, obj);
                }
                else { // case-2
                    return parseConfigObject(*itr, obj);
                }

                break;
            }
        }

        break;

    case ConfigFieldDescriptor::MAP:
        for (; itr != node.end(); ++itr) {
            switch (field->type) {
            case ConfigFieldDescriptor::CPPTYPE_BOOL:
                break;

            case ConfigFieldDescriptor::CPPTYPE_INT32:
                object->add<int>(field,
                                 itr->first.Scalar(),
                                 itr->second.as<int>());
                break;

            case ConfigFieldDescriptor::CPPTYPE_INT64:
                object->add<int64_t>(field,
                                     itr->first.Scalar(),
                                     itr->as<int64_t>());
                break;

            case  ConfigFieldDescriptor::CPPTYPE_DOUBLE:
                object->add<double>(field,
                                    itr->first.Scalar(),
                                    itr->second.as<double>());
                break;

            case ConfigFieldDescriptor::CPPTYPE_STRING:
                object->add<std::string>(field,
                                         itr->first.Scalar(),
                                         itr->second.Scalar());
                break;

            case ConfigFieldDescriptor::CPPTYPE_OBJECT:
                ConfigObject* obj = object->addObject(field, itr->first.Scalar());

                // case-1
                // items:
                //   - objectName :
                //       field1 : value
                //       field2 : value
                //
                // case-2
                // items:
                //   - field1 : value
                //     field2 : value
                return parseConfigObject(itr->second, obj);

                break;
            }
        }

        break;
    }

    return true;
}

}
}
