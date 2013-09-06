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

#include <cetty/util/Uuid.h>

#include <sstream>
#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <cetty/Common.h>
#include <cetty/logging/LoggerHelper.h>

namespace cetty {
namespace util {

using namespace boost::uuids;

boost::uuids::string_generator Uuid::sgen_;
boost::uuids::random_generator Uuid::rgen_;

Uuid::Uuid(const char* str) {
    if (str) {
        try {
            uuid u(sgen_(str));
            uuid::swap(u);
        }
        catch (const std::runtime_error& e) {
            uuid u(nil_uuid());
            uuid::swap(u);
            LOG_ERROR << "failed to construct the Uuid from "
                      << e.what() << " : "
                      << str;
        }
    }
    else {
        LOG_WARN << "failed to construct the Uuid from a NULL str";
    }
}

Uuid::Uuid(const std::string& str) {
    try {
        uuid u(sgen_(str));
        uuid::swap(u);
    }
    catch (const std::runtime_error& e) {
        uuid u(nil_uuid());
        uuid::swap(u);
        LOG_ERROR << "failed to construct the Uuid from "
                  << e.what() << " : "
                  << str;
    }
}

std::string Uuid::toString() const {
    //     std::stringstream ss;
    //     ss << static_cast<const uuid&>(*this);

    return boost::lexical_cast<std::string>(*this);
}

std::vector<char> Uuid::toBytes() const {
    std::vector<char> v(uuid::size());
    std::copy(uuid::begin(), uuid::end(), v.begin());
    return v;
}

void Uuid::extractBytes(std::vector<char>* bytes) {
    if (bytes) {
        bytes->resize(uuid::size());
        std::copy(uuid::begin(), uuid::end(), bytes->begin());
    }
}

}
}
