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

#include <cetty/protobuf/ProtobufClientServiceAdaptor.h>

#include <boost/bind.hpp>
#include <google/protobuf/descriptor.h>

#include <cetty/protobuf/ProtobufService.h>
#include <cetty/protobuf/handler/ProtobufRpcMessage.h>
#include <cetty/protobuf/proto/rpc.pb.h>

namespace cetty {
namespace protobuf {

using namespace cetty::protobuf::handler;
using namespace cetty::protobuf::proto;

// Call the given method of the remote service.  The signature of this
// procedure looks the same as Service::CallMethod(), but the requirements
// are less strict in one important way:  the request and response objects
// need not be of any specific class as long as their descriptors are
// method->input_type() and method->output_type().
void ProtobufClientServiceAdaptor::CallMethod(
    const ::google::protobuf::MethodDescriptor* method,
    const ::google::protobuf::Message* request,
    const ProtobufServiceFuturePtr& future) {

    if (!service) {
        if (future) {
            future->setFailure();
        }

        return;
    }

    ProtobufRpcMessagePtr message(
        new ProtobufRpcMessage(REQUEST,
                               method->service()->full_name(),
                               method->name(),
                               request));

    service->call(message, future);
}

}
}
