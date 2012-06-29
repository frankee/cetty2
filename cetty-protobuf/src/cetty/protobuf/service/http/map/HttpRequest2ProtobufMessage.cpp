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
#include <google/protobuf/descriptor.h>

#include <cetty/util/StringUtil.h>
#include <cetty/protobuf/service/ProtobufServiceRegister.h>
#include <cetty/protobuf/service/ProtobufServiceMessage.h>

namespace cetty {
namespace protobuf {
namespace service {
namespace http {
namespace map {

using namespace google::protobuf;
using namespace cetty::util;
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

            if (req && parseMessage(*tmpl, request, req)) {
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

bool HttpRequest2ProtobufMessage::parseMessage(const HttpServiceTemplate& tmpl,
        const HttpRequestPtr& request,
        Message* message) {
    BOOST_ASSERT(message && "Message should not be NULL.");
    const google::protobuf::Reflection* reflection = message->GetReflection();
    const google::protobuf::Descriptor* descriptor = message->GetDescriptor();

    int fieldCnt = descriptor->field_count();

    for (int i = 0; i < fieldCnt; ++i) {
        const google::protobuf::FieldDescriptor* field = descriptor->field(i);

        if (tmpl.hasField(field->full_name())) {
            if (!parseField(tmpl, request, field, message)) {
                return false;
            }
        }
    }

    return true;
}

static int getValue(const HttpRequestPtr& request,
                         const HttpServiceTemplate::Parameter& parameter,
                         NameValueCollection::ConstIterator* begin,
                         NameValueCollection::ConstIterator* end) {
    if (parameter.isInPath()) {
        return true;
    }
    else if (parameter.isInQuery()) {
        const NameValueCollection& nameValues = request->getQueryParameters();
        if (begin && end) {
            *begin = nameValues.lowerBound(parameter.name);
            *end = nameValues.upperBound(parameter.name);

            return *begin != nameValues.end();
        }
        else {
            return nameValues.has(parameter.name);
        }
    }
    else if (parameter.isInCookie()) {
        if (parameter.index >= 0) { // like: cookie=value; or cookie={v:a}{}
            return true;
        }

        return false;//TODO
    }

    return false;
}

bool HttpRequest2ProtobufMessage::parseField(const HttpServiceTemplate& tmpl,
        const HttpRequestPtr& request,
        const FieldDescriptor* field,
        Message* message) {
    const google::protobuf::Reflection* reflection = message->GetReflection();

    int fieldType = field->type();

    if (field->is_extension()) {
        //TODO
    }
    else if (fieldType == google::protobuf::FieldDescriptor::TYPE_GROUP) {
        // Groups must be serialized with their original capitalization.
    }
    else if (fieldType == google::protobuf::FieldDescriptor::TYPE_MESSAGE) {
        google::protobuf::Message* msg
            = field->is_repeated() ? reflection->AddMessage(message, field)
                                   : reflection->MutableMessage(message, field);
        return parseMessage(tmpl, request, msg);
    }

    const HttpServiceTemplate::Parameter* parameter = tmpl.getParameter(field->full_name());
    NameValueCollection::ConstIterator beginParam;
    NameValueCollection::ConstIterator endParam;

    if (NULL == parameter
            || getValue(request, *parameter, &beginParam, &endParam) == 0) {
        return false;
    }

    if (field->is_repeated()) {
        NameValueCollection::ConstIterator itr;
        for (itr = beginParam; itr != endParam; ++itr) {
            const std::string& value = itr->second;
            switch (field->cpp_type()) {
            case google::protobuf::FieldDescriptor::CPPTYPE_INT32:
                reflection->AddInt32(message, field, std::atoi(value));
                break;

            case google::protobuf::FieldDescriptor::CPPTYPE_INT64:
                reflection->AddInt64(message, field, std::atoi(value));
                break;

            case  google::protobuf::FieldDescriptor::CPPTYPE_DOUBLE:
                reflection->AddDouble(message, field, StringUtil::atof(value));
                break;

            case google::protobuf::FieldDescriptor::CPPTYPE_STRING:
                reflection->AddString(message, field, value);
                break;

            default:
                return false;
            }
        }
    }
    else {
        const std::string& value = beginParam->second;

        switch (field->cpp_type()) {
        case google::protobuf::FieldDescriptor::CPPTYPE_INT32:
            reflection->SetInt32(message, field, StringUtil::atoi(value));
            break;

        case google::protobuf::FieldDescriptor::CPPTYPE_INT64:
            reflection->SetInt64(message, field, StringUtil::atoi(value));
            break;

        case  google::protobuf::FieldDescriptor::CPPTYPE_DOUBLE:
            reflection->SetDouble(message, field, StringUtil::atof(value));
            break;

        case google::protobuf::FieldDescriptor::CPPTYPE_STRING:
            reflection->SetString(message, field, value);
            break;

        default:
            return false;
        }
    }

    return true;
}

}
}
}
}
}