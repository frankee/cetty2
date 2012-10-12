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

#include <cetty/gearman/builder/GearmanClientBuilder.h>

#include <cetty/channel/ChannelPipelines.h>
#include <cetty/channel/ChannelPipeline.h>
#include <cetty/handler/codec/LengthFieldBasedFrameDecoder.h>
#include <cetty/gearman/GearmanDecoder.h>
#include <cetty/gearman/GearmanEncoder.h>
#include <cetty/gearman/GearmanClientHandler.h>

namespace cetty {
namespace gearman {
namespace builder {

using namespace cetty::channel;
using namespace cetty::handler::codec;
using namespace cetty::gearman;

GearmanClientBuilder::GearmanClientBuilder()
    : ClientBuilderType() {
    init();
}

GearmanClientBuilder::GearmanClientBuilder(int threadCnt)
    : ClientBuilderType(threadCnt) {
    init();
}

GearmanClientBuilder::GearmanClientBuilder(const EventLoopPoolPtr& eventLoopPool)
    : ClientBuilderType(eventLoopPool) {
    init();
}

GearmanClientBuilder::GearmanClientBuilder(const EventLoopPtr& eventLoop)
    :  ClientBuilderType(eventLoop) {
    init();
}

void GearmanClientBuilder::init() {
    pipeline = ChannelPipelines::pipeline();

    pipeline->addLast("frameDecoder", new LengthFieldBasedFrameDecoder(16 * 1024 * 1024, 8, 4, 0, 4));

    pipeline->addLast("gearmanDecoder", new GearmanDecoder());
    pipeline->addLast("gearmanEncoder", new GearmanEncoder());
    pipeline->addLast("gearmanClient", new GearmanClientHandler());

    ClientBuilderType::setPipeline(pipeline);
}

}
}
}
