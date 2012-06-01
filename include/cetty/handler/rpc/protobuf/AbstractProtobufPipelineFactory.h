#if !defined(CETTY_HANDLER_RPC_PROTOBUF_ABSTRACTPROTOBUFPIPELINEFACTORY_H)
#define CETTY_HANDLER_RPC_PROTOBUF_ABSTRACTPROTOBUFPIPELINEFACTORY_H

/**
 * Copyright (c) 2010-2011 frankee zhou (frankee.zhou at gmail dot com)
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

#include <map>
#include <string>
#include "cetty/channel/ChannelPipelineFactory.h"
#include "cetty/handler/rpc/protobuf/ProtobufServiceRegister.h"

namespace cetty {
namespace handler {
namespace rpc {
namespace protobuf {

using namespace cetty::channel;

class AbstractProtobufPipelineFactory : public ChannelPipelineFactory {
public:
    virtual ~AbstractProtobufPipelineFactory() {}

    int registerService(ProtobufServicePtr& service) {
        return servicesRegister.registerService(service);
    }

    void unregisterService(const std::string& name) {
        servicesRegister.unregisterService(name);
    }

protected:
    ProtobufServiceRegister servicesRegister;
};

}
}
}
}

#endif //#if !defined(CETTY_HANDLER_RPC_PROTOBUF_ABSTRACTPROTOBUFPIPELINEFACTORY_H)

// Local Variables:
// mode: c++
// End:

