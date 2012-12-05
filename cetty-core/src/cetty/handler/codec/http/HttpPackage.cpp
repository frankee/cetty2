
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

#include <cetty/handler/codec/http/HttpPackage.h>

namespace cetty {
namespace handler {
namespace codec {
namespace http {

class HttpPackageEmptyVisitor : public boost::static_visitor<bool> {
public:
    bool operator()(const HttpRequestPtr& value) const { return !!value; }
    bool operator()(const HttpResponsePtr& value) const { return !!value; }
    bool operator()(const HttpChunkPtr& value) const { return !!value; }
    bool operator()(const HttpChunkTrailerPtr& value) const { return !!value; }

    template <typename T>
    bool operator()(const T& t) const { return false; }
};

HttpPackage::operator bool() const {
    static HttpPackageEmptyVisitor visitor;
    return variant.apply_visitor(visitor);
}

}
}
}
}
