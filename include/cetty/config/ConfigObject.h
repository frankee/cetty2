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

class ConfigObject;
typedef ConfigObject* ConfigObjectPtr;
typedef ConfigObject const* ConstConfigObjectPtr;

class ConfigObject {
public:
    typedef boost::optional<int> IntOption;
    typedef boost::optional<int64_t> Int64Option;
    typedef boost::optional<double> DoubleOption;
    typedef boost::optional<bool> BoolOption;

    typedef std::map<std::string, int> IntMap;
    typedef std::map<std::string, int64_t> Int64Map;
    typedef std::map<std::string, double> DoubleMap;
    typedef std::map<std::string, bool> BoolMap;
    typedef std::map<std::string, ConfigObject*> ObjectMap;

    typedef ConfigObject* Ptr;
    typedef ConfigObject const* ConstPtr;

private:
    typedef std::map<std::string, ConfigObjectDescriptor*> ObjectDescriptors;

public:
    ConfigObject(const std::string& name);
    virtual ~ConfigObject();

    const ConfigObjectDescriptor* descriptor() const;

    const std::string& name() const;
    void setName(const std::string& name);

    const std::string& className() const;

    virtual ConfigObject* create() const = 0;

    virtual void clear();
    virtual void copyFrom(const ConfigObject& from);

    // reflections
    void list(std::vector<const ConfigFieldDescriptor*>* fields) const;

    bool has(const ConfigFieldDescriptor* field) const;

    template<typename T>
    const T& get(const ConfigFieldDescriptor* field) const {
        return *constRaw<T>(field);
    }

    int getInt(const ConfigFieldDescriptor* field) const;
    int64_t getInt64(const ConfigFieldDescriptor* field) const;
    double getDouble(const ConfigFieldDescriptor* field) const;
    bool getBool(const ConfigFieldDescriptor* field) const;
    const std::string& getString(const ConfigFieldDescriptor* field) const;

    template<typename T>
    void set(const ConfigFieldDescriptor* field, const T& value) {
        BOOST_STATIC_ASSERT(!IsVector<T>::VALUE && !IsMap<T>::VALUE);

        BOOST_ASSERT(field);

        if (field->optional) {
            *mutableRaw<boost::optional<T> >(field) = value;
        }
        else {
            *mutableRaw<T>(field) = value;
        }
    }

    void set(const ConfigFieldDescriptor* field, const std::string& value) {
        BOOST_ASSERT(field);
        mutableRaw<std::string>(field)->assign(value);
    }

    void set(const ConfigFieldDescriptor* field, const ConfigObject& value) {
        BOOST_ASSERT(field);
        mutableObject(field)->copyFrom(value);
    }

    void set(const ConfigFieldDescriptor* field, const ConfigObject*& value) {
        BOOST_ASSERT(field);

        if (value) {
            mutableObject(field)->copyFrom(*value);
        }
    }

    template<typename T>
    T* mutableField(const ConfigFieldDescriptor* field) {
        BOOST_STATIC_ASSERT(!IsPointer<T>::VALUE);
        return mutableRaw<T>(field);
    }

    ConfigObject* mutableObject(const ConfigFieldDescriptor* field);

    template<typename T>
    void add(const ConfigFieldDescriptor* field, const T& value) {
        BOOST_STATIC_ASSERT(!IsPointer<T>::VALUE && !IsMap<T>::VALUE);
        mutableRaw<std::vector<T> >(field)->push_back(value);
    }

    template<typename T>
    void add(const ConfigFieldDescriptor* field,
             const std::string& key,
             const T& value) {
        BOOST_STATIC_ASSERT(!IsPointer<T>::VALUE);
        mutableRaw<std::map<std::string, T> >(field)->insert(
            std::make_pair(key, value));
    }

    template<typename T>
    void add(const ConfigFieldDescriptor* field,
             const std::vector<T>& values) {
        BOOST_ASSERT(!IsPointer<T>::VALUE);
        std::vector<T>* to = mutableRaw<std::vector<T> >(field);

        if (to) {
            to->insert(to->end(), value.begin(), value.end());
        }
    }

    template<typename T>
    void add(const ConfigFieldDescriptor* field,
             const std::map<std::string, T>& values) {
        BOOST_ASSERT(!IsPointer<T>::VALUE);
    }

    ConfigObject* addObject(const ConfigFieldDescriptor* field);
    ConfigObject* addObject(const ConfigFieldDescriptor* field,
                            const std::string& key);

public:
    static void addDescriptor(ConfigObjectDescriptor* descriptor);
    static const ConfigObject* defaultObject(const std::string& name);

    static ObjectDescriptors& objectDescriptors();

private:
    template <typename T>
    inline T* mutableRaw(const ConfigFieldDescriptor* field) const {
        void* ptr = reinterpret_cast<uint8_t*>(this) + field->offset;
        return reinterpret_cast<T*>(ptr);
    }

    template<typename t>
    inline const T* constRaw(const ConfigFieldDescriptor* field) const {
        const void* ptr = reinterpret_cast<const uint8_t*>(this) +
                          field->offset;
        return reinterpret_cast<const T*>(ptr);
    }

    template<typename T>
    void copyField(const ConfigFieldDescriptor* field, const ConfigObject& from) {
        BOOST_ASSERT(field);

        if (field->optional &&
                field->type != CPPTYPE_STRING &&
                field->type != CPPTYPE_OBJECT) {
            const boost::optional<T>& value =
                from.get<boost::optional<T> >(field);

            if (value) {
                set<T>(field, value);
            }
        }
        else {
            set<T>(field, from.get<T>(field));
        }
    }

    template<typename T>
    void copyRepeatedField(const ConfigFieldDescriptor* field, const ConfigObject& from) {
        const std::vector<T>& value =
            getField<std::vector<T> >(from, field);

        if (!value.empty()) {
            std::vector<T>* toValue = mutableRaw<std::vector<T> >(to, field);
            toValue->assign(value.begin(), value.end());
        }
    }

    template<typename T>
    void clearField(ConfigObject* object,
                    const ConfigFieldDescriptor* field) const {
        boost::optional<T>* raw =
            mutableRaw<boost::optional<T> >(object, field);

        if (raw) {
            boost::none_t null;
            (*raw) = null;
        }
    }

    template<typename T>
    void clearRepeatedField(ConfigObject* object,
                            const ConfigFieldDescriptor* field) const {
        std::vector<T>* raw = mutableRaw<std::vector<T> >(object, field);

        if (raw) {
            raw->clear();
        }
    }

private:
    static ObjectDescriptors* objectDescriptors_;

private:
    std::string name_;
    std::string className_;
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

template<> inline
void ConfigObject::copyRepeatedField<ConfigObject*>(const ConfigObject& from,
        const ConfigFieldDescriptor* field,
        ConfigObject* to) const {
    std::vector<ConfigObject*> values =
        getField<std::vector<ConfigObject*> >(from, field);

    for (std::size_t i = 0; i < values.size(); ++i) {
        ConfigObject* toValue = addObject(to, field);
        toValue->copyFrom(*values[i]);
    }
}

template<> inline
void ConfigObject::clearRepeatedField<ConfigObject*>(ConfigObject* object,
        const ConfigFieldDescriptor* field) const {

    std::vector<ConfigObject*>* raw = mutableRaw<std::vector<ConfigObject*> >(object, field);

    if (raw) {
        for (std::size_t i = 0; i < raw->size(); ++i) {
            delete raw->at(i);
        }

        raw->clear();
    }
}

template<> inline
void ConfigObject::clearField<std::string>(ConfigObject* object,
        const ConfigFieldDescriptor* field) const {
    std::string* str = mutableRaw<std::string>(object, field);

    if (str) {
        str->clear();
    }
}

template<> inline
void ConfigObject::clearField<ConfigObject*>(ConfigObject* object,
        const ConfigFieldDescriptor* field) const {
    ConfigObject** obj = mutableRaw<ConfigObject*>(object, field);

    if (obj && *obj) {
        (*obj)->clear();
    }
}

}
}

#endif //#if !defined(CETTY_CONFIG_CONFIGOBJECT_H)

// Local Variables:
// mode: c++
// End:
