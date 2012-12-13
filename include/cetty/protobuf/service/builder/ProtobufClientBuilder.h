#if !defined(CETTY_PROTOBUF_SERVICE_BUILDER_PROTOBUFCLIENTBUILDER_H)
#define CETTY_PROTOBUF_SERVICE_BUILDER_PROTOBUFCLIENTBUILDER_H

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

#include <cetty/service/builder/ClientBuilder.h>
#include <cetty/protobuf/service/ProtobufServiceMessagePtr.h>

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

}
}

namespace cetty {
namespace protobuf {
namespace service {
namespace builder {

using namespace cetty::channel;
using namespace cetty::service::builder;

class ProtobufClientBuilder
        : public cetty::service::builder::ClientBuilder<ProtobufServiceMessagePtr> {
public:
    typedef ClientBuilder<ProtobufServiceMessagePtr> ClientBuilderType;

public:
    ProtobufClientBuilder();
    ProtobufClientBuilder(int threadCnt);
    ProtobufClientBuilder(const EventLoopPtr& eventLoop);
    ProtobufClientBuilder(const EventLoopPoolPtr& eventLoopPool);

private:
    void init();

private:
};

}
}
}
}

#endif //#if !defined(CETTY_PROTOBUF_SERVICE_BUILDER_PROTOBUFCLIENTBUILDER_H)

// Local Variables:
// mode: c++
// End:
