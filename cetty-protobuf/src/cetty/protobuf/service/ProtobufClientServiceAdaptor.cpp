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

#include <cetty/protobuf/service/ProtobufClientServiceAdaptor.h>

#include <boost/bind.hpp>
#include <google/protobuf/descriptor.h>

#include <cetty/service/ClientService.h>
#include <cetty/protobuf/service/ProtobufService.h>
#include <cetty/protobuf/service/ProtobufServiceMessage.h>
#include <cetty/protobuf/service/service.pb.h>
#include <cetty/util/Exception.h>

namespace cetty {
namespace protobuf {
namespace service {

using namespace google::protobuf;
using namespace cetty::service;
using namespace cetty::protobuf::service;
using namespace cetty::util;

// Call the given method of the remote service.  The signature of this
// procedure looks the same as Service::CallMethod(), but the requirements
// are less strict in one important way:  the request and response objects
// need not be of any specific class as long as their descriptors are
// method->input_type() and method->output_type().

ProtobufClientServiceAdaptor::ProtobufClientServiceAdaptor(const ClientServicePtr& service)
    : service(service) {
}

ProtobufClientServiceAdaptor::~ProtobufClientServiceAdaptor() {
}

void ProtobufClientServiceAdaptor::doCallMethod(
    const ::google::protobuf::MethodDescriptor* method,
    const ConstMessagePtr& request,
    const ProtobufServiceFuturePtr& future) {

    if (!service) {
        if (future) {
            future->setFailure(Exception(""));
        }

        return;
    }

    ProtobufServiceMessagePtr message(
        new ProtobufServiceMessage(REQUEST,
                                   method->service()->full_name(),
                                   method->name(),
                                   (MessagePtr)request));

    cetty::service::callMethod(service, message, future);
}

}
}
}
