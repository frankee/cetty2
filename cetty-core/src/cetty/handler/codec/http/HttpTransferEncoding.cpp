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

#include <cetty/handler/codec/http/HttpTransferEncoding.h>

namespace cetty {
namespace handler {
namespace codec {
namespace http {

const HttpTransferEncoding HttpTransferEncoding::CHUNKED(0, false);
const HttpTransferEncoding HttpTransferEncoding::STREAMED(1, false);
const HttpTransferEncoding HttpTransferEncoding::SINGLE(2, true);

static const std::string CHUNKED_STR = "CHUNKED";
static const std::string STREAMED_STR = "STREAMED";
static const std::string SINGLE_STR = "SINGLE";
static const std::string UNKNOWN_TYPE = "UNKNOWN_TYPE";

std::string HttpTransferEncoding::toString() const {
    switch (v) {
    case 0: return CHUNKED_STR;
    case 1: return STREAMED_STR;
    case 2: return SINGLE_STR;
    default: return UNKNOWN_TYPE;
    }
}

}
}
}
}
