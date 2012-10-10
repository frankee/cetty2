/*
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

#include <cetty/channel/ChannelPipelines.h>

#include <boost/assert.hpp>

#include <cetty/util/Exception.h>
#include <cetty/util/StringUtil.h>

#include <cetty/channel/Channel.h>
#include <cetty/channel/ChannelPipelineFactory.h>
#include <cetty/channel/ChannelPipeline.h>

namespace cetty {
namespace channel {

using namespace cetty::util;

class CloneChannelPipelineFactory : public ChannelPipelineFactory {
public:
    CloneChannelPipelineFactory(const ChannelPipelinePtr pipeline)
        : cloneFromPipeline(pipeline) {
    }
    virtual ~CloneChannelPipelineFactory() {}

    ChannelPipelinePtr getPipeline() {
        return ChannelPipelines::pipeline(cloneFromPipeline);
    }

private:
    ChannelPipelinePtr cloneFromPipeline;
};

ChannelPipelinePtr ChannelPipelines::pipeline() {
    return new ChannelPipeline();
}

ChannelPipelinePtr ChannelPipelines::pipeline(const ChannelPipelinePtr& pipeline) {
    ChannelPipelinePtr newPipeline = ChannelPipelines::pipeline();
    BOOST_ASSERT(pipeline && "the input pipe should not be NULL");

    ChannelPipeline::ChannelHandlers handlers = pipeline->getChannelHandles();
    ChannelPipeline::ChannelHandlers::iterator itr = handlers.begin();

    for (; itr != handlers.end(); ++itr) {
        newPipeline->addLast(itr->first, itr->second->clone());
    }

    return newPipeline;
}

ChannelPipelinePtr ChannelPipelines::pipeline(const ChannelHandlerPtr& handler) {
    ChannelPipelinePtr newPipeline = pipeline();

    if (handler) {
        newPipeline->addLast("0", handler);
    }

    return newPipeline;
}

ChannelPipelinePtr ChannelPipelines::pipeline(const ChannelHandlerPtr& handler0,
                                      const ChannelHandlerPtr& handler1) {
    ChannelPipelinePtr newPipeline = pipeline();

    if (handler0) { newPipeline->addLast("0", handler0); }

    if (handler1) { newPipeline->addLast("1", handler1); }

    return newPipeline;
}

ChannelPipelinePtr ChannelPipelines::pipeline(const ChannelHandlerPtr& handler0,
                                      const ChannelHandlerPtr& handler1,
                                      const ChannelHandlerPtr& handler2) {
    ChannelPipelinePtr newPipeline = pipeline();

    if (handler0) { newPipeline->addLast("0", handler0); }

    if (handler1) { newPipeline->addLast("1", handler1); }

    if (handler1) { newPipeline->addLast("2", handler2); }

    return newPipeline;
}

ChannelPipelinePtr ChannelPipelines::pipeline(const ChannelHandlerPtr& handler0,
                                      const ChannelHandlerPtr& handler1,
                                      const ChannelHandlerPtr& handler2,
                                      const ChannelHandlerPtr& handler3) {
    ChannelPipelinePtr newPipeline = pipeline();

    if (handler0) { newPipeline->addLast("0", handler0); }

    if (handler1) { newPipeline->addLast("1", handler1); }

    if (handler1) { newPipeline->addLast("2", handler2); }

    if (handler1) { newPipeline->addLast("3", handler3); }

    return newPipeline;
}

ChannelPipelinePtr ChannelPipelines::pipeline(const std::vector<ChannelHandlerPtr>& handlers) {
    ChannelPipelinePtr newPipeline = ChannelPipelines::pipeline();
    BOOST_ASSERT(newPipeline);

    for (size_t i = 0; i < handlers.size(); ++i) {
        const ChannelHandlerPtr& h = handlers[i];

        if (!h) {
            continue;
        }

        newPipeline->addLast(StringUtil::strprintf("%d", i), handlers[i]);
    }

    return newPipeline;
}

}
}
