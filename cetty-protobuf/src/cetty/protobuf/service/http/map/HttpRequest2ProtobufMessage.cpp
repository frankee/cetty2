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

#include <cetty/protobuf/service/http/map/HttpRequest2ProtobufMessage.h>

#include <boost/assert.hpp>
#include <google/protobuf/message.h>
#include <cetty/protobuf/service/ProtobufServiceRegister.h>
#include <cetty/protobuf/service/ProtobufServiceMessage.h>

namespace cetty {
namespace protobuf {
namespace service {
namespace http {
namespace map {

using namespace google::protobuf;
using namespace cetty::protobuf::service;

ProtobufServiceMessagePtr HttpRequest2ProtobufMessage::getProtobufMessage(
    const HttpRequestPtr& request) {
    HttpServiceTemplate* tmpl = templates->match(request->getMethod(), request->getPathSegments());

    if (tmpl) {
        const Message* prototype =
            ProtobufServiceRegister::instance().getRequestPrototype(
                tmpl->getService(),
                tmpl->getMethod());

        if (prototype) {
            Message* req = prototype->New();

            if (req && !parse(*tmpl, request, req)) {
                ProtobufServiceMessagePtr message(
                    new ProtobufServiceMessage(ProtobufServiceMessage::T_REQUEST,
                                               tmpl->getService(),
                                               tmpl->getMethod()));
                message->setPayload(req);
                return message;
            }
            else {
                printf("can not parse the message.");
            }
        }
        else {
            printf("has no such service register.");
        }
    }
    else {
        printf("wrong uri format.");
    }
}

int HttpRequest2ProtobufMessage::parse(const HttpServiceTemplate& tmpl,
    const HttpRequestPtr& request,
    Message* message) {
    BOOST_ASSERT(message);

    return -1;

}

}
}
}
}
}