#if !defined(CETTY_PROTOBUF_SERVICE_PROTOBUFCLIENTSERVICEADAPTOR_H)
#define CETTY_PROTOBUF_SERVICE_PROTOBUFCLIENTSERVICEADAPTOR_H

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

// Service and RpcChannel classes are incorporated from
// google/protobuf/service.h

// Protocol Buffers - Google's data interchange format
// Copyright 2008 Google Inc.  All rights reserved.
// http://code.google.com/p/protobuf/
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//     * Neither the name of Google Inc. nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

// Author: kenton@google.com (Kenton Varda)
//  Based on original Protocol Buffers design by
//  Sanjay Ghemawat, Jeff Dean, and others.

#include <boost/bind.hpp>
#include <cetty/service/ClientServicePtr.h>
#include <cetty/service/TypeCastServiceFuture.h>
#include <cetty/protobuf/service/ProtobufServiceFuture.h>
#include <cetty/protobuf/service/ProtobufServiceMessage.h>

namespace google {
namespace protobuf {

// Defined in other files.
class Descriptor;            // descriptor.h
class ServiceDescriptor;     // descriptor.h
class MethodDescriptor;      // descriptor.h
class Message;               // message.h

}
}

namespace cetty {
namespace protobuf {
namespace service {

using namespace cetty::service;


// template<typename To, typename From>     // use like this: down_cast<T*>(foo);
// inline To bare_point_down_cast(const From& f) {                   // so we only accept pointers
// #if !defined(NDEBUG)
//     assert(!f || point_dynamic_cast<To>(f) != NULL);  // RTTI: debug mode only!
// #endif
//     return static_cast<To>(f);
// }

// Abstract interface for an RPC channel.  An RpcChannel represents a
// communication line to a Service which can be used to call that Service's
// methods.  The Service may be running on another machine.  Normally, you
// should not call an RpcChannel directly, but instead construct a stub Service
// wrapping it.  Example:
//
//   ProtobufClientServicePtr service = ProtobufClientBuilder("remotehost.example.com:1234");
//   MyService* myService = new MyService::Stub(service);
//   MyMehodResponseFuturePtr future(new MyMehodResponseFuture(callback));
//   myService->MyMethod(request, future);
//
class ProtobufClientServiceAdaptor {
public:
    ProtobufClientServiceAdaptor(const ClientServicePtr& service);
    ~ProtobufClientServiceAdaptor();

    // Call the given method of the remote service.  The signature of this
    // procedure looks the same as Service::CallMethod(), but the requirements
    // are less strict in one important way:  the request and response objects
    // need not be of any specific class as long as their descriptors are
    // method->input_type() and method->output_type().
    void CallMethod(const ::google::protobuf::MethodDescriptor* method,
                    const ConstMessagePtr& request,
                    const ProtobufServiceFuturePtr& future);

    template<typename RepT>
    RepT downPointerCast(const ProtobufServiceMessagePtr& from) {
        return cetty::util::static_pointer_cast<typename RepT::element_type>(from->getPayload());
    }

    template<typename ReqT, typename RepT>
    void CallMethod(const ::google::protobuf::MethodDescriptor* method,
                    const cetty::util::BarePointer<ReqT const>& request,
                    const boost::intrusive_ptr<ServiceFuture<RepT> >& future) {
        CallMethod(method,
                   cetty::util::static_pointer_cast<typename MessagePtr::element_type const>(request),
                   ProtobufServiceFuturePtr(
                       new TypeCastServiceFuture<ProtobufServiceMessagePtr, RepT>(future,
                               boost::bind(&ProtobufClientServiceAdaptor::downPointerCast<RepT>, this, _1))));
    }

    const ClientServicePtr& getService() {
        return service;
    }

private:
    ClientServicePtr service;
};

}
}
}

#endif //#if !defined(CETTY_PROTOBUF_SERVICE_PROTOBUFCLIENTSERVICEADAPTOR_H)

// Local Variables:
// mode: c++
// End:
