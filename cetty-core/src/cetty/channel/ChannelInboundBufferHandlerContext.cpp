
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

#include <cetty/channel/ChannelInboundBufferHandlerContext.h>
#include <boost/bind.hpp>

#include <cetty/channel/Channel.h>
#include <cetty/channel/ChannelInboundBufferHandler.h>

namespace cetty {
namespace channel {

void ChannelInboundBufferHandlerContext::setInboundChannelBuffer(
    const ChannelBufferPtr& buffer) {
    if (eventloop->inLoopThread()) {
        inboundHandler->setInboundChannelBuffer(buffer);
    }
    else {
        eventloop->post(boost::bind(
                            &ChannelInboundBufferHandlerContext::setInboundChannelBuffer,
                            this,
                            buffer));
    }
}

ChannelInboundBufferHandlerContext::ChannelInboundBufferHandlerContext(
    const std::string& name,
    ChannelPipeline& pipeline,
    const ChannelHandlerPtr& handler,
    ChannelHandlerContext* prev,
    ChannelHandlerContext* next)
    : ChannelHandlerContext(name, pipeline, handler, prev, next) {
    hasInboundBufferHandler = true;
    inboundHandler = boost::dynamic_pointer_cast<ChannelInboundBufferHandler>(handler);
    BOOST_ASSERT(inboundHandler);
}

ChannelInboundBufferHandlerContext::ChannelInboundBufferHandlerContext(
    const std::string& name,
    const EventLoopPtr& eventLoop,
    ChannelPipeline& pipeline,
    const ChannelHandlerPtr& handler,
    ChannelHandlerContext* prev,
    ChannelHandlerContext* next)
    : ChannelHandlerContext(name, eventLoop, pipeline, handler, prev, next) {
    hasInboundBufferHandler = true;
    inboundHandler = boost::dynamic_pointer_cast<ChannelInboundBufferHandler>(handler);
    BOOST_ASSERT(inboundHandler);
}

ChannelInboundBufferHandlerContext::~ChannelInboundBufferHandlerContext() {

}

}
}