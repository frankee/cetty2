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

#include <cetty/craft/http/ServiceRequestMapping.h>

#include <boost/bind.hpp>
#include <boost/regex.hpp>
#include <boost/assert.hpp>
#include <google/protobuf/message.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/descriptor.pb.h>

#include <cetty/util/StringUtil.h>
#include <cetty/logging/LoggerHelper.h>

#include <cetty/handler/codec/http/HttpRequest.h>

#include <cetty/protobuf/service/ProtobufService.h>
#include <cetty/protobuf/service/ProtobufServiceMessage.h>
#include <cetty/protobuf/service/ProtobufServiceRegister.h>

#include <cetty/protobuf/serialization/ProtobufParser.h>

#include <cetty/craft/craft.pb.h>

namespace cetty {
namespace craft {
namespace http {

using namespace google::protobuf;
using namespace cetty::protobuf::service;
using namespace cetty::protobuf::serialization;

static boost::thread_specific_ptr<boost::smatch> regexResultPtr;

class ServiceMethod {
private:
    typedef std::map<std::string, int> PathParams;

public:
    ServiceMethod()
        : descriptor_(),
          httpMethod_("GET"),
          uriPath_() {
    }

    bool init(const std::string& path, const MethodDescriptor* descriptor) {
        const MethodOptions& options = descriptor->options();

        if (options.HasExtension(craft_method_options)) {
            const CraftMethodOptions& craftOptions =
                options.GetExtension(craft_method_options);

            path_ = path;
            path_ += craftOptions.path();
            httpMethod_ = HttpMethod::valueOf(craftOptions.method());

            descriptor_ = descriptor;
            service_ = descriptor_->service()->full_name();

            return initRegex();
        }
        else {
            return false;
        }
    }

    bool acceptedPost(const HttpRequestPtr& request) {
        const std::string& type = request->headers().headerValue(HttpHeaders::Names::CONTENT_TYPE);
        return request->method() == HttpMethod::POST &&
            (type == HttpHeaders::Values::X_PROTOBUFFER ||
            type == HttpHeaders::Values::X_WWW_FORM_URLENCODED);
    }

    bool match(const HttpRequestPtr& request) {
        if (request->method() != httpMethod_ && !acceptedPost(request)) {
            return false;
        }

        uriPath_ = &request->uri().getPath();

        if (regex_.empty()) {
            return uriPath_->compare(path_) == 0;
        }
        else {
            boost::smatch& result = getRegexResult();

            if (boost::regex_match(*uriPath_,
                                   result,
                                   regex_,
                                   boost::match_extra)) {
                return true;
            }
        }

        return false;
    }

    const std::string& serviceName() const {
        return service_;
    }
    const std::string& methodName() const {
        return descriptor_->name();
    }

    const std::string& fullMethodName() const {
        return descriptor_->full_name();
    }

    StringPiece pathParamValue(const std::string& name) const {
        PathParams::const_iterator itr = params_.find(name);

        if (itr != params_.end()) {
            const boost::smatch& result = getRegexResult();

            if (itr->second < static_cast<int>(result.size())) {
                return StringPiece(*uriPath_).substr(result.position(itr->second),
                                                     result.length(itr->second));
            }
        }

        return StringPiece();
    }

private:
    boost::smatch& getRegexResult() const {
        boost::smatch* result = regexResultPtr.get();

        if (!result) {
            result = new boost::smatch;
            regexResultPtr.reset(result);
        }

        return *result;
    }

    bool initRegex() {
        bool inBrace = false;
        int paramCount = 0;
        std::string param;
        std::string regexStr;
        std::size_t braceIndex = 0;

        for (std::size_t i = 0; i < path_.size(); ++i) {
            switch (path_[i]) {
            case '{':
                if (!inBrace) {
                    inBrace = true;
                }
                else {
                    return false;
                }

                braceIndex = i;
                break;

            case '}':
                if (inBrace) {
                    inBrace = false;
                }
                else {
                    return false;
                }

                ++paramCount;

                param = path_.substr(braceIndex + 1, i - braceIndex - 1);

                regexStr.append("(.*)");
                params_[param] = paramCount;
                break;

            case '.':
                regexStr.append("\\.");
                break;

            default:
                if (!inBrace) {
                    regexStr.append(1, path_[i]);
                }

                break;
            }
        }

        if (!regexStr.empty()) {
            regex_ = regexStr;
        }

        return true;
    }

private:
    const MethodDescriptor* descriptor_;

    HttpMethod httpMethod_;
    std::string path_;

    boost::regex regex_;
    PathParams params_;

    const std::string* uriPath_;

    // parsed data.
    std::string service_;
};

ServiceRequestMapping* ServiceRequestMapping::mapping_ = NULL;

ServiceRequestMapping& ServiceRequestMapping::instance() {
    if (!mapping_) {
        mapping_ = new ServiceRequestMapping;
    }

    return *mapping_;
}

ServiceRequestMapping::ServiceRequestMapping() {
    ProtobufServiceRegister::instance().registerServiceRegisteredCallback(
        boost::bind(&ServiceRequestMapping::onServiceRegistered,
                    this,
                    _1));
}

ServiceRequestMapping::~ServiceRequestMapping() {
}

ProtobufServiceMessagePtr ServiceRequestMapping::toProtobufMessage(
    const HttpRequestPtr& request,
    std::string* format) {
    ServiceMethod* method = route(request);

    if (method) {
        const Message* prototype =
            ProtobufServiceRegister::instance().getRequestPrototype(
                method->serviceName(),
                method->methodName());

        if (prototype) {
            Message* req = prototype->New();

            if (req) {
                if (parseMessage(request, *method, req)) {
                    ProtobufServiceMessagePtr message(
                        new ProtobufServiceMessage(ProtobufServiceMessage::T_REQUEST,
                                                   method->serviceName(),
                                                   method->methodName()));
                    message->setPayload(req);

                    if (request->headers().hasHeader("X-protobuf-id")) {
                        message->setId(StringUtil::strto64(request->headers().headerValue("X-protobuf-id")));
                    }

                    StringPiece value = method->pathParamValue("format");
                    if (format && !value.empty()) {
                        format->assign(value.c_str(), value.size());
                    }

                    return message;
                }
                else {
                    delete req;
                    LOG_ERROR << "parse the request Message error of "
                              << method->fullMethodName();
                }
            }
            else {
                LOG_ERROR << "can not allocate the new request Message for "
                          << method->fullMethodName();
            }
        }
        else {
            LOG_ERROR << "has no such service register: "
                      << method->fullMethodName();
        }
    }
    else {
        LOG_ERROR << "wrong uri format, can not match any rpc method."
                  << request->getUriString();
    }

    return ProtobufServiceMessagePtr();
}

bool ServiceRequestMapping::parseMessage(const HttpRequestPtr& request,
        const ServiceMethod& method,
        Message* message) {
    BOOST_ASSERT(message && "Message should not be NULL.");

    const google::protobuf::Reflection* reflection = message->GetReflection();
    const google::protobuf::Descriptor* descriptor = message->GetDescriptor();
    BOOST_ASSERT(reflection && descriptor && "reflection or descriptor should not be NULL.");

    bool parsed = false;

    if (request->headers().headerValue(HttpHeaders::Names::CONTENT_TYPE).compare(HttpHeaders::Values::X_PROTOBUFFER) == 0) {
        const char* content = NULL;
        int contentSize = 0;
        content = request->content()->readableBytes(&contentSize);
        if (content) {
            return message->ParseFromArray(content, contentSize);
        }
        else {
            return false;
        }
    }

    //std::string format = method.pathParamValue("format");

    if (descriptor->options().HasExtension(craft_message_options)) {
        const CraftMessageOptions& messageOptions =
            descriptor->options().GetExtension(craft_message_options);

        std::string consumer;

        if (messageOptions.has_entity_field()) {
            const FieldDescriptor* field =
                descriptor->FindFieldByName(messageOptions.entity_field());

            if (field->options().HasExtension(craft_options)) {
                const CraftFieldOptions& fieldOptions =
                    field->options().GetExtension(craft_options);

                if (!parseField(request, method, fieldOptions, field, message)) {
                    LOG_ERROR << "parse filed: " << field->full_name() << " error.";
                    return false;
                }
                else {
                    parsed = true;
                }
            }
        }
        else {
            StringPiece value = getValue(request, messageOptions);
            return parseMessage(value, consumer, message);
        }
    }

    int fieldCnt = descriptor->field_count();

    for (int i = 0; i < fieldCnt; ++i) {
        const google::protobuf::FieldDescriptor* field = descriptor->field(i);

        if (field->options().HasExtension(craft_options)) {
            const CraftFieldOptions& fieldOptions =
                field->options().GetExtension(craft_options);

            if (!parseField(request, method, fieldOptions, field, message)) {
                LOG_ERROR << "parse filed: " << field->full_name() << " error.";
                return false;
            }
            else {
                parsed = true;
            }
        }
    }

    return parsed;
}

bool ServiceRequestMapping::parseMessage(const StringPiece& value,
        const std::string& consumer,
        Message* message) {
    ProtobufParser* parser = ProtobufParser::getParser("json");

    if (parser) {
        try {
            return parser->parse(value.c_str(), value.size(), message) == 0;
        }
        catch (const std::exception& e) {
            LOG_ERROR << "parse message has error: " << e.what();
            return false;
        }
    }

    return true;
}

bool ServiceRequestMapping::parseField(const HttpRequestPtr& request,
                                       const ServiceMethod& method,
                                       const CraftFieldOptions& options,
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

        return parseMessage(request, method, msg);
    }

    std::vector<StringPiece> values;

    if (!getValues(request, method, options, &values)) {
        return false;
    }

    if (field->is_repeated()) {
        std::vector<StringPiece>::const_iterator itr;

        for (itr = values.begin(); itr != values.end(); ++itr) {
            const StringPiece& value = *itr;

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
                reflection->AddDouble(message, field, StringUtil::strtof(value.c_str()));
                break;

            case google::protobuf::FieldDescriptor::CPPTYPE_STRING:
                reflection->AddString(message, field, value.c_str());
                break;

            default:
                return false;
            }
        }
    }
    else {
        if (values.size() == 1) {
            const StringPiece& value = values.front();

            switch (field->cpp_type()) {
            case google::protobuf::FieldDescriptor::CPPTYPE_BOOL:
                if (StringUtil::iequals(value.c_str(), "true")
                        || StringUtil::iequals(value.c_str(), "1")
                        || StringUtil::iequals(value.c_str(), "yes")
                        || StringUtil::iequals(value.c_str(), "y")) {
                    reflection->SetBool(message, field, true);
                }
                else {
                    reflection->SetBool(message, field, false);
                }

                break;

            case google::protobuf::FieldDescriptor::CPPTYPE_INT32:
                reflection->SetInt32(message, field, StringUtil::strto32(value));
                break;

            case google::protobuf::FieldDescriptor::CPPTYPE_INT64:
                reflection->SetInt64(message, field, StringUtil::strto64(value));
                break;

            case  google::protobuf::FieldDescriptor::CPPTYPE_DOUBLE:
                reflection->SetDouble(message, field, StringUtil::strtof(value.as_string()));
                break;

            case google::protobuf::FieldDescriptor::CPPTYPE_STRING:
                reflection->SetString(message, field, value.as_string());
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

bool ServiceRequestMapping::getValues(const HttpRequestPtr& request,
                                      const ServiceMethod& method,
                                      const CraftFieldOptions& options,
                                      std::vector<StringPiece>* values) {
    if (NULL == values) {
        return false;
    }

    if (options.has_path_param()) {
        StringPiece value = method.pathParamValue(options.path_param());
        if (!value.empty()) {
            values->push_back(value);
            return true;
        }
    }
    else if (options.has_query_param()) {
        const NameValueCollection& nameValues = request->queryParameters();
        return nameValues.get(options.query_param(), values) > 0;
    }
    else if (options.has_cookie_param()) {

    }
    else if (options.has_header_param()) {

    }
    else if (options.has_form_param()) {

    }

    return false;
}

void ServiceRequestMapping::onServiceRegistered(const ProtobufServicePtr& service) {
    const ServiceDescriptor* descriptor = service->GetDescriptor();

    const std::string& path =
        descriptor->options().GetExtension(craft_service_options).path();

    int methodCount = descriptor->method_count();
    std::string methodName;

    for (int i = 0; i < methodCount; ++i) {
        const MethodDescriptor* method = descriptor->method(i);
        methodName = method->full_name();

        if (serviceMethods_.find(methodName) == serviceMethods_.end()) {
            ServiceMethod* serviceMethod = new ServiceMethod;

            if (serviceMethod->init(path, method)) {
                serviceMethods_.insert(methodName, serviceMethod);
            }
            else {
                delete serviceMethod;
            }
        }
    }
}

ServiceMethod* ServiceRequestMapping::route(const HttpRequestPtr& request) {
    ServiceMethods::iterator itr = serviceMethods_.begin();

    for (; itr != serviceMethods_.end(); ++itr) {
        ServiceMethod* method = itr->second;

        if (method->match(request)) {
            return method;
        }
    }

    return NULL;
}

cetty::util::StringPiece ServiceRequestMapping::getValue(
    const HttpRequestPtr& request,
    const CraftMessageOptions& options) {
    if (options.has_path_param()) {

    }
    else if (options.has_query_param()) {
        const NameValueCollection& nameValues = request->queryParameters();
        return nameValues.get(options.query_param());
    }
    else if (options.has_cookie_param()) {

    }
    else if (options.has_header_param()) {

    }
    else if (options.has_form_param()) {

    }

    return StringPiece();
}

}
}
}
