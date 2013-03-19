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
#include <boost/assert.hpp>
#include <boost/optional.hpp>
#include <boost/static_assert.hpp>

#include <cetty/Types.h>
#include <cetty/util/MetaProgramming.h>

namespace cetty {
namespace config {

using namespace cetty::util;

class ConfigFieldDescriptor;
class ConfigObjectDescriptor;

class ConfigObject {
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

    //     int getInt(const ConfigFieldDescriptor* field) const;
    //     int64_t getInt64(const ConfigFieldDescriptor* field) const;
    //     double getDouble(const ConfigFieldDescriptor* field) const;
    //     bool getBool(const ConfigFieldDescriptor* field) const;
    //     const std::string& getString(const ConfigFieldDescriptor* field) const;

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

    ConfigObject* addObject(const ConfigFieldDescriptor* field);
    ConfigObject* addObject(const ConfigFieldDescriptor* field,
                            const std::string& key);

public:
    static ObjectDescriptors& objectDescriptors();

    /**
     * register the config object descriptor.
     */
    static void addDescriptor(ConfigObjectDescriptor* descriptor);

    /**
     *
     */
    static const ConfigObject* defaultObject(const std::string& name);

private:
    template <typename T>
    inline T* mutableRaw(const ConfigFieldDescriptor* field) {
        BOOST_ASSERT(field);
        void* ptr = reinterpret_cast<uint8_t*>(this) + field->offset;
        return reinterpret_cast<T*>(ptr);
    }

    template <typename T>
    inline const T* constRaw(const ConfigFieldDescriptor* field) const {
        BOOST_ASSERT(field);
        const void* ptr = reinterpret_cast<const uint8_t*>(this) +
                          field->offset;
        return reinterpret_cast<const T*>(ptr);
    }

    template<typename T>
    void copyField(const ConfigFieldDescriptor* field, const ConfigObject& from) {
        BOOST_ASSERT(field);

        if (field->optional) {
            const boost::optional<T>& value =
                from.get<boost::optional<T> >(field);

            if (value) {
                set<boost::optional<T> >(field, value);
            }
        }
        else {
            set<T>(field, from.get<T>(field));
        }
    }

    template<typename T>
    void copyRepeatedField(const ConfigFieldDescriptor* field,
                           const ConfigObject& from) {
        const T& value = from.get<T>(field);

        if (!value.empty()) {
            *mutableRaw<T>(field) = value;
        }
    }

    template<typename T>
    void clearField(const ConfigFieldDescriptor* field) {
        if (field->optional) {
            *mutableRaw<boost::optional<T> >(field) = boost::none_t();
        }
    }

    template<typename T>
    void clearRepeatedField(const ConfigFieldDescriptor* field) {
        mutableRaw<T>(field)->clear();
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
void ConfigObject::copyRepeatedField<std::vector<ConfigObject*> >(
    const ConfigFieldDescriptor* field,
    const ConfigObject& from) {
    const std::vector<ConfigObject*>& values =
        from.get<std::vector<ConfigObject*> >(field);

    for (std::size_t i = 0; i < values.size(); ++i) {
        ConfigObject* toValue = addObject(field);
        toValue->copyFrom(*values[i]);
    }
}

template<> inline
void ConfigObject::copyRepeatedField<std::map<std::string, ConfigObject*> >(
    const ConfigFieldDescriptor* field,
    const ConfigObject& from) {
    const std::map<std::string, ConfigObject*>& values =
        from.get<std::map<std::string, ConfigObject*> >(field);

    std::map<std::string, ConfigObject*>::const_iterator itr;

    for (itr = values.begin(); itr != values.end(); ++itr) {
        ConfigObject* toValue = addObject(field, itr->first);
        toValue->copyFrom(*itr->second);
    }
}

template<> inline
void ConfigObject::clearField<std::string>(const ConfigFieldDescriptor* field) {
    mutableRaw<std::string>(field)->clear();
}

template<> inline
void ConfigObject::clearField<ConfigObject*>(const ConfigFieldDescriptor* field) {
    ConfigObject** obj = mutableRaw<ConfigObject*>(field);

    if (obj && *obj) {
        delete(*obj);
        *obj = NULL;
    }
}

template<> inline
void ConfigObject::clearRepeatedField<std::vector<ConfigObject*> >(
    const ConfigFieldDescriptor* field) {
    std::vector<ConfigObject*>* raw =
        mutableRaw<std::vector<ConfigObject*> >(field);

    if (raw) {
        for (std::size_t i = 0; i < raw->size(); ++i) {
            delete raw->at(i);
        }

        raw->clear();
    }
}

template<> inline
void ConfigObject::clearRepeatedField<std::map<std::string, ConfigObject*> >(
    const ConfigFieldDescriptor* field) {
    std::map<std::string, ConfigObject*>* raw =
        mutableRaw<std::map<std::string, ConfigObject*> >(field);
    BOOST_ASSERT(raw);
    std::map<std::string, ConfigObject*>::iterator itr = raw->begin();

    for (; itr != raw->end(); ++itr) {
        delete itr->second;
    }

    raw->clear();
}

}
}

#endif //#if !defined(CETTY_CONFIG_CONFIGOBJECT_H)

// Local Variables:
// mode: c++
// End:
