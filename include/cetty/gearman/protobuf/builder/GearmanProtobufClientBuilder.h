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

class GearmanProtobufClientBuilder
        : public ClientBuilder<ProtobufServiceMessagePtr> {
public:
    typedef ClientBuilder<ProtobufServiceMessagePtr> ClientBuilderType;

public:
    GearmanProtobufClientBuilder();
    GearmanProtobufClientBuilder(int threadCnt);
    GearmanProtobufClientBuilder(const AsioServicePoolPtr& ioServicePool);
    GearmanProtobufClientBuilder(const AsioServicePtr& ioService);
    virtual ~GearmanProtobufClientBuilder();

private:
    void init();

private:
    ChannelPipelinePtr pipeline;
};

}
}
}
}

#endif //#if !defined(CETTY_GEARMAN_PROTOBUF_GEARMANPROTOBUFCLIENTBUILDER_H)

// Local Variables:
// mode: c++
// End:
