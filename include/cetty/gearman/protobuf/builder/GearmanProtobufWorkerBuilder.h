#if !defined(CETTY_GEARMAN_PROTOBUF_BUILDER_GEARMANPROTOBUFWORKERBUILDER_H)
#define CETTY_GEARMAN_PROTOBUF_BUILDER_GEARMANPROTOBUFWORKERBUILDER_H

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

#include <cetty/gearman/builder/GearmanWorkerBuilder.h>
#include <cetty/protobuf/service/ProtobufServicePtr.h>

namespace cetty {
namespace gearman {
namespace protobuf {
namespace builder {

using namespace cetty::gearman::builder;
using namespace cetty::protobuf::service;

class GearmanProtobufWorkerBuilder : private boost::noncopyable {
public:
    typedef GearmanWorkerBuilder::WorkFunctor WorkFunctor;

public:
    GearmanProtobufWorkerBuilder();
    GearmanProtobufWorkerBuilder(int threadCnt);
    GearmanProtobufWorkerBuilder(const EventLoopPtr& eventLoop);
    GearmanProtobufWorkerBuilder(const EventLoopPoolPtr& eventLoopPool);

    GearmanProtobufWorkerBuilder& addConnection(const std::string& host, int port);
    GearmanProtobufWorkerBuilder& registerService(const ProtobufServicePtr& service);

    GearmanProtobufWorkerBuilder& buildWorkers();

    void waitingForExit();

private:
    GearmanWorkerBuilder builder_;
};

inline
GearmanProtobufWorkerBuilder& GearmanProtobufWorkerBuilder::addConnection(
    const std::string& host, int port) {
    builder_.addConnection(host, port);
    return *this;
}

inline
GearmanProtobufWorkerBuilder& GearmanProtobufWorkerBuilder::buildWorkers() {
    builder_.buildWorkers();
    return *this;
}

inline
void GearmanProtobufWorkerBuilder::waitingForExit() {
    builder_.waitingForExit();
}

}
}
}
}

#endif //#if !defined(CETTY_GEARMAN_PROTOBUF_BUILDER_GEARMANPROTOBUFWORKERBUILDER_H)

// Local Variables:
// mode: c++
// End:
