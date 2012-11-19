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

#include <cetty/craft/http/HttpRequest2ProtobufMessage.h>

#include <cstdlib>
#include <boost/assert.hpp>
#include <google/protobuf/message.h>
#include <google/protobuf/descriptor.h>

#include <cetty/util/StringUtil.h>
#include <cetty/logging/LoggerHelper.h>

#include <cetty/handler/codec/http/HttpRequest.h>

#include <cetty/protobuf/service/ProtobufServiceRegister.h>
#include <cetty/protobuf/service/ProtobufServiceMessage.h>

#include <cetty/protobuf/serialization/ProtobufParser.h>

#include <cetty/craft/http/ServiceRequestMapper.h>

namespace cetty {
namespace craft {
namespace http {

using namespace google::protobuf;
using namespace cetty::util;
using namespace cetty::protobuf::service;
using namespace cetty::protobuf::serialization;

HttpRequest2ProtobufMessage::HttpRequest2ProtobufMessage() {

}

HttpRequest2ProtobufMessage::HttpRequest2ProtobufMessage(
    const ServiceRequestMapperPtr& serviceTemplates)
    : templates(serviceTemplates) {

}

void HttpRequest2ProtobufMessage::setRequestMapper(
    const ServiceRequestMapperPtr& requestMapper) {
    this->templates = requestMapper;
}

ProtobufServiceMessagePtr HttpRequest2ProtobufMessage::getProtobufMessage(
    const HttpRequestPtr& request) {
    HttpRequestTemplate* tmpl = templates->match(request->getMethod(), request->getPathSegments());
    fieldNameScope.clear();

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
                // deprecated
                const HttpRequestTemplate::Parameter* p = tmpl->getParameter("format");
                if (p) {
                    const std::vector<std::string>& segments = request->getPathSegments();
                    request->setLabel(segments[p->index]);
                }

                if (request->getQueryParameters().has("req")) {
                    ProtobufParser* jsonParser = ProtobufParser::getParser("json");

                    if (jsonParser) {
                        const std::string& reqStr = request->getQueryParameters().get("req");

                        if (req && !jsonParser->parse(reqStr, req)) {
                            ProtobufServiceMessagePtr message(
                                new ProtobufServiceMessage(ProtobufServiceMessage::T_REQUEST,
                                                           tmpl->getService(),
                                                           tmpl->getMethod()));
                            message->setPayload(req);
                            return message;
                        }
                    }
                }

                LOG_ERROR << "can not parse the message: "
                          << prototype->GetDescriptor()->full_name();
            }

            delete req;
        }
        else {
            LOG_ERROR << "has no such service register: "
                      << tmpl->getService() << ":" << tmpl->getMethod();
        }
    }
    else {
        LOG_ERROR << "wrong uri format, can not match any template."
                  << request->getUriString();
    }

    return ProtobufServiceMessagePtr();
}

bool HttpRequest2ProtobufMessage::parseMessage(const HttpRequestTemplate& tmpl,
        const HttpRequestPtr& request,
        Message* message) {
    BOOST_ASSERT(message && "Message should not be NULL.");

    const google::protobuf::Reflection* reflection = message->GetReflection();
    const google::protobuf::Descriptor* descriptor = message->GetDescriptor();
    BOOST_ASSERT(reflection && descriptor && "reflection or descriptor should not be NULL.");

    bool parsed = false;
    int fieldCnt = descriptor->field_count();
    std::string fieldName;

    for (int i = 0; i < fieldCnt; ++i) {
        const google::protobuf::FieldDescriptor* field = descriptor->field(i);
        fieldName = fieldNameScope + field->name();

        if (tmpl.hasField(fieldName)) {
            if (!parseField(tmpl, request, field, fieldName, message)) {
                LOG_ERROR << "parse filed: " << fieldName << " error.";
                return false;
            }
            else {
                parsed = true;
            }
        }
    }

    // remove this message name in field name scope.
    if (!fieldNameScope.empty()) {
        std::string::size_type size = fieldNameScope.size();

        if (fieldNameScope[size - 1] == '.' && size > 1) {
            std::string::size_type pos = fieldNameScope.rfind('.', size - 2);

            if (pos == fieldNameScope.npos) {
                fieldNameScope.clear();
            }
            else {
                fieldNameScope.erase(pos + 1, size - pos - 1);
            }
        }
        else {
            LOG_ERROR << "the fieldNameScope ("
                      << fieldNameScope
                      << ") not end with '.', should NOT happened";
            return false;
        }
    }

    return parsed;
}

static bool getValue(const HttpRequestPtr& request,
                     const HttpRequestTemplate::Parameter& parameter,
                     std::vector<std::string>* values) {
    if (NULL == values) {
        return false;
    }

    if (parameter.isInPath()) {
        const std::vector<std::string>& pathSegments
            = request->getPathSegments();

        values->push_back(pathSegments[parameter.index]);
        return true;
    }
    else if (parameter.isInQuery()) {
        const NameValueCollection& nameValues = request->getQueryParameters();
        return nameValues.get(parameter.name, values) > 0;
    }
    else if (parameter.isInCookie()) {
        //if (parameter.index >= 0) { // like: cookie=value; or cookie={v:a}{}
        //    return true;
        //}

        return false;//TODO
    }

    return false;
}

bool HttpRequest2ProtobufMessage::parseField(const HttpRequestTemplate& tmpl,
        const HttpRequestPtr& request,
        const FieldDescriptor* field,
        const std::string& fieldName,
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

        // append the father filed name to name scope.
        fieldNameScope += field->name();
        fieldNameScope += ".";

        return parseMessage(tmpl, request, msg);
    }

    const HttpRequestTemplate::Parameter* parameter = tmpl.getParameter(fieldName);
    std::vector<std::string> values;

    if (NULL == parameter || !getValue(request, *parameter, &values)) {
        return false;
    }

    if (field->is_repeated()) {
        std::vector<std::string>::const_iterator itr;

        for (itr = values.begin(); itr != values.end(); ++itr) {
            const std::string& value = *itr;

            switch (field->cpp_type()) {
            case google::protobuf::FieldDescriptor::CPPTYPE_BOOL:
                if (StringUtil::iequals(value.c_str(), "true")
                    || StringUtil::iequals(value.c_str(), "1")
                    || StringUtil::iequals(value.c_str(), "yes")
                    || StringUtil::iequals(value.c_str(), "y")) {
                        reflection->AddBool(message, field, true);
                }
                else {
                    reflection->AddBool(message, field, false);
                }

                break;
            case google::protobuf::FieldDescriptor::CPPTYPE_INT32:
                reflection->AddInt32(message, field, StringUtil::strto32(value));
                break;

            case google::protobuf::FieldDescriptor::CPPTYPE_INT64:
                reflection->AddInt64(message, field, StringUtil::strto64(value));
                break;

            case  google::protobuf::FieldDescriptor::CPPTYPE_DOUBLE:
                reflection->AddDouble(message, field, StringUtil::strtof(value));
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
        if (values.size() == 1) {
            const std::string& value = values.front();

            switch (field->cpp_type()) {
            case google::protobuf::FieldDescriptor::CPPTYPE_BOOL:
                if (StringUtil::iequals(value.c_str(), "true")
                    || StringUtil::iequals(value.c_str(), "1")
                    || StringUtil::iequals(value.c_str(), "yes")
                    || StringUtil::iequals(value.c_str(), "y")) {
                        reflection->AddBool(message, field, true);
                }
                else {
                    reflection->AddBool(message, field, false);
                }
                break;
            case google::protobuf::FieldDescriptor::CPPTYPE_INT32:
                reflection->SetInt32(message, field, StringUtil::strto32(value));
                break;

            case google::protobuf::FieldDescriptor::CPPTYPE_INT64:
                reflection->SetInt64(message, field, StringUtil::strto64(value));
                break;

            case  google::protobuf::FieldDescriptor::CPPTYPE_DOUBLE:
                reflection->SetDouble(message, field, StringUtil::strtof(value));
                break;

            case google::protobuf::FieldDescriptor::CPPTYPE_STRING:
                reflection->SetString(message, field, value);
                break;

            default:
                return false;
            }
        }
        else {
            // a.b1, a.b2, a.c, a.d
            // if has tow 'b'(b1&b2), but the field of b is not repeated,
            // so, when a is repeated, then will create a twice, or a.b2 will be skip.

        }
    }

    return true;
}

}
}
}
