#if !defined(CETTY_CONFIG_CONFIGOBJECT_H)
#define CETTY_CONFIG_CONFIGOBJECT_H

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

#include <map>
#include <string>
#include <vector>
#include <cetty/config/ConfigReflection.h>

namespace cetty {
namespace config {

class ConfigObject {
public:
    ConfigObject() {}
    ConfigObject(const std::string& name)
        : name_(name) {}

    virtual ~ConfigObject() {}

    virtual const ConfigReflection* getReflection() const;
    virtual const ConfigObjectDescriptor* getDescriptor() const;

    virtual ConfigObject* create() const = 0;

    const std::string& name() const { return name_; }
    void setName(const std::string& name) { name_ = name; }

    virtual void clear();
    virtual void copyFrom(const ConfigObject& from);

    void listFields(std::vector<const ConfigFieldDescriptor*>* output) const {
        reflection_->listFields(*this, output);
    }

    int getInt(const ConfigFieldDescriptor* field) const {
        return reflection_->getInt(*this, field);
    }

    int64_t getInt64(const ConfigFieldDescriptor* field) const {
        return reflection_->getInt64(*this, field);
    }

    double getDouble(const ConfigFieldDescriptor* field) const {
        return reflection_->getDouble(*this, field);
    }

    bool getBool(const ConfigFieldDescriptor* field) const {
        return reflection_->getBool(*this, field);
    }

    std::string getString(const ConfigFieldDescriptor* field) const {
        return reflection_->getString(*this, field);
    }

    void setInt32(const ConfigFieldDescriptor* field,
                  int value) {
        reflection_->setInt32(this, field, value);
    }

    void setInt64(const ConfigFieldDescriptor* field,
                  int64_t value) {
        reflection_->setInt64(this, field, value);
    }
    void setDouble(const ConfigFieldDescriptor* field,
                   double value) {
        reflection_->setDouble(this, field, value);
    }
    void setBool(const ConfigFieldDescriptor* field,
                 bool value) {
        reflection_->setBool(this, field, value);
    }
    void setString(const ConfigFieldDescriptor* field,
                   const std::string& value) {
        reflection_->setString(this, field, value);
    }

    ConfigObject* mutableObject(const ConfigFieldDescriptor* field) {
        reflection_->mutableObject(this, field);
    }

    void addInt32(const ConfigFieldDescriptor* field,
                  int value) {
        reflection_->addInt32(this, field, value);
    }
    void addInt32(const ConfigFieldDescriptor* field,
        const std::vector<int>& value) {
            reflection_->addInt32(this, field, value);
    }
    void addInt64(const ConfigFieldDescriptor* field,
                  int64_t value) {
        reflection_->addInt64(this, field, value);
    }
    void addInt64(const ConfigFieldDescriptor* field,
        const std::vector<int64_t>& value) {
            reflection_->addInt64(this, field, value);
    }
    void addDouble(const ConfigFieldDescriptor* field,
                   double value) {
        reflection_->addDouble(this, field, value);
    }
    void addDouble(const ConfigFieldDescriptor* field,
        const std::vector<double>& value) {
            reflection_->addDouble(this, field, value);
    }
    void addBool(const ConfigFieldDescriptor* field,
                 bool value) {
        reflection_->addBool(this, field, value);
    }
    void addBool(const ConfigFieldDescriptor* field,
        const std::vector<bool>& value) {
            reflection_->addBool(this, field, value);
    }
    void addString(const ConfigFieldDescriptor* field,
                   const std::string& value) {
        reflection_->addString(this, field, value);
    }
    void addString(const ConfigFieldDescriptor* field,
        const std::vector<std::string>& value) {
            reflection_->addString(this, field, value);
    }

    ConfigObject* addObject(const ConfigFieldDescriptor* field) {
        return reflection_->addObject(this, field);
    }

public:
    static void addDescriptor(ConfigObjectDescriptor* descriptor);

    static const ConfigObject* getDefaultObject(const std::string& name);

private:
    std::string name_;

private:
    typedef std::map<std::string, ConfigObjectDescriptor*> ObjectDescriptorMap;

private:
    static ObjectDescriptorMap& objectDescriptorMap();

    static ObjectDescriptorMap* objects_;
    static ConfigReflection* reflection_;
};

}
}

#endif //#if !defined(CETTY_CONFIG_CONFIGOBJECT_H)

// Local Variables:
// mode: c++
// End:
