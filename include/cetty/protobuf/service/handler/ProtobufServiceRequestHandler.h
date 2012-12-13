#if !defined(CETTY_PROTOBUF_SERVICE_HANDLER_PROTOBUFSERVICEREQUESTHANDLER_H)
#define CETTY_PROTOBUF_SERVICE_HANDLER_PROTOBUFSERVICEREQUESTHANDLER_H

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

#include <boost/intrusive_ptr.hpp>
#include <cetty/service/OutstandingCall.h>
#include <cetty/service/ServiceRequestHandler.h>
#include <cetty/protobuf/service/ProtobufServiceMessage.h>

namespace cetty {
namespace service {

using namespace cetty::protobuf::service;

// template<>
// int64_t OutstandingCall<ProtobufServiceMessagePtr, ProtobufServiceMessagePtr>::id() const {
//     return request->id(); //future->getResponse()->getId();
// }
// 
// template<>
// void OutstandingCall<ProtobufServiceMessagePtr, ProtobufServiceMessagePtr>::setId(int64_t id) {
//     request->setId(id);
// }
// 
// template<>
// void ClientService<ProtobufServiceMessagePtr, ProtobufServiceMessagePtr>::doInitialize() {
// 
// }

}
}

namespace cetty {
namespace protobuf {
namespace service {
namespace handler {

using namespace cetty::service;
using namespace cetty::protobuf::service;

typedef OutstandingCall<ProtobufServiceMessagePtr, ProtobufServiceMessagePtr> ProtobufServiceCall;
typedef boost::intrusive_ptr<ProtobufServiceCall> ProtobufServiceCallPtr;

typedef ClientServiceRequestAdaptor<ProtobufServiceMessagePtr, ProtobufServiceMessagePtr> ProtobufServiceRequestHandler;

}
}
}
}

#endif //#if !defined(CETTY_PROTOBUF_SERVICE_HANDLER_PROTOBUFSERVICEREQUESTHANDLER_H)

// Local Variables:
// mode: c++
// End:
