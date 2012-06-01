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

#include <cetty/channel/DefaultServerChannelConfig.h>
#include <cetty/buffer/HeapChannelBufferFactory.h>
#include <cetty/util/Exception.h>

namespace cetty {
namespace channel {

using namespace cetty::util;
using namespace cetty::buffer;

DefaultServerChannelConfig::DefaultServerChannelConfig()
    : bufferFactory(&(HeapChannelBufferFactory::getInstance())) {
}

void DefaultServerChannelConfig::setOptions(const std::map<std::string, boost::any>& options) {
    std::map<std::string, boost::any>::const_iterator itr;
    for (itr = options.begin(); itr != options.end(); ++itr) {
        setOption(itr->first, itr->second);
    }
}

bool DefaultServerChannelConfig::setOption(const std::string& key, const boost::any& value) {
    try {
        if (key.compare("pipelineFactory") == 0) {
            setPipelineFactory(boost::any_cast<ChannelPipelineFactoryPtr>(value));
        }
        else if (key.compare("bufferFactory") == 0) {
            setBufferFactory(boost::any_cast<ChannelBufferFactoryPtr>(value));
        }
        else {
            return false;
        }

        return true;
    }
    catch (const std::exception& e) {
        throw InvalidArgumentException(e.what());
    }
}

const ChannelPipelineFactoryPtr& DefaultServerChannelConfig::getPipelineFactory() const {
    return this->pipelineFactory;
}

void DefaultServerChannelConfig::setPipelineFactory(const ChannelPipelineFactoryPtr& pipelineFactory) {
    if (pipelineFactory) {
        this->pipelineFactory = pipelineFactory;
    }
    else {
        throw NullPointerException("pipelineFactory");
    }
}

const ChannelBufferFactoryPtr& DefaultServerChannelConfig::getBufferFactory() const {
    return this->bufferFactory;
}

void DefaultServerChannelConfig::setBufferFactory(const ChannelBufferFactoryPtr& bufferFactory) {
    if (!bufferFactory) {
        throw NullPointerException("bufferFactory");
    }

    this->bufferFactory = bufferFactory;
}

int DefaultServerChannelConfig::getConnectTimeout() const {
    return 0;
}

void DefaultServerChannelConfig::setConnectTimeout(int connectTimeoutMillis) {
}

bool DefaultServerChannelConfig::channelHasReaderBuffer() const {
    return false;
}

}
}