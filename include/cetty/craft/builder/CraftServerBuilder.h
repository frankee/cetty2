#if !defined(CETTY_CRAFT_BUILDER_CRAFTSERVERBUILDER_H)
#define CETTY_CRAFT_BUILDER_CRAFTSERVERBUILDER_H

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
#include <cetty/protobuf/service/ProtobufServicePtr.h>
#include <cetty/protobuf/service/builder/ProtobufServerBuilder.h>

namespace cetty {
namespace config {
class ConfigCenter;
}
}

namespace cetty {
namespace craft {
namespace builder {

using namespace cetty::channel;
using namespace cetty::protobuf::service;

class CraftServerBuilder
    : public cetty::protobuf::service::builder::ProtobufServerBuilder {
public:
    CraftServerBuilder();
    CraftServerBuilder(int parentThreadCnt, int childThreadCnt);

    virtual ~CraftServerBuilder();

    ChannelPtr buildHttp(int port);

private:
    void init();
    ChannelPipelinePtr createHttpServicePipeline();
};

}
}
}

#endif //#if !defined(CETTY_CRAFT_BUILDER_CRAFTSERVERBUILDER_H)

// Local Variables:
// mode: c++
// End:
