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

#include <cetty/protobuf/service/http/map/HttpServiceTemplate.h>

namespace cetty {
namespace protobuf {
namespace service {
namespace http {
namespace map {

HttpServiceTemplate::HttpServiceTemplate(const std::string& method,
        const std::string& uri,
        const std::string& cookie)
    : httpMethod(method), uriTemplate(uri), cookieTemplate(cookie) {


   const UriTemplate::PathSegments& pathSegments = uriTemplate.getPathSegments();
   std::size_t j = pathSegments.size();
   for (std::size_t i= 0; i < j; ++i) {
       const UriTemplate::PathSegment& segment = pathSegments[i];
       if (segment.type == UriTemplate::PATH_SEG_PARAM) {
           trie.addKey(segment.alias,
               Parameter::createFromPath(segment.name, segment.alias, i));
       }
       else if (segment.type == UriTemplate::PATH_SEG_ALIAS) {
           if (service.empty()) {
               service = segment.alias;
           }
           else if (this->method.empty()){
               this->method = segment.alias;
           }
       }
   }

   const UriTemplate::QueryParams& queryParams = uriTemplate.getQueryParams();
   UriTemplate::QueryParams::const_iterator itr = queryParams.begin();
   for (; itr != queryParams.end(); ++itr) {
       trie.addKey(itr->second,
           Parameter::createFromQuery(itr->first, itr->second));
   }
}

bool HttpServiceTemplate::validated() const {
    return !service.empty() && !method.empty();
}

}
}
}
}
}