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

#include <cetty/channel/DefaultChannelConfig.h>
#include <cetty/buffer/ChannelBufferFactory.h>
#include <cetty/buffer/HeapChannelBufferFactory.h>
#include <cetty/channel/ChannelPipelineFactory.h>
#include <cetty/util/Exception.h>

namespace cetty {
namespace channel {

using namespace cetty::buffer;
using namespace cetty::util;

DefaultChannelConfig::DefaultChannelConfig()
    : connectTimeoutMillis(10000/*ms*/),
      bufferFactory(&(HeapChannelBufferFactory::getInstance())) {
}

void DefaultChannelConfig::setOptions(const std::map<std::string, boost::any>& options) {
    std::map<std::string, boost::any>::const_iterator itr =
        options.begin();

    while (itr != options.end()) {
        setOption(itr->first, itr->second);
        ++itr;
    }
}

bool DefaultChannelConfig::setOption(const std::string& key, const boost::any& value) {
    try {
        if (key == "pipelineFactory") {
            setPipelineFactory(boost::any_cast<ChannelPipelineFactoryPtr>(value));
        }
        else if (key == "connectTimeoutMillis") {
            setConnectTimeout(boost::any_cast<int>(value));
        }
        else if (key == "bufferFactory") {
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

void DefaultChannelConfig::setBufferFactory(const ChannelBufferFactoryPtr& bufferFactory) {
    if (!bufferFactory) {
        throw NullPointerException("bufferFactory");
    }

    this->bufferFactory = bufferFactory;
}

void DefaultChannelConfig::setConnectTimeout(int connectTimeoutMillis) {
    if (connectTimeoutMillis < 0) {
        throw InvalidArgumentException("connectTimeoutMillis is negative.");
    }

    this->connectTimeoutMillis = connectTimeoutMillis;
}

const ChannelBufferFactoryPtr& DefaultChannelConfig::getBufferFactory() const {
    return bufferFactory;
}

const ChannelPipelineFactoryPtr& DefaultChannelConfig::getPipelineFactory() const {
    return pipelineFactory;
}

void DefaultChannelConfig::setPipelineFactory(const ChannelPipelineFactoryPtr& pipelineFactory) {
    // Unused
}

int DefaultChannelConfig::getConnectTimeout() const {
    return this->connectTimeoutMillis;
}

bool DefaultChannelConfig::channelHasReaderBuffer() const {
    return true;
}

}
}