#if !defined(CETTY_UTIL_ENUM_H)
#define CETTY_UTIL_ENUM_H

/*
 * Copyright (c) 2010-2011 frankee zhou (frankee.zhou at gmail dot com)
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
#include <boost/optional.hpp>
#include <cetty/Types.h>
#include <cetty/util/StringUtil.h>

namespace cetty {
namespace util {

template<class SubT, int CaseSensitive = 0>
class Enum {
public:
    Enum(const Enum& rhs)
        : value_(rhs.value_),
          name_(rhs.name_) {
    }

    Enum& operator=(const Enum& rhs) {
        value_ = rhs.value_;
        name_ = rhs.name_;
        return *this;
    }

public:
    /**
     * convert string to Enum type.
     *
     * To enable this function, you should:
     * 1. using Enum(int i, const char* name)
     * 2. set default enum, which will be return when failed to parse.
     *    if (!markSetDefaultEnum()) {
     *        setDefaultEnum(new MyEnum(-1, "unkonwn"));
     *    }
     *
     * CaseSensitive default to 0, means parse the name is case insensitive,
     * set to 1, make sure the parse will case sensitive.
     */
    static const SubT& parseFrom(const std::string& name);

public:
    int value() const {
        return value_;
    }

    const char* name() const {
        return name_;
    }

    friend bool operator==(const SubT& lhs, const SubT& rhs) {
        return lhs.value() == rhs.value();
    }

    friend bool operator!=(const SubT& lhs, const SubT& rhs) {
        return !(lhs == rhs);
    }

    friend bool operator < (const SubT& lhs, const SubT& rhs) {
        return lhs.value() < rhs.value();
    }

    friend bool operator <=(const SubT& lhs, const SubT& rhs) {
        return lhs.value() <= rhs.value();
    }

    friend bool operator > (const SubT& lhs, const SubT& rhs) {
        return lhs.value() > rhs.value();
    }

    friend bool operator >=(const SubT& lhs, const SubT& rhs) {
        return lhs.value() >= rhs.value();
    }

protected:
    Enum(int i)
        : value_(i),
          name_(NULL) {
    }

    Enum(int i, const char* name)
        : value_(i),
          name_(name) {
        if (!enums_) {
            enums_ = new Enums;
        }

        enums_->insert(std::make_pair(name, this));

        if (!CaseSensitive) {
            (*enums_)[removeUnderLine(name)] = this;
        }
    }

    // return true if mark successfully when the default enum has not been to be set
    bool markSetDefaultEnum() {
        if (!isSetting_) {
            isSetting_ = true;
            return true;
        }
        else {
            return false;
        }
    }

    const SubT* defaultEnum() const {
        return defaultEnum_;
    }

    void setDefaultEnum(const SubT* e) {
        if (defaultEnum_) {
            delete defaultEnum_;
        }

        defaultEnum_ = e;
    }

private:
    std::string removeUnderLine(const char* name) {
        std::string newName;

        for (std::size_t i = 0; name[i] != '\0'; ++i) {
            if (name[i] == '_') {
                continue;
            }

            newName.append(1, name[i]);
        }

        return newName;
    }

private:
    struct LessThan {
        bool operator()(const std::string& s1, const std::string& s2) const {
            if (CaseSensitive) {
                return s1 < s2;
            }
            else {
                return StringUtil::icompare(s1, s2) < 0;
            }
        }
    };

    typedef Enum<SubT, CaseSensitive> Self;
    typedef std::map<std::string, Self const*, LessThan> Enums;

    static Enums* enums_;
    static bool isSetting_;
    static const SubT* defaultEnum_;

private:
    int value_;
    const char* name_;
};

template<class SubT, int CaseSensitive>
typename Enum<SubT, CaseSensitive>::Enums* Enum<SubT, CaseSensitive>::enums_ = NULL;

template<class SubT, int CaseSensitive>
bool cetty::util::Enum<SubT, CaseSensitive>::isSetting_ = false;

template<class SubT, int CaseSensitive>
const SubT* cetty::util::Enum<SubT, CaseSensitive>::defaultEnum_ = NULL;

template<class SubT, int CaseSensitive> inline
const SubT& Enum<SubT, CaseSensitive>::parseFrom(const std::string& name) {
    BOOST_ASSERT(enums_ && defaultEnum_ && "This Enum does not support parseFrom");

    typename Enums::const_iterator itr = enums_->find(name);

    if (itr != enums_->end()) {
        return *reinterpret_cast<SubT const*>(itr->second);
    }

    return *defaultEnum_;
}

}
}

#endif //#if !defined(CETTY_UTIL_ENUM_H)

// Local Variables:
// mode: c++
// End:
