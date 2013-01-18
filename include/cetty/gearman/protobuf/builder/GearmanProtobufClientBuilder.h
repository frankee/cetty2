#if !defined(CETTY_GEARMAN_PROTOBUF_GEARMANPROTOBUFCLIENTBUILDER_H)
#define CETTY_GEARMAN_PROTOBUF_GEARMANPROTOBUFCLIENTBUILDER_H

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
#include <cetty/protobuf/service/ProtobufServiceMessage.h>

namespace cetty {
namespace gearman {
namespace protobuf {
namespace builder {

using namespace cetty::service;
using namespace cetty::service::builder;
using namespace cetty::protobuf::service;

class GearmanProtobufClientBuilder : private boost::noncopyable {
public:
    typedef ClientBuilder<ProtobufServiceMessagePtr> Builder;

public:
    GearmanProtobufClientBuilder();
    GearmanProtobufClientBuilder(int threadCnt);
    GearmanProtobufClientBuilder(const EventLoopPtr& eventLoop);
    GearmanProtobufClientBuilder(const EventLoopPoolPtr& eventLoopPool);
    
    ~GearmanProtobufClientBuilder() {}

    GearmanProtobufClientBuilder& addConnection(const std::string& host, int port) {
        builder_.addConnection(host, port, 1);
        return *this;
    }

    ChannelPtr build() {
        return builder_.build();
    }

private:
    void init();

private:
    Builder builder_;
};

}
}
}
}

#endif //#if !defined(CETTY_GEARMAN_PROTOBUF_GEARMANPROTOBUFCLIENTBUILDER_H)

// Local Variables:
// mode: c++
// End:
