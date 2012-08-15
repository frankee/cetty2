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

#include <cetty/bootstrap/Bootstrap.h>
#include <cetty/channel/Channel.h>
#include <cetty/channel/ChannelPipeline.h>
#include <cetty/channel/ChannelPipelines.h>
#include <cetty/channel/ChannelHandler.h>
#include <cetty/channel/ChannelFactory.h>
#include <cetty/channel/ChannelPipeline.h>
#include <cetty/channel/ChannelPipelineFactory.h>
#include <cetty/util/Exception.h>

#include <cetty/logging/LoggerHelper.h>

namespace cetty {
namespace bootstrap {

using namespace cetty::channel;
using namespace cetty::util;
using namespace cetty::logging;

Bootstrap::Bootstrap() {
}

Bootstrap::Bootstrap(const ChannelFactoryPtr& channelFactory) {
    setFactory(channelFactory);
}

Bootstrap::~Bootstrap() {
}

const ChannelFactoryPtr& Bootstrap::getFactory() {
    if (!factory) {
        LOG_WARN() << "getFactory, but has not set yet.";
    }

    return factory;
}

Bootstrap& Bootstrap::setFactory(const ChannelFactoryPtr& factory) {
    if (factory) {
        if (this->factory) {
            LOG_WARN() << "setFactory, override the orignial channel facotry.";
        }

        this->factory = factory;
    }
    else {
        LOG_WARN() << "setFactory, but the factory is NULL, do nothing.";
    }

    return *this;
}

const ChannelPipelinePtr& Bootstrap::getPipeline() {
    if (!pipeline) {
        LOG_WARN() << "pipeline has not set yet.";
    }

    return this->pipeline;
}

Bootstrap& Bootstrap::setPipeline(const ChannelPipelinePtr& pipeline) {
    if (!pipeline) {
        LOG_WARN() << "the pipeline set is NULL, do nothing.";
        return *this;
    }

    if (this->pipeline) {
        LOG_WARN() << "pipeline has already set, will override.";
    }

    this->pipeline = pipeline;

    return *this;
}

ChannelOption::Variant Bootstrap::getOption(const ChannelOption& option) const {

    ChannelOption::Options::const_iterator itr = options.find(option);

    if (itr == options.end()) {
        LOG_WARN() << "can not get the option of " << option.getName();
        return ChannelOption::Variant();
    }

    return itr->second;
}

Bootstrap& Bootstrap::setOption(const ChannelOption& option,
                          const ChannelOption::Variant& value) {
    if (value.empty()) {
        options.erase(option);
        LOG_WARN() << "setOption, the key ("
            << option.getName()
            << ") is empty value, remove from the options.";
    }
    else {
        LOG_DEBUG() << "set Option, the key is " << option.getName();
        options.insert(std::make_pair(option, value));
    }

    return *this;
}

const ChannelOption::Options& Bootstrap::getOptions() const {
    return this->options;
}

ChannelOption::Options& Bootstrap::getOptions() {
    return this->options;
}

Bootstrap& Bootstrap::setOptions(const ChannelOption::Options& options) {
    LOG_INFO() << "set options using map, will reset the original options.";
    this->options = options;

    return *this;
}

void Bootstrap::shutdown() {
    if (factory) {
        factory->shutdown();
    }
}


}
}
