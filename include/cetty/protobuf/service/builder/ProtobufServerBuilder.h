#if !defined(CETTY_PROTOBUF_SERVICE_BUILDER_PROTOBUFSERVERBUILDER_H)
#define CETTY_PROTOBUF_SERVICE_BUILDER_PROTOBUFSERVERBUILDER_H

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

#include <cetty/channel/ChannelPtr.h>
#include <cetty/service/builder/ServerBuilder.h>
#include <cetty/protobuf/service/ProtobufServicePtr.h>

namespace cetty {
namespace config {
class ConfigCenter;
}
}

namespace cetty {
namespace protobuf {
namespace service {
namespace builder {

using namespace cetty::channel;
using namespace cetty::service;
using namespace cetty::config;
using namespace cetty::protobuf::service;

class ProtobufServerBuilder : public cetty::service::builder::ServerBuilder {
public:
    ProtobufServerBuilder();
    ProtobufServerBuilder(int parentThreadCnt, int childThreadCnt);

    virtual ~ProtobufServerBuilder();

    ProtobufServerBuilder& registerService(const ProtobufServicePtr& service);

    ChannelPtr buildRpc(int port);

private:
    void init();

    bool initializeChannel(const ChannelPtr& channel);
};

}
}
}
}

#endif //#if !defined(CETTY_PROTOBUF_SERVICE_BUILDER_PROTOBUFSERVERBUILDER_H)

// Local Variables:
// mode: c++
// End:
