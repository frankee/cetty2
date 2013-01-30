#if !defined(CETTY_CONFIG_CONFIGREFLECTION_H)
#define CETTY_CONFIG_CONFIGREFLECTION_H

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

#include <string>
#include <vector>
#include <boost/none_t.hpp>
#include <boost/optional.hpp>
#include <boost/noncopyable.hpp>
#include <cetty/Types.h>
#include <cetty/config/ConfigDescriptor.h>

namespace cetty {
namespace config {

class ConfigObject;
class ConfigFieldDescriptor;

class ConfigReflection : private boost::noncopyable {
public:
    ConfigReflection();

    void listFields(const ConfigObject& object,
                    std::vector<const ConfigFieldDescriptor*>* output) const;

    boost::optional<int> getInt(const ConfigObject& object,
                                const ConfigFieldDescriptor* field) const;

    boost::optional<int64_t> getInt64(const ConfigObject& object,
                                      const ConfigFieldDescriptor* field) const;

    boost::optional<double> getDouble(const ConfigObject& object,
                                      const ConfigFieldDescriptor* field) const;

    boost::optional<bool> getBool(const ConfigObject& object,
                                  const ConfigFieldDescriptor* field) const;

    std::string getString(const ConfigObject& object,
                          const ConfigFieldDescriptor* field) const;

    void setInt32(ConfigObject* object,
                  const ConfigFieldDescriptor* field,
                  int value) const;

    void setInt64(ConfigObject* object,
                  const ConfigFieldDescriptor* field,
                  int64_t value) const;

    void setDouble(ConfigObject* object,
                   const ConfigFieldDescriptor* field,
                   double value) const;

    void setBool(ConfigObject* object,
                 const ConfigFieldDescriptor* field,
                 bool value) const;

    void setString(ConfigObject* object,
                   const ConfigFieldDescriptor* field,
                   const std::string& value) const;

    ConfigObject* mutableObject(ConfigObject* object,
                                const ConfigFieldDescriptor* field) const;

    void addInt32(ConfigObject* object,
                  const ConfigFieldDescriptor* field,
                  int value) const;

    void addInt32(ConfigObject* object,
                  const ConfigFieldDescriptor* field,
                  const std::vector<int>& value) const;

    void addInt64(ConfigObject* object,
                  const ConfigFieldDescriptor* field,
                  int64_t value) const;

    void addInt64(ConfigObject* object,
                  const ConfigFieldDescriptor* field,
                  const std::vector<int64_t>& value) const;

    void addDouble(ConfigObject* object,
                   const ConfigFieldDescriptor* field,
                   double value) const;

    void addDouble(ConfigObject* object,
                   const ConfigFieldDescriptor* field,
                   const std::vector<double>& value) const;

    void addBool(ConfigObject* object,
                 const ConfigFieldDescriptor* field,
                 bool value) const;

    void addBool(ConfigObject* object,
                 const ConfigFieldDescriptor* field,
                 const std::vector<bool>& value) const;

    void addString(ConfigObject* object,
                   const ConfigFieldDescriptor* field,
                   const std::string& value) const;

    void addString(ConfigObject* object,
                   const ConfigFieldDescriptor* field,
                   const std::vector<std::string>& value) const;

    ConfigObject* addObject(ConfigObject* object,
                            const ConfigFieldDescriptor* field) const;



private:
    template <typename Type>
    inline const Type& getField(const ConfigObject& object,
                                const ConfigFieldDescriptor* field) const {
        return getRaw<Type>(object, field);
    }

    template <typename Type>
    inline void setField(ConfigObject* object,
                         const ConfigFieldDescriptor* field,
                         const Type& value) const {
        *mutableRaw<Type>(object, field) = value;
    }

    template <typename Type>
    inline Type* mutableRaw(ConfigObject* object,
                            const ConfigFieldDescriptor* field) const {
        void* ptr = reinterpret_cast<uint8_t*>(object) + field->offset;
        return reinterpret_cast<Type*>(ptr);
    }

    template <typename Type>
    inline const Type& getRaw(const ConfigObject& object,
                              const ConfigFieldDescriptor* field) const {
        const void* ptr = reinterpret_cast<const uint8_t*>(&object) +
                          field->offset;
        return *reinterpret_cast<const Type*>(ptr);
    }

    template <typename Type>
    inline void addField(ConfigObject* object,
                         const ConfigFieldDescriptor* field,
                         const Type& value) const {
        mutableRaw<std::vector<Type> >(object, field)->push_back(value);
    }

    template<typename Type>
    inline void addFields(ConfigObject* object,
                          const ConfigFieldDescriptor* field,
                          const std::vector<Type>& value) const {
        std::vector<Type>* to =
            mutableRaw<std::vector<Type> >(object, field);

        if (to) {
            to->insert(to->end(), value.begin(), value.end());
        }
    }
};

inline
boost::optional<int> ConfigReflection::getInt(const ConfigObject& object,
        const ConfigFieldDescriptor* field) const {
    return getField<boost::optional<int> >(object, field);
}

inline
boost::optional<int64_t> ConfigReflection::getInt64(const ConfigObject& object,
        const ConfigFieldDescriptor* field) const {
    return getField<boost::optional<int64_t> >(object, field);
}

inline
boost::optional<double> ConfigReflection::getDouble(const ConfigObject& object,
        const ConfigFieldDescriptor* field) const {
    return getField<boost::optional<double> >(object, field);
}

inline
boost::optional<bool> ConfigReflection::getBool(const ConfigObject& object,
        const ConfigFieldDescriptor* field) const {
    return getField<boost::optional<bool> >(object, field);
}

inline
std::string ConfigReflection::getString(const ConfigObject& object,
                                        const ConfigFieldDescriptor* field) const {
    return getField<std::string>(object, field);
}

inline
void ConfigReflection::setInt32(ConfigObject* object,
                                const ConfigFieldDescriptor* field,
                                int value) const {
    setField<boost::optional<int> >(object, field, value);
}

inline
void ConfigReflection::setInt64(ConfigObject* object,
                                const ConfigFieldDescriptor* field,
                                int64_t value) const {
    setField<boost::optional<int64_t> >(object, field, value);
}

inline
void ConfigReflection::setDouble(ConfigObject* object,
                                 const ConfigFieldDescriptor* field,
                                 double value) const {
    setField<boost::optional<double> >(object, field, value);
}

inline
void ConfigReflection::setBool(ConfigObject* object,
                               const ConfigFieldDescriptor* field,
                               bool value) const {
    setField<boost::optional<bool> >(object, field, value);
}

inline
void ConfigReflection::setString(ConfigObject* object,
                                 const ConfigFieldDescriptor* field,
                                 const std::string& value) const {
    setField<std::string>(object, field, value);
}

inline
void ConfigReflection::addInt32(ConfigObject* object,
                                const ConfigFieldDescriptor* field,
                                int value) const {
    addField<int>(object, field, value);
}

inline
void ConfigReflection::addInt32(ConfigObject* object,
                                const ConfigFieldDescriptor* field,
                                const std::vector<int>& value) const {
    addFields<int>(object, field, value);
}

inline
void ConfigReflection::addInt64(ConfigObject* object,
                                const ConfigFieldDescriptor* field,
                                int64_t value) const {
    addField<int64_t>(object, field, value);
}

inline
void ConfigReflection::addInt64(ConfigObject* object,
                                const ConfigFieldDescriptor* field,
                                const std::vector<int64_t>& value) const {
    addFields<int64_t>(object, field, value);
}

inline
void ConfigReflection::addDouble(ConfigObject* object,
                                 const ConfigFieldDescriptor* field,
                                 double value) const {
    addField<double>(object, field, value);
}

inline
void ConfigReflection::addDouble(ConfigObject* object,
                                 const ConfigFieldDescriptor* field,
                                 const std::vector<double>& value) const {
    addFields<double>(object, field, value);
}

inline
void ConfigReflection::addBool(ConfigObject* object,
                               const ConfigFieldDescriptor* field,
                               bool value) const {
    addField<bool>(object, field, value);
}

inline
void ConfigReflection::addBool(ConfigObject* object,
                               const ConfigFieldDescriptor* field,
                               const std::vector<bool>& value) const {
    addFields<bool>(object, field, value);
}

inline
void ConfigReflection::addString(ConfigObject* object,
                                 const ConfigFieldDescriptor* field,
                                 const std::string& value) const {
    addField<std::string>(object, field, value);
}

inline
void ConfigReflection::addString(ConfigObject* object,
                                 const ConfigFieldDescriptor* field,
                                 const std::vector<std::string>& value) const {
    addFields<std::string>(object, field, value);
}

}
}

#endif //#if !defined(CETTY_CONFIG_CONFIGREFLECTION_H)

// Local Variables:
// mode: c++
// End:
