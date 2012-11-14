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

#include <cetty/craft/http/ProtobufMessage2HttpResponse.h>

#include <google/protobuf/message.h>
#include <boost/variant.hpp>
#include <cetty/Types.h>

#include <cetty/logging/LoggerHelper.h>

#include <cetty/buffer/Unpooled.h>
#include <cetty/buffer/ChannelBuffer.h>
#include <cetty/handler/codec/http/HttpResponse.h>
#include <cetty/handler/codec/http/HttpRequest.h>
#include <cetty/handler/codec/http/HttpHeaders.h>
#include <cetty/protobuf/service/ProtobufUtil.h>
#include <cetty/protobuf/service/ProtobufServiceMessage.h>
#include <cetty/protobuf/serialization/ProtobufFormatter.h>

#include <cetty/craft/http/ServiceResponseMapper.h>

namespace cetty {
namespace craft {
namespace http {

using namespace cetty::buffer;
using namespace cetty::handler::codec::http;
using namespace cetty::protobuf::service;
using namespace cetty::protobuf::serialization;

using google::protobuf::Message;

class MessageFieldFormatter : public boost::static_visitor<ChannelBufferPtr> {
private:
    static const int RESERVED_MIN_SIZE = 512;
    static const int RESERVED_WRITE_SIZE = 12;
    static const int RESERVED_AHEAD_WRITE_SIZE = 512;

public:
    MessageFieldFormatter(ProtobufFormatter* formatter, const Message* message)
        : formatter(formatter), message(message)  {
    }

    void setFieldName(const std::string& fieldName) {
        this->fieldName = fieldName;
    }

    ChannelBufferPtr operator()(int64_t value) const {
        ChannelBufferPtr content = Unpooled::buffer(
                                       fieldName.size() + 16 + RESERVED_WRITE_SIZE,
                                       RESERVED_AHEAD_WRITE_SIZE);

        formatter->format(fieldName, value, content);

        return content;
    }

    ChannelBufferPtr operator()(double value) const {
        ChannelBufferPtr content = Unpooled::buffer(
                                       fieldName.size() + 32 + RESERVED_WRITE_SIZE,
                                       RESERVED_AHEAD_WRITE_SIZE);

        formatter->format(fieldName, value, content);

        return content;
    }

    ChannelBufferPtr operator()(const std::string* value) const {
        ChannelBufferPtr content;

        if (value) {
            content = Unpooled::buffer(
                          value->size() + fieldName.size() + RESERVED_WRITE_SIZE,
                          RESERVED_AHEAD_WRITE_SIZE);

            formatter->format(fieldName, *value, content);
        }

        return content;
    }

    ChannelBufferPtr operator()(const Message* value) const {
        ChannelBufferPtr content;

        if (value) {
            int size = value->ByteSize();
            content = Unpooled::buffer(size + RESERVED_MIN_SIZE * 4,
                                                    RESERVED_AHEAD_WRITE_SIZE);

            formatter->format(*value, content);
        }

        return content;
    }

    ChannelBufferPtr operator()(const std::vector<int64_t>& value) const {
        ChannelBufferPtr content = Unpooled::buffer(
                                       fieldName.size() + 16 * value.size() + RESERVED_WRITE_SIZE,
                                       RESERVED_AHEAD_WRITE_SIZE);

        formatter->format(fieldName, value, content);
        return content;
    }

    ChannelBufferPtr operator()(const std::vector<double>& value) const {
        ChannelBufferPtr content = Unpooled::buffer(
                                       fieldName.size() + 32 * value.size() + RESERVED_WRITE_SIZE,
                                       RESERVED_AHEAD_WRITE_SIZE);

        formatter->format(fieldName, value, content);

        return content;
    }

    ChannelBufferPtr operator()(const std::vector<const std::string*>& value) const {
        return ChannelBufferPtr();
    }

    ChannelBufferPtr operator()(const std::vector<const Message*>& value) const {
        return ChannelBufferPtr();
    }

    template<typename T>
    ChannelBufferPtr operator()(const T& value) const {
        return ChannelBufferPtr();
    }

private:
    ProtobufFormatter* formatter;
    const Message* message;

    std::string fieldName;
};

ProtobufMessage2HttpResponse::ProtobufMessage2HttpResponse() {
}

ProtobufMessage2HttpResponse::ProtobufMessage2HttpResponse(
    const ServiceResponseMapperPtr& mapper)
    : mapper(mapper) {
}

void ProtobufMessage2HttpResponse::setResponseMapper(
    const ServiceResponseMapperPtr& mapper) {
    this->mapper = mapper;
}

HttpResponsePtr ProtobufMessage2HttpResponse::getHttpResponse(
    const HttpRequestPtr& req,
    const ProtobufServiceMessagePtr& message) {
    const ServiceResponseMapper::ResponseTemplate* tmpl
        = mapper->match(message->getService(), message->getMethod());

    if (tmpl) {
        HttpResponsePtr response(new HttpResponse(HttpVersion::HTTP_1_1,
                                 HttpResponseStatus::OK));

        ServiceResponseMapper::setHttpHeaders(*tmpl, response);

        const std::string& format = req->getLabel();
        ProtobufFormatter* formatter = ProtobufFormatter::getFormatter(format);

        if (!formatter) {
            //
            //response->setStatus(HttpResponseStatus::NOT_FOUND);
        }

        Message* paylod = message->getPayload();

        if (!paylod) {
            //
        }

        ChannelBufferPtr content;
        MessageFieldFormatter f(formatter, paylod);

        if (tmpl->content.empty()) {
            content = f((const Message*)paylod);
        }
        else {
            ProtobufUtil::FieldValue field
                = ProtobufUtil::getMessageField(tmpl->content, *paylod);
            f.setFieldName(tmpl->content);

            content = field.apply_visitor(f);
        }

        response->setContent(content);

        // for jsonp
        if (format == "json") {
            std::string jquery = req->getQueryParameters().get("jsoncallback");

            if (!jquery.empty()) {
                jquery.append("(");
                content->writeBytesAhead(jquery);
                content->writeByte(')');
            }
        }

        // Decide whether to close the connection or not.
        bool keepAlive = HttpHeaders::isKeepAlive(*req);

        if (keepAlive) {
            // Add 'Content-Length' header only for a keep-alive connection.
            LOG_DEBUG << "keep alive mode,set the content_length header";
            
            response->setHeader(HttpHeaders::Names::CONNECTION,
                HttpHeaders::Values::KEEP_ALIVE);
            
            response->setHeader(HttpHeaders::Names::CONTENT_LENGTH,
                                response->getContent()->readableBytes());
        }
        else {
            response->setHeader(HttpHeaders::Names::CONNECTION,
                                HttpHeaders::Values::CLOSE);
        }

        return response;
    }

    return HttpResponsePtr();
}

}
}
}
