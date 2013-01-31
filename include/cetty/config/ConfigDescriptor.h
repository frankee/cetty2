#if !defined(CETTY_CONFIG_CONFIGDESCRIPTOR_H)
#define CETTY_CONFIG_CONFIGDESCRIPTOR_H

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
#include <vector>
#include <string>
#include <boost/noncopyable.hpp>

namespace cetty {
namespace config {

class ConfigObject;

#define CETTY_CONFIG_ADD_DESCRIPTOR(Object, fieldCnt, field, ...) \
    class DescriptorRegister##Object {\
    public:\
        DescriptorRegister##Object() {\
            ::cetty::config::ConfigObject::addDescriptor(\
                    new ::cetty::config::ConfigObjectDescriptor(\
                            new Object,\
                            fieldCnt,\
                            field,\
                            ##__VA_ARGS__));\
        }\
    };\
    static DescriptorRegister##Object descriptorRegister##Object;


#define CETTY_CONFIG_FIELD(TYPE, FIELD, CPP_TYPE) \
    new ::cetty::config::ConfigFieldDescriptor(\
            static_cast<int>(reinterpret_cast<const char*>(\
                             &reinterpret_cast<const TYPE*>(16)->FIELD) - reinterpret_cast<const char*>(16)),\
            ::cetty::config::ConfigFieldDescriptor::CPPTYPE_##CPP_TYPE,\
            #FIELD,\
            #CPP_TYPE,\
            ::cetty::config::ConfigFieldDescriptor::NO_REPEATED,\
            false)

#define CETTY_CONFIG_OPTIONAL_FIELD(TYPE, FIELD, CPP_TYPE) \
    new ::cetty::config::ConfigFieldDescriptor(\
            static_cast<int>(reinterpret_cast<const char*>(\
                             &reinterpret_cast<const TYPE*>(16)->FIELD) - reinterpret_cast<const char*>(16)),\
            ::cetty::config::ConfigFieldDescriptor::CPPTYPE_##CPP_TYPE,\
            #FIELD,\
            #CPP_TYPE,\
            ::cetty::config::ConfigFieldDescriptor::NO_REPEATED,\
            true)

#define CETTY_CONFIG_OBJECT_FIELD(TYPE, FIELD, CLASS) \
    new ::cetty::config::ConfigFieldDescriptor(\
            static_cast<int>(reinterpret_cast<const char*>(\
                             &reinterpret_cast<const TYPE*>(16)->FIELD) - reinterpret_cast<const char*>(16)),\
            ::cetty::config::ConfigFieldDescriptor::CPPTYPE_OBJECT,\
            #FIELD,\
            #CLASS,\
            ::cetty::config::ConfigFieldDescriptor::NO_REPEATED,\
            false)

#define CETTY_CONFIG_LIST_FIELD(TYPE, FIELD, CPP_TYPE) \
    new ::cetty::config::ConfigFieldDescriptor(\
            static_cast<int>(reinterpret_cast<const char*>(\
                             &reinterpret_cast<const TYPE*>(16)->FIELD) - reinterpret_cast<const char*>(16)),\
            ::cetty::config::ConfigFieldDescriptor::CPPTYPE_##CPP_TYPE,\
            #FIELD,\
            #CPP_TYPE,\
            ::cetty::config::ConfigFieldDescriptor::LIST,\
            false)

#define CETTY_CONFIG_LIST_OBJECT_FIELD(TYPE, FIELD, CLASS) \
    new ::cetty::config::ConfigFieldDescriptor(\
            static_cast<int>(reinterpret_cast<const char*>(\
                             &reinterpret_cast<const TYPE*>(16)->FIELD) - reinterpret_cast<const char*>(16)),\
            ::cetty::config::ConfigFieldDescriptor::CPPTYPE_OBJECT,\
            #FIELD,\
            #CLASS,\
            ::cetty::config::ConfigFieldDescriptor::LIST,\
            false)

#define CETTY_CONFIG_MAP_FIELD(TYPE, FIELD, CPP_TYPE) \
    new ::cetty::config::ConfigFieldDescriptor(\
            static_cast<int>(reinterpret_cast<const char*>(\
                             &reinterpret_cast<const TYPE*>(16)->FIELD) - reinterpret_cast<const char*>(16)),\
            ::cetty::config::ConfigFieldDescriptor::CPPTYPE_##CPP_TYPE,\
            #FIELD,\
            #CPP_TYPE,\
            ::cetty::config::ConfigFieldDescriptor::MAP,\
            false)

#define CETTY_CONFIG_MAP_OBJECT_FIELD(TYPE, FIELD, CLASS) \
    new ::cetty::config::ConfigFieldDescriptor(\
            static_cast<int>(reinterpret_cast<const char*>(\
                             &reinterpret_cast<const TYPE*>(16)->FIELD) - reinterpret_cast<const char*>(16)),\
            ::cetty::config::ConfigFieldDescriptor::CPPTYPE_OBJECT,\
            #FIELD,\
            #CLASS,\
            ::cetty::config::ConfigFieldDescriptor::MAP,\
            false)

class ConfigFieldDescriptor {
public:
    // Specifies the C++ data type used to represent the field.  There is a
    // fixed mapping from Type to CppType where each Type maps to exactly one
    // CppType.  0 is reserved for errors.
    enum CppType {
        CPPTYPE_INT32       =  1,
        CPPTYPE_INT64       =  2,
        CPPTYPE_DOUBLE      =  5,
        CPPTYPE_BOOL        =  7,
        CPPTYPE_STRING      =  9,
        CPPTYPE_OBJECT      = 10,

        MAX_CPPTYPE         = 10,    // Constant useful for defining lookup tables
    };

    enum RepeatedType {
        NO_REPEATED = 0,
        LIST        = 1,
        MAP         = 2
    };

    bool optional;
    int  offset;
    int  type;
    int  repeatedType;

    std::string name;
    std::string className;

    ConfigFieldDescriptor(int offset,
                          int type,
                          const char* name,
                          const char* className,
                          int repeatedType,
                          bool optional)
        : optional(optional),
          offset(offset),
          type(type),
          repeatedType(repeatedType),
          name(name),
          className(className) {
    }
};

typedef ConfigFieldDescriptor const* ConstConfigFieldDescriptorPtr;

class ConfigObjectDescriptor : private boost::noncopyable {
public:
    typedef std::vector<ConstConfigFieldDescriptorPtr> FieldDescriptors;
    typedef FieldDescriptors::const_iterator ConstIterator;

    typedef std::map<std::string, ConstConfigFieldDescriptorPtr> FieldDescriptorMap;

public:
    ConfigObjectDescriptor(ConfigObject* defaultInstance,
                           int count,
                           ConstConfigFieldDescriptorPtr descriptor, ...);

    ~ConfigObjectDescriptor();

public:
    const std::string& className() const;

    const ConfigObject* defaultInstance() const;

    ConstIterator begin() const;
    ConstIterator end() const;

    int fieldCnt() const;
    ConstConfigFieldDescriptorPtr field(int index) const;
    ConstConfigFieldDescriptorPtr field(const std::string& name) const;

    bool hasField(const std::string& name) const;

private:
    FieldDescriptors fields_;
    FieldDescriptorMap fieldMap_;
    ConfigObject* defaultInstance_;
};

inline
const ConfigObject* ConfigObjectDescriptor::defaultInstance() const {
    return defaultInstance_;
}

inline
ConfigObjectDescriptor::ConstIterator ConfigObjectDescriptor::begin() const {
    return fields_.begin();
}

inline
ConfigObjectDescriptor::ConstIterator ConfigObjectDescriptor::end() const {
    return fields_.end();
}

inline
int ConfigObjectDescriptor::fieldCnt() const {
    return static_cast<int>(fields_.size());
}

inline
ConstConfigFieldDescriptorPtr ConfigObjectDescriptor::field(int index) const {
    return fields_[index];
}

inline
ConstConfigFieldDescriptorPtr
ConfigObjectDescriptor::field(const std::string& name) const {
    FieldDescriptorMap::const_iterator itr = fieldMap_.find(name);
    if (itr != fieldMap_.end()) {
        return itr->second;
    }
    return NULL;
}

inline
bool ConfigObjectDescriptor::hasField(const std::string& name) const {
    return fieldMap_.find(name) != fieldMap_.end();
}

}
}

#endif //#if !defined(CETTY_CONFIG_CONFIGDESCRIPTOR_H)

// Local Variables:
// mode: c++
// End:
