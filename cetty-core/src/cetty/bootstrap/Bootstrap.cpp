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
#include <cetty/channel/Channels.h>
#include <cetty/channel/ChannelHandler.h>
#include <cetty/channel/ChannelFactory.h>
#include <cetty/channel/ChannelPipeline.h>
#include <cetty/channel/ChannelPipelineFactory.h>
#include <cetty/util/Exception.h>

#include <cetty/logging/LoggerHelper.h>
#include <cetty/logging/InternalLoggerFactory.h>

namespace cetty {
namespace bootstrap {

using namespace cetty::channel;
using namespace cetty::util;
using namespace cetty::logging;

InternalLogger* Bootstrap::logger = NULL;

Bootstrap::Bootstrap()
    : externalSetPipelineFactory(false), pipeline(NULL), factory(NULL) {

    if (NULL == logger) {
        logger = InternalLoggerFactory::getInstance("Bootstrap");
    }
}

Bootstrap::Bootstrap(const ChannelFactoryPtr& channelFactory)
    : externalSetPipelineFactory(false), pipeline(NULL), factory(NULL) {
    setFactory(channelFactory);

    if (NULL == logger) {
        logger = InternalLoggerFactory::getInstance("Bootstrap");
    }
}

Bootstrap::~Bootstrap() {
}

const ChannelFactoryPtr& Bootstrap::getFactory() {
    if (!factory) {
        LOG_WARN(logger, "getFactory, but has not set yet.");
    }

    return factory;
}

void Bootstrap::setFactory(const ChannelFactoryPtr& factory) {
    if (factory) {
        if (this->factory) {
            LOG_WARN(logger, "setFactory, override the orignial channel facotry.");
        }

        this->factory = factory;
    }
    else {
        LOG_WARN(logger, "setFactory, but the factory is NULL, do nothing.");
    }
}

const ChannelPipelinePtr& Bootstrap::getPipeline() {
    if (!pipeline) {
        if (externalSetPipelineFactory) {
            LOG_WARN(logger, "pipelineFactory has already set, NULL pipeline returned.");
        }
        else {
            LOG_WARN(logger, "pipeline has not set yet.");
        }
    }

    return this->pipeline;
}

void Bootstrap::setPipeline(const ChannelPipelinePtr& pipeline) {
    if (!pipeline) {
        LOG_WARN(logger, "the pipeline set is NULL, do nothing.");
        return;
    }

    if (externalSetPipelineFactory) {
        LOG_WARN(logger, "pipelineFactory has already set, do nothing.");
        return;
    }

    this->pipeline = pipeline;
    pipelineFactory = Channels::pipelineFactory(pipeline);
    externalSetPipelineFactory = false;
}

void Bootstrap::setPipelineFactory(const ChannelPipelineFactoryPtr& pipelineFactory) {
    if (pipelineFactory) {
        this->pipelineFactory = pipelineFactory;
        externalSetPipelineFactory = true;
    }
    else {
        LOG_WARN(logger, "pipelineFactory set is NULL, do nothing.");
    }
}

boost::any Bootstrap::getOption(const std::string& key) const {
    std::map<std::string, boost::any>::const_iterator itr = options.find(key);

    if (itr == options.end()) {
        LOG_WARN(logger, "can not get the option of %s.", key.c_str());
        return boost::any();
    }

    return itr->second;
}

void Bootstrap::setOption(const std::string& key, const boost::any& value) {
    if (value.empty()) {
        options.erase(key);
        LOG_WARN(logger, "setOption, the key (%s) is empty value, remove from the options.", key.c_str());
    }
    else {
        LOG_INFO(logger, "set Option, the key is %s.", key.c_str());
        options.insert(std::make_pair(key, value));
    }
}

void Bootstrap::releaseExternalResources() {
    if (factory) {
        factory->releaseExternalResources();
    }
}

const ChannelPipelineFactoryPtr& Bootstrap::getPipelineFactory() {
    return this->pipelineFactory;
}

const std::map<std::string, boost::any>& Bootstrap::getOptions() const {
    return this->options;
}

std::map<std::string, boost::any>& Bootstrap::getOptions() {
    return this->options;
}

void Bootstrap::setOptions(const std::map<std::string, boost::any>& options) {
    LOG_INFO(logger, "set options using map, will reset the original options.");
    this->options = options;
}

}
}
