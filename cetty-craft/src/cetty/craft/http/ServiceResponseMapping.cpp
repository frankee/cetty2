/**
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

#include <cetty/craft/http/ServiceResponseMapping.h>

#include <boost/bind.hpp>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/descriptor.pb.h>

#include <cetty/logging/LoggerHelper.h>

#include <cetty/buffer/Unpooled.h>
#include <cetty/buffer/ChannelBuffer.h>
#include <cetty/handler/codec/http/HttpResponse.h>
#include <cetty/handler/codec/http/HttpRequest.h>
#include <cetty/handler/codec/http/HttpHeaders.h>
#include <cetty/protobuf/service/ProtobufUtil.h>
#include <cetty/protobuf/service/ProtobufService.h>
#include <cetty/protobuf/service/ProtobufServiceMessage.h>
#include <cetty/protobuf/service/ProtobufServiceRegister.h>
#include <cetty/protobuf/serialization/ProtobufFormatter.h>

#include <cetty/craft/craft.pb.h>

namespace cetty {
namespace craft {
namespace http {

using namespace google::protobuf;
using namespace cetty::protobuf::service;
using namespace cetty::protobuf::serialization;

static const int RESERVED_MIN_SIZE = 512;
static const int RESERVED_WRITE_SIZE = 12;
static const int RESERVED_AHEAD_WRITE_SIZE = 512;

ServiceResponseMapping::ServiceResponseMapping() {
    ProtobufServiceRegister::instance().registerServiceRegisteredCallback(
        boost::bind(&ServiceResponseMapping::onServiceRegistered,
                    this,
                    _1));
}

HttpResponsePtr ServiceResponseMapping::toHttpResponse(
    const HttpRequestPtr& req,
    const ProtobufServiceMessagePtr& message,
    const std::string& format) {

    HttpResponsePtr response(new HttpResponse(HttpVersion::HTTP_1_1,
                             HttpResponseStatus::OK));

    std::string method = message->method();
    setHttpHeaders(method, response);

    const Message* paylod = message->payload();

    if (paylod) {
        const FieldDescriptor* field = paylod->GetDescriptor()->FindFieldByName("status");
        const Reflection* reflection = paylod->GetReflection();
        if (reflection->HasField(*paylod, field)) {
            Message* status = reflection->MutableMessage(const_cast<Message*>(paylod), field);
            if (status) {
                const FieldDescriptor* f = status->GetDescriptor()->FindFieldByName("code");
                const Reflection* r = status->GetReflection();
                int httpStatus = r->GetInt32(*status, f);
                if (httpStatus >= 300 ) {
                    response->setStatus(HttpResponseStatus::valueOf(httpStatus));
                }

                //r->ClearField(status, f);
            }
        }

        setHttpContent(*paylod, format, response);
    }

    // for jsonp
    if (format.compare("json") == 0) {
        std::string jquery = req->queryParameters().get("jsoncallback");

        if (!jquery.empty()) {
            jquery.append("(");
            response->content()->writeBytesAhead(jquery);
            response->content()->writeByte(')');
        }

        response->headers().addHeader(HttpHeaders::Names::CONTENT_TYPE, "application/json");
    }
    else if (format.compare("png") == 0) {
        response->headers().addHeader(HttpHeaders::Names::CONTENT_TYPE, "image/png");
        response->headers().addHeader(HttpHeaders::Names::CACHE_CONTROL, "max-age=2592000");
    }
    else if (format.compare("vt") == 0) {
        response->headers().addHeader(HttpHeaders::Names::CONTENT_TYPE, "application/x-vtile");
        response->headers().addHeader(HttpHeaders::Names::CACHE_CONTROL, "max-age=2592000");

        if (message->id() > 0) {
            response->headers().addHeader("X-protobuf-id", StringUtil::numtostr(message->id()));
        }
    }
    else if (format.compare("pb") == 0) {
        response->headers().addHeader(HttpHeaders::Names::CONTENT_TYPE, HttpHeaders::Values::X_PROTOBUFFER);
        response->headers().addHeader("X-protobuf-id", StringUtil::numtostr(message->id()));
    }

    // Decide whether to close the connection or not.
    bool keepAlive = req->keepAlive();

    if (keepAlive) {
        // Add 'Content-Length' header only for a keep-alive connection.
        LOG_DEBUG << "keep alive mode,set the content_length header";

        response->headers().setHeader(HttpHeaders::Names::CONNECTION,
                                      HttpHeaders::Values::KEEP_ALIVE);

        if (response->content() && response->content()->readable()) {
            response->headers().setHeader(HttpHeaders::Names::CONTENT_LENGTH,
                                          response->content()->readableBytes());
        }
        else {
            response->headers().setHeader(HttpHeaders::Names::CONTENT_LENGTH, 0);
        }
    }
    else {
        response->headers().setHeader(HttpHeaders::Names::CONNECTION,
                                      HttpHeaders::Values::CLOSE);
    }

    return response;
}

void ServiceResponseMapping::setHttpContent(const Message& message,
        const std::string& producer,
        const HttpResponsePtr& response) {
    ChannelBufferPtr content;
    const CraftMessageOptions& options =
        message.GetDescriptor()->options().GetExtension(craft_message_options);

    if (options.has_raw_field()) {
        const FieldDescriptor* field =
            message.GetDescriptor()->FindFieldByName(options.raw_field());
        const Reflection* reflection = message.GetReflection();

        const CraftFieldOptions& fieldOptions =
            field->options().GetExtension(craft_options);

        BOOST_ASSERT(field);

        if (field->type() == FieldDescriptor::TYPE_MESSAGE) {
            const Message& msg = reflection->GetMessage(message, field);

            if (fieldOptions.has_raw_field()) {
                const FieldDescriptor* fieldfield =
                    msg.GetDescriptor()->FindFieldByName(fieldOptions.raw_field());
                const Reflection* fieldReflection = msg.GetReflection();

                const std::string& fieldContent =
                    fieldReflection->GetStringReference(msg, fieldfield, NULL);

                content = Unpooled::buffer(fieldContent.size(),
                                           RESERVED_AHEAD_WRITE_SIZE);
                content->writeBytes(fieldContent);
                response->setContent(content);
                return;
            }
        }
        else {

        }
    }

    if (producer.compare("pb") == 0) {
        int size = message.ByteSize();
        content = Unpooled::buffer(RESERVED_MIN_SIZE * 2 + size, RESERVED_AHEAD_WRITE_SIZE);
        char* data;
        int dataSize;
        data = content->writableBytes(&dataSize);

        uint8_t* out = message.SerializeWithCachedSizesToArray((uint8_t*)data);
        content->offsetReaderIndex(out - (uint8_t*)data);
        response->setContent(content);
        return;
    }

    ProtobufFormatter* formatter = NULL;

    if (!producer.empty()) {
        formatter = ProtobufFormatter::getFormatter(producer);
    }
    else {
        //producer = "json";
        formatter = ProtobufFormatter::getFormatter("json");
    }

    if (!formatter) {
        //formatter = ProtobufFormatter::getFormatter("json");
        LOG_ERROR << "CAN't found the formatter: " << producer;
        return;
        //
        //response->setStatus(HttpResponseStatus::NOT_FOUND);
    }

    if (options.has_entity_field()) {
        const FieldDescriptor* field =
            message.GetDescriptor()->FindFieldByName(options.entity_field());
        const Reflection* reflection = message.GetReflection();
        BOOST_ASSERT(field);

        if (field->is_repeated()) {
            switch (field->type()) {
            case FieldDescriptor::TYPE_INT64:
                content = Unpooled::buffer(
                              RESERVED_WRITE_SIZE,
                              RESERVED_AHEAD_WRITE_SIZE);

                //formatter->format(field->camelcase_name(), , content);
                break;

            case FieldDescriptor::CPPTYPE_DOUBLE:
                //                 content = Unpooled::buffer(
                //                               fieldName.size() + 32 * value.size() + RESERVED_WRITE_SIZE,
                //                               RESERVED_AHEAD_WRITE_SIZE);
                //
                //                 formatter->format(fieldName, value, content);
                break;

            default:
                break;
            }
        }
        else {
            switch (field->type()) {
            case FieldDescriptor::TYPE_INT64:
                //                 content = Unpooled::buffer(
                //                               fieldName.size() + 16 + RESERVED_WRITE_SIZE,
                //                               RESERVED_AHEAD_WRITE_SIZE);
                //
                //                 formatter->format(fieldName, value, content);

                break;

            case FieldDescriptor::TYPE_DOUBLE:
                //                 content = Unpooled::buffer(
                //                               fieldName.size() + 32 + RESERVED_WRITE_SIZE,
                //                               RESERVED_AHEAD_WRITE_SIZE);
                //
                //                 formatter->format(fieldName, value, content);

                break;

            case FieldDescriptor::TYPE_BYTES:
                break;

            case FieldDescriptor::TYPE_STRING:
                //                 if (value) {
                //                     content = Unpooled::buffer(
                //                                   value->size() + fieldName.size() + RESERVED_WRITE_SIZE,
                //                                   RESERVED_AHEAD_WRITE_SIZE);
                //
                //                     formatter->format(fieldName, *value, content);
                //                 }

                break;

            case FieldDescriptor::TYPE_MESSAGE:
                const Message& msg = reflection->GetMessage(message, field);

                int size = msg.ByteSize();
                content = Unpooled::buffer(size * 8 + RESERVED_MIN_SIZE * 4,
                                           RESERVED_AHEAD_WRITE_SIZE);

                formatter->format(msg, content);

                break;
            }
        }
    }
    else {
        content = Unpooled::buffer(RESERVED_MIN_SIZE * 4,
                                   RESERVED_AHEAD_WRITE_SIZE);

        formatter->format(message, content);
    }

    {
        const char* bytes = NULL;
        int size = 0;
        bytes = content->readableBytes(&size);

        if (size == 0 || (size == 2 && strncmp(bytes, "{}", 2) == 0)) {
            content->clear();
        }
    }

    response->setContent(content);
}

void ServiceResponseMapping::setHttpHeaders(const std::string& method,
        const HttpResponsePtr& response) {

    //response->headers().
}

void ServiceResponseMapping::onServiceRegistered(const ProtobufServicePtr& service) {
    const ServiceDescriptor* descriptor = service->GetDescriptor();

    int methodCount = descriptor->method_count();

    for (int i = 0; i < methodCount; ++i) {
        const MethodDescriptor* method = descriptor->method(i);
        const MethodOptions& options = method->options();

        if (options.HasExtension(craft_method_options)) {
            const CraftMethodOptions& methodOptions =
                options.GetExtension(craft_method_options);

            parseHeaders(methodOptions.headers(), &headers_[method->full_name()]);
        }
    }
}

void ServiceResponseMapping::parseHeaders(const std::string& headerStr,
        NameValueCollection* headers) {

}

ServiceResponseMapping& ServiceResponseMapping::instance() {
    if (!mapping_) {
        mapping_ = new ServiceResponseMapping;
    }

    return *mapping_;
}

ServiceResponseMapping* ServiceResponseMapping::mapping_ = NULL;

}
}
}
