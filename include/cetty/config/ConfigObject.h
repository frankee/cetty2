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
    ConfigObject(const std::string& name);
    virtual ~ConfigObject();

    const ConfigReflection* reflection() const;
    const ConfigObjectDescriptor* descriptor() const;

    const std::string& name() const;
    void setName(const std::string& name);

    const std::string& className() const;

    virtual ConfigObject* create() const = 0;

    virtual void clear();
    virtual void copyFrom(const ConfigObject& from);

    // reflections

    void listFields(std::vector<const ConfigFieldDescriptor*>* output) const;

    bool hasInt(const ConfigFieldDescriptor* field) const;

    int getInt(const ConfigFieldDescriptor* field) const;

    bool hasInt64(const ConfigFieldDescriptor* field) const;

    int64_t getInt64(const ConfigFieldDescriptor* field) const;

    bool hasDouble(const ConfigFieldDescriptor* field) const;

    double getDouble(const ConfigFieldDescriptor* field) const;

    bool hasBool(const ConfigFieldDescriptor* field) const;

    bool getBool(const ConfigFieldDescriptor* field) const;

    std::string getString(const ConfigFieldDescriptor* field) const;

    void setInt32(const ConfigFieldDescriptor* field,
                  int value);

    void setInt64(const ConfigFieldDescriptor* field,
                  int64_t value);
    void setDouble(const ConfigFieldDescriptor* field,
                   double value);
    void setBool(const ConfigFieldDescriptor* field,
                 bool value);
    void setString(const ConfigFieldDescriptor* field,
                   const std::string& value);

    ConfigObject* mutableObject(const ConfigFieldDescriptor* field);

    void addInt32(const ConfigFieldDescriptor* field,
                  int value);
    void addInt32(const ConfigFieldDescriptor* field,
                  const std::vector<int>& value);
    void addInt64(const ConfigFieldDescriptor* field,
                  int64_t value);
    void addInt64(const ConfigFieldDescriptor* field,
                  const std::vector<int64_t>& value);
    void addDouble(const ConfigFieldDescriptor* field,
                   double value);
    void addDouble(const ConfigFieldDescriptor* field,
                   const std::vector<double>& value);
    void addBool(const ConfigFieldDescriptor* field,
                 bool value);
    void addBool(const ConfigFieldDescriptor* field,
                 const std::vector<bool>& value);
    void addString(const ConfigFieldDescriptor* field,
                   const std::string& value);
    void addString(const ConfigFieldDescriptor* field,
                   const std::vector<std::string>& value);

    ConfigObject* addObject(const ConfigFieldDescriptor* field);

public:
    static void addDescriptor(ConfigObjectDescriptor* descriptor);

    static const ConfigObject* getDefaultObject(const std::string& name);

private:
    std::string name_;
    std::string className_;

private:
    typedef std::map<std::string, ConfigObjectDescriptor*> ObjectDescriptorMap;

private:
    static ObjectDescriptorMap& objectDescriptorMap();

    static ObjectDescriptorMap* objects_;
    static ConfigReflection* reflection_;
};

inline
const std::string& ConfigObject::className() const {
    return className_;
}

inline
const std::string& ConfigObject::name() const {
    return name_;
}

inline
void ConfigObject::setName(const std::string& name) {
    name_ = name;
}

}
}

#endif //#if !defined(CETTY_CONFIG_CONFIGOBJECT_H)

// Local Variables:
// mode: c++
// End:
