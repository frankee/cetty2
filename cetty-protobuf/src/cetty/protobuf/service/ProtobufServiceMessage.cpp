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

#include <boost/assert.hpp>
#include <cetty/protobuf/service/ProtobufServiceMessage.h>
#include <cetty/protobuf/service/service.pb.h>

namespace cetty {
namespace protobuf {
namespace service {

using namespace cetty::protobuf::service;

ProtobufServiceMessage::ProtobufServiceMessage()
    : message_(new ServiceMessage), payload_() {

}

ProtobufServiceMessage::ProtobufServiceMessage(int type)
    : message_(new ServiceMessage), payload_() {
    message_->set_type(MessageType(type));
}

ProtobufServiceMessage::ProtobufServiceMessage(int type, int64_t id)
    : message_(new ServiceMessage), payload_() {
    message_->set_type(MessageType(type));
    message_->set_id(id);
}

ProtobufServiceMessage::ProtobufServiceMessage(int type,
        int64_t id,
        const MessagePtr& payload)
    : message_(new ServiceMessage), payload_(payload) {
    message_->set_type(MessageType(type));
    message_->set_id(id);
}

ProtobufServiceMessage::ProtobufServiceMessage(int type,
        const std::string& service,
        const std::string& method)
    : message_(new ServiceMessage), payload_() {
    message_->set_type(MessageType(type));
    message_->set_service(service);
    message_->set_method(method);
}

ProtobufServiceMessage::ProtobufServiceMessage(int type,
        const std::string& service,
        const std::string& method,
        const MessagePtr& payload)
    : message_(new ServiceMessage), payload_(payload) {
    message_->set_type(MessageType(type));
    message_->set_service(service);
    message_->set_method(method);
}

ProtobufServiceMessage::ProtobufServiceMessage(int type,
        int64_t id,
        const std::string& service,
        const std::string& method,
        const MessagePtr& payload)
    : message_(new ServiceMessage), payload_(payload) {
    message_->set_type(static_cast<MessageType>(type));
    message_->set_id(id);
    message_->set_service(service);
    message_->set_method(method);
}

ProtobufServiceMessage::~ProtobufServiceMessage() {
    if (message_) {
        delete message_;
    }

    if (payload_) {
        delete payload_;
    }
}

bool ProtobufServiceMessage::isRequest() const {
    BOOST_ASSERT(message_ && "message should not be NULL.");
    return message_->type() == REQUEST;
}

bool ProtobufServiceMessage::isResponse() const {
    BOOST_ASSERT(message_ && "message should not be NULL.");
    return message_->type() == RESPONSE;
}

bool ProtobufServiceMessage::isError() const {
    BOOST_ASSERT(message_ && "message should not be NULL.");
    return message_->type() == ERROR;
}

void ProtobufServiceMessage::setType(int type) {
    BOOST_ASSERT(message_ && "message should not be NULL.");
    message_->set_type(MessageType(type));
}

int ProtobufServiceMessage::type() const {
    BOOST_ASSERT(message_ && "message should not be NULL.");
    return (int)message_->type();
}

int64_t ProtobufServiceMessage::id() const {
    BOOST_ASSERT(message_ && "message should not be NULL.");
    return message_->id();
}

void ProtobufServiceMessage::setId(int64_t id) {
    BOOST_ASSERT(message_ && "message should not be NULL.");
    return message_->set_id(id);
}

const std::string& ProtobufServiceMessage::service() const {
    BOOST_ASSERT(message_ && "message should not be NULL.");
    return message_->service();
}

std::string* ProtobufServiceMessage::mutableService() {
    BOOST_ASSERT(message_ && "message should not be NULL.");
    return message_->mutable_service();
}

const std::string& ProtobufServiceMessage::method() const {
    BOOST_ASSERT(message_ && "message should not be NULL.");
    return message_->method();
}

std::string* ProtobufServiceMessage::mutableMethod() {
    BOOST_ASSERT(message_ && "message should not be NULL.");
    return message_->mutable_method();
}

const ServiceMessage& ProtobufServiceMessage::serviceMessage() const {
    BOOST_ASSERT(message_ && "message should not be NULL.");
    return *message_;
}

static MessagePtr EMPTY_MESSAGE= NULL;

const MessagePtr& ProtobufServiceMessage::response() const {
    if (isResponse()) {
        return payload_;
    }

    return EMPTY_MESSAGE;
}

const MessagePtr& ProtobufServiceMessage::request() const {
    if (isRequest()) {
        return payload_;
    }

    return EMPTY_MESSAGE;
}

bool ProtobufServiceMessage::checkType(int type) {
    return MessageType_IsValid(type);
}

int ProtobufServiceMessage::messageSize() const {
    return message_->ByteSize() + (payload_ ? payload_->ByteSize() + 8 : 0);
}

}
}
}
