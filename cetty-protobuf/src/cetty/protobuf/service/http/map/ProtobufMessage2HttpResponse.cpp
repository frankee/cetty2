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

#include <cetty/protobuf/service/http/map/ProtobufMessage2HttpResponse.h>

#include <google/protobuf/message.h>
#include <boost/variant.hpp>
#include <boost/cstdint.hpp>

#include <cetty/buffer/ChannelBuffer.h>
#include <cetty/buffer/ChannelBuffers.h>
#include <cetty/handler/codec/http/HttpResponse.h>
#include <cetty/handler/codec/http/HttpRequest.h>
#include <cetty/handler/codec/http/HttpHeaders.h>
#include <cetty/protobuf/service/ProtobufUtil.h>
#include <cetty/protobuf/service/ProtobufServiceMessage.h>
#include <cetty/protobuf/service/http/map/ServiceResponseMapper.h>
#include <cetty/protobuf/serialization/ProtobufFormatter.h>

namespace cetty {
namespace protobuf {
namespace service {
namespace http {
namespace map {

using namespace cetty::buffer;
using namespace cetty::handler::codec::http;
using namespace cetty::protobuf::service;
using namespace cetty::protobuf::serialization;

using google::protobuf::Message;

class MessageFieldFormatter : public boost::static_visitor<ChannelBufferPtr> {
public:
    MessageFieldFormatter(ProtobufFormatter* formatter, const Message* message)
        : formatter(formatter), message(message)  {
    }

    void setFieldName(const std::string& fieldName) {
        this->fieldName = fieldName;
    }

    ChannelBufferPtr operator()(boost::int64_t value) const {
        return ChannelBufferPtr();
    }

    ChannelBufferPtr operator()(double value) const {
        return ChannelBufferPtr();
    }

    ChannelBufferPtr operator()(const std::string* value) const {
        ChannelBufferPtr content;

        if (value) {
            content = ChannelBuffers::buffer(value->size());
            formatter->format(fieldName, *value, content);
        }
        return content;
    }

    ChannelBufferPtr operator()(const Message* value) const {
        std::string content;
        formatter->format(*value, &content);
        return ChannelBuffers::copiedBuffer(content);
    }

    ChannelBufferPtr operator()(const std::vector<boost::int64_t>& value) const {
        ChannelBufferPtr content = ChannelBuffers::buffer(1024);
        formatter->format(fieldName, value, content);
        return content;
    }

    ChannelBufferPtr operator()(const std::vector<double>& value) const {
        return ChannelBufferPtr();
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

        if (!tmpl->content.empty()) {
            ProtobufUtil::FieldValue field
                = ProtobufUtil::getMessageField(tmpl->content, *paylod);
            f.setFieldName(tmpl->content);

            content = field.apply_visitor(f);
        }
        else {
            content = f((const Message*)paylod);
        }

        response->setContent(content);

        // Decide whether to close the connection or not.
        bool keepAlive = HttpHeaders::isKeepAlive(*req);

        if (keepAlive) {
            // Add 'Content-Length' header only for a keep-alive connection.
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
}
}
