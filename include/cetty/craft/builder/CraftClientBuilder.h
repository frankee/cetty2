#if !defined(CETTY_CRAFT_BUILDER_CRAFTRPCCLIENTBUILDER_H)
#define CETTY_CRAFT_BUILDER_CRAFTRPCCLIENTBUILDER_H

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

#include <cetty/protobuf/service/builder/ProtobufClientBuilder.h>

namespace cetty {
namespace craft {
namespace builder {

using namespace cetty::channel;
using namespace cetty::protobuf::service::builder;

class CraftClientBuilder : private boost::noncopyable {
public:
    CraftClientBuilder();
    CraftClientBuilder(int threadCnt);
    CraftClientBuilder(const EventLoopPtr& eventLoop);
    CraftClientBuilder(const EventLoopPoolPtr& eventLoopPool);

    void setServiceInitializer(const Channel::Initializer& initializer) {
        builder_.setServiceInitializer(initializer);
    }

    void addConnection(const std::string& host, int port, int limit = 1) {
        builder_.addConnection(host, port, limit);
    }

    ChannelPtr build() {
        return builder_.build();
    }

private:
    ProtobufClientBuilder builder_;
};

}
}
}

#endif //#if !defined(CETTY_CRAFT_BUILDER_CRAFTRPCCLIENTBUILDER_H)

// Local Variables:
// mode: c++
// End:
