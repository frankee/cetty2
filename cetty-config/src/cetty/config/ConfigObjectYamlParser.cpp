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

namespace cetty {
namespace config {

int parseField(const ConfigFieldDescriptor* field,
               const YAML::Node& node,
               ConfigObject* object);

bool parseConfigObject(const YAML::Node& node, ConfigObject* object) {
    if (!object) {
        return false;
    }

    const ConfigReflection* reflection = object->getreflection();
    const ConfigDescriptor* descriptor = object->getdescriptor();

    ConfigDescriptor::ConstIterator itr = descriptor->begin();

    for (; itr != descriptor->end(); ++itr) {
        const ConfigFieldDescriptor* field = *itr;
        const YAML::Node fieldNode = node[field->name];

        YAML::NodeType::value type = fieldNode.Type();

        if (!fieldNode) {
            //if (fieldNode.Type() == YAML::NodeType::Null) {
            if (descriptor->fieldCnt() == 1 && field->repeated) {
                if (!parseField(field, node, object)) {
                    return false;
                }
            }
            else {
                return false;
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

    const ConfigReflection* reflection = object->getreflection();

    if (field->repeated) {
        //if (!value.isArray()) {
        //    printf("protobuf field is repeated, but json not.\n");
        //    return false;
        //}

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
                ConfigObject* obj = reflection->addConfigObject(object, field);

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
            ConfigObject* obj = reflection->mutableConfigObject(object, field);
            return parseConfigObject(node, obj);
            break;
        }
    }

    return true;
}

}
}
