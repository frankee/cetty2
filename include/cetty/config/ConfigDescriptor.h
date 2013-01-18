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
                              #CPP_TYPE\
                             )\
     
#define CETTY_CONFIG_OBJECT_FIELD(TYPE, FIELD, CLASS) \
    new ::cetty::config::ConfigFieldDescriptor(\
                              static_cast<int>(reinterpret_cast<const char*>(\
                                      &reinterpret_cast<const TYPE*>(16)->FIELD) - reinterpret_cast<const char*>(16)),\
                              ::cetty::config::ConfigFieldDescriptor::CPPTYPE_OBJECT,\
                              #FIELD,\
                              #CLASS\
                             )\
     
#define CETTY_CONFIG_REPEATED_FIELD(TYPE, FIELD, CPP_TYPE) \
    new ::cetty::config::ConfigFieldDescriptor(\
                              static_cast<int>(reinterpret_cast<const char*>(\
                                      &reinterpret_cast<const TYPE*>(16)->FIELD) - reinterpret_cast<const char*>(16)),\
                              ::cetty::config::ConfigFieldDescriptor::CPPTYPE_##CPP_TYPE,\
                              #FIELD,\
                              #CPP_TYPE,\
                              true\
                             )\
     
#define CETTY_CONFIG_REPEATED_OBJECT_FIELD(TYPE, FIELD, CLASS) \
    new ::cetty::config::ConfigFieldDescriptor(\
                              static_cast<int>(reinterpret_cast<const char*>(\
                                      &reinterpret_cast<const TYPE*>(16)->FIELD) - reinterpret_cast<const char*>(16)),\
                              ::cetty::config::ConfigFieldDescriptor::CPPTYPE_OBJECT,\
                              #FIELD,\
                              #CLASS,\
                              true\
                             )\
     

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

    bool repeated;

    int offset;
    int type;
    std::string name;
    std::string className;

    ConfigFieldDescriptor(int offset,
                          int type,
                          const char* name,
                          const char* className,
                          bool repeated = false)
        : repeated(repeated),
          offset(offset),
          type(type),
          name(name),
          className(className) {
    }
};

typedef ConfigFieldDescriptor const* ConstConfigFieldDescriptorPtr;

class ConfigObjectDescriptor : private boost::noncopyable {
public:
    typedef std::vector<ConstConfigFieldDescriptorPtr> FieldDescriptors;
    typedef FieldDescriptors::const_iterator ConstIterator;

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

private:
    FieldDescriptors fields_;
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

}
}

#endif //#if !defined(CETTY_CONFIG_CONFIGDESCRIPTOR_H)

// Local Variables:
// mode: c++
// End:
