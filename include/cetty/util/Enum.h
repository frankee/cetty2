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

namespace cetty {
namespace util {

template<typename SubT>
class Enum {
public:
    Enum(const Enum& rhs) : value_(rhs.value_) {}
    Enum& operator=(const Enum& rhs) { value_ = rhs.value_; return *this; }

public:
    int value() const {
        return value_;
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
        : value_(i) {
    }

private:
    int value_;
};

}
}

#endif //#if !defined(CETTY_UTIL_ENUM_H)

// Local Variables:
// mode: c++
// End:
