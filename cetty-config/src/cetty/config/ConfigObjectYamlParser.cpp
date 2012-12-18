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
#include <cetty/config/KeyValuePair.h>
#include <cetty/config/ConfigDescriptor.h>
#include <cetty/config/ConfigReflection.h>

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

    const ConfigObjectDescriptor* descriptor = object->getDescriptor();

    ConfigObjectDescriptor::ConstIterator itr = descriptor->begin();

    for (; itr != descriptor->end(); ++itr) {
        const ConfigFieldDescriptor* field = *itr;
        const YAML::Node fieldNode = node[field->name];

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
            if (descriptor->fieldCnt() == 1 && field->repeated) {
                if (!parseField(field, node, object)) {
                    return false;
                }
            }
            else if (object->name() == KeyValuePair::NAME) {
                KeyValuePair* kv = dynamic_cast<KeyValuePair*>(object);
                if (kv) {
                    kv->key = node.begin()->first.Scalar();
                    kv->value = node.begin()->second.Scalar();
                }
                else {
                    LOG_ERROR << "the config object name is "
                        << KeyValuePair::NAME
                        << " , but the instance is not KeyValuePair";

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

    const ConfigReflection* reflection = object->getReflection();

    if (field->repeated) {
        YAML::Node::const_iterator itr = node.begin();

        for (; itr != node.end(); ++itr) {
            switch (field->type) {
            case ConfigFieldDescriptor::CPPTYPE_INT32:
                reflection->addInt32(object, field, itr->as<int>());
                break;

            case ConfigFieldDescriptor::CPPTYPE_INT64:
                reflection->addInt64(object, field, itr->as<int64_t>());
                break;

            case  ConfigFieldDescriptor::CPPTYPE_DOUBLE:
                reflection->addDouble(object, field, itr->as<double>());
                break;

            case ConfigFieldDescriptor::CPPTYPE_STRING:
                reflection->addString(object, field, itr->Scalar());
                break;

            case ConfigFieldDescriptor::CPPTYPE_OBJECT:
                ConfigObject* obj = reflection->addObject(object, field);

                // items:
                //   - objectName :
                //       field1 : value
                //       field2 : value

                const std::string& objectName = itr->first.Scalar();
                if (!objectName.empty()) {
                    obj->setName(objectName);
                } 

                if (!parseConfigObject(*itr, obj)) {
                    return false;
                }

                break;
            }
        }
    }
    else {
        switch (field->type) {
        case ConfigFieldDescriptor::CPPTYPE_INT32:
            reflection->setInt32(object, field, node.as<int>());
            break;

        case ConfigFieldDescriptor::CPPTYPE_INT64:
            reflection->setInt64(object, field, node.as<int64_t>());
            break;

        case  ConfigFieldDescriptor::CPPTYPE_DOUBLE:
            reflection->setDouble(object, field, node.as<double>());
            break;

        case ConfigFieldDescriptor::CPPTYPE_STRING:
            reflection->setString(object, field, node.Scalar());
            break;

        case ConfigFieldDescriptor::CPPTYPE_OBJECT:
            ConfigObject* obj = reflection->mutableObject(object, field);
            return parseConfigObject(node, obj);
            break;
        }
    }

    return true;
}

}
}
