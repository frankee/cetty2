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

#include <cetty/shuttle/ShuttleBackend.h>

#include <cetty/handler/codec/LengthFieldBasedFrameDecoder.h>
#include <cetty/handler/codec/LengthFieldPrepender.h>
#include <cetty/service/builder/ClientBuilder.h>
#include <cetty/protobuf/service/ProtobufUtil.h>
#include <cetty/protobuf/service/ProtobufServicePtr.h>
#include <cetty/shuttle/protobuf/ProtobufProxyCodec.h>
#include <cetty/shuttle/protobuf/ProtobufProxyMessage.h>

namespace cetty {
namespace shuttle {

using namespace cetty::handler::codec;
using namespace cetty::service::builder;
using namespace cetty::protobuf::service;
using namespace cetty::shuttle::protobuf;

cetty::channel::ChannelPtr ShuttleBackend::emptyChannel_;
std::map<ThreadId, ShuttleBackendPtr> ShuttleBackend::backends_;

bool ShuttleBackend::initializeChannel(ChannelPipeline& pipeline) {
    pipeline.addLast<LengthFieldBasedFrameDecoder::Handler>(
        "frameDecoder",
        LengthFieldBasedFrameDecoder::HandlerPtr(new LengthFieldBasedFrameDecoder(
                    16 * 1024 * 1024,
                    0,
                    4,
                    0,
                    4,
                    4,
                    ProtobufUtil::adler32Checksum)));

    pipeline.addLast<LengthFieldPrepender::Handler>(
        "frameEncoder",
        LengthFieldPrepender::HandlerPtr(new LengthFieldPrepender(
                4,
                4,
                ProtobufUtil::adler32Checksum)));

    pipeline.addLast<ProtobufProxyCodec::Handler>(
        "protobufCodec",
        ProtobufProxyCodec::HandlerPtr(new ProtobufProxyCodec));

    return true;
}

void ShuttleBackend::configure(const std::map<std::string, Backend*>& config, const EventLoopPoolPtr& pool) {
    EventLoopPool::Iterator itr = pool->begin();

    for (; itr != pool->end(); ++itr) {
        ShuttleBackendPtr backend = ShuttleBackendPtr(new ShuttleBackend(*itr));
        backend->configure(config);
        backends_.insert(std::make_pair((*itr)->threadId(), backend));
    }
}

void ShuttleBackend::configure(const std::map<std::string, Backend*>& config) {
    std::map<std::string, Backend*>::const_iterator itr;

    for (itr = config.begin(); itr != config.end(); ++itr) {
        channels_.insert(std::make_pair(itr->first,
                                        builder_.build(itr->second->servers)));
    }
}


const ChannelPtr& ShuttleBackend::getChannel(const std::string& method) {
    std::map<ThreadId, ShuttleBackendPtr>::const_iterator itr =
        backends_.find(CurrentThread::id());

    if (itr != backends_.end()) {
        return itr->second->channel(method);
    }
    else {
        return emptyChannel_;
    }
}

const ChannelPtr& ShuttleBackend::channel(const std::string& method) const {
    std::map<std::string, ChannelPtr>::const_iterator itr =
        channels_.find(method);

    if (itr != channels_.end()) {
        return itr->second;
    }

    return emptyChannel_;
}

ShuttleBackend::ShuttleBackend(const EventLoopPtr& loop)
    : builder_(loop) {
    init();
}

void ShuttleBackend::init() {
    builder_.setClientInitializer(
        boost::bind(&ShuttleBackend::initializeChannel, this, _1));
}

}
}
