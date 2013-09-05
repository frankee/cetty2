#if !defined(CETTY_UTIL_UUID_H)
#define CETTY_UTIL_UUID_H

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
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>

namespace cetty {
namespace util {

using namespace boost::uuids;

class Uuid : public boost::uuids::uuid {
public:
    /**
     * construct a random uuid
     */
    Uuid();

    /**
     * construct a nil uuid
     */
    explicit Uuid(int);

    /**
     * construct a uuid from a string
     * including types:
     * 1. "01234567-89ab-cdef-0123456789abcdef"
     * 2. "0123456789abcdef0123456789abcdef"
     * 3. "{01234567-89ab-cdef-0123456789abcdef}"
     * 4. "{0123456789abcdef0123456789abcdef}"
     * others will be nil uuid, no exception thrown.
     */
    explicit Uuid(const char* str);

    /**
     * construct a uuid from a string
     * including types:
     * 1. "01234567-89ab-cdef-0123456789abcdef"
     * 2. "0123456789abcdef0123456789abcdef"
     * 3. "{01234567-89ab-cdef-0123456789abcdef}"
     * 4. "{0123456789abcdef0123456789abcdef}"
     * others will be nil uuid, no exception thrown.
     */
    explicit Uuid(const std::string& str);

    /**
     * generates a name based uuid from a namespace uuid and a name
     */
    explicit Uuid(const uuid& u, const char* name);

    /**
     * generates a name based uuid from a namespace uuid and a name
     */
    explicit Uuid(const uuid& u, const std::string& name);

    /**
     * generates a name based uuid from a namespace uuid and a name
     */
    explicit Uuid(const Uuid& u, const char* name);

    /**
     * generates a name based uuid from a namespace uuid and a name
     */
    explicit Uuid(const Uuid& u, const std::string& name);

    /**
     *
     */
    explicit Uuid(const uuid& u);

    /**
     *
     */
    explicit Uuid(const Uuid& u);

    /**
     *
     */
    operator uuid();

    /**
     *
     */
    operator uuid() const;

    /**
     * extract the 16 bytes of a uuid directly.
     * @param bytes
     */
    void extractBytes(std::vector<char>* bytes);

    /**
     *  return the 16 bytes of a uuid directly.
     */
    std::vector<char> toBytes() const;

    /**
     * return the representation of a uuid, a string of hexadecimal digits of 
     * the following form: hhhhhhhh-hhhh-hhhh-hhhh-hhhhhhhhhhhh
     */
    std::string toString() const;

private:
    static random_generator rgen_;
    static string_generator sgen_;
};

inline
Uuid::Uuid()
    : uuid(rgen_()) {
}

inline
Uuid::Uuid(int)
    : uuid(nil_uuid()) {
}

inline
Uuid::Uuid(const uuid& u, const char* name)
    : uuid(name_generator(u)(name)) {
}

inline
Uuid::Uuid(const uuid& u, const std::string& name)
    : uuid(name_generator(u)(name)) {
}

inline
Uuid::Uuid(const Uuid& u, const char* name)
    : uuid(name_generator(u)(name)) {
}

inline
Uuid::Uuid(const Uuid& u, const std::string& name)
    : uuid(name_generator(u)(name)) {
}

inline
Uuid::Uuid(const uuid& u)
    : uuid(u) {
}

inline
Uuid::Uuid(const Uuid& u)
    : uuid(static_cast<const uuid&>(u)) {
}

inline
Uuid::operator uuid() {
    return static_cast<uuid&>(*this);
}

inline
Uuid::operator uuid() const {
    return static_cast<const uuid&>(*this);
}

}
}


#endif //#if !defined(CETTY_UTIL_UUID_H)