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
    : connectTimeoutMillis(10000/*ms*/) {
}

void DefaultChannelConfig::setOptions(const ChannelOption::Options& options) {
    ChannelOption::Options::const_iterator itr = options.begin();

    while (itr != options.end()) {
        setOption(itr->first, itr->second);
        ++itr;
    }
}

bool DefaultChannelConfig::setOption(const ChannelOption& option,
    const ChannelOption::Variant& value) {
    try {
        if (option == ChannelOption::CONNECT_TIMEOUT_MILLIS) {
            setConnectTimeout(boost::get<int>(value));
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

void DefaultChannelConfig::setConnectTimeout(int connectTimeoutMillis) {
    if (connectTimeoutMillis < 0) {
        throw InvalidArgumentException("connectTimeoutMillis is negative.");
    }

    this->connectTimeoutMillis = connectTimeoutMillis;
}

int DefaultChannelConfig::getConnectTimeout() const {
    return this->connectTimeoutMillis;
}

}
}