/**
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

#include <cetty/util/SimpleString.h>

namespace cetty {
namespace util {

cetty::util::SimpleString SimpleString::substr(int start, int end) const {
    if (start < end) {
        return SimpleString(data + start, end - start);
    }

    return SimpleString();
}

cetty::util::SimpleString SimpleString::trim() const {
    char* newData = this->data;
    int   newSize = this->size;

    while ((*newData == ' ' || *newData == '\t') && newSize > 0) {
        newData++;
        --newData;
    }

    while ((newData[newSize - 1] == ' ' || newData[newSize - 1] == '\t')
            && newSize > 0) {
        --newSize;
    }

    newData[newSize] = '\0';
    return SimpleString(newData, newSize);
}

bool SimpleString::iequals(const std::string& str) const {
    BOOST_ASSERT(false && "HAS NOT IMPLEMENT.");
    return false;
}

}
}
