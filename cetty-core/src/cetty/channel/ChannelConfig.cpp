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

#include <cetty/channel/ChannelConfig.h>
#include <cetty/util/Exception.h>

#include <cetty/logging/LoggerHelper.h>

namespace cetty {
namespace channel {

using namespace cetty::util;

ChannelConfig::ChannelConfig()
    : autoRead_(true),
      connectTimeoutMillis_(10000/*ms*/) {
}

ChannelConfig::~ChannelConfig() {
}

void ChannelConfig::setOptions(const ChannelOptions& options) {
    ChannelOptions::ConstIterator itr = options.begin();

    for (; itr != options.end(); ++itr) {
        setOption(itr->first, itr->second);
    }
}

bool ChannelConfig::setOption(const ChannelOption& option,
                              const ChannelOption::Variant& value) {
    try {
        if (!callback_ || !callback_(option, value)) {
            if (option == ChannelOption::CO_CONNECT_TIMEOUT_MILLIS) {
                setConnectTimeout(boost::get<int>(value));
                return true;
            }
            else if (option == ChannelOption::CO_AUTO_READ) {
                setAutoRead(boost::get<bool>(value));
                return true;
            }
        }

        return false;
    }
    catch (const std::exception& e) {
        LOG_ERROR << "failed to set the value of " << option.name()
                  << ", message: " << e.what();
        return false;
    }
}

ChannelConfig& ChannelConfig::setConnectTimeout(int connectTimeoutMillis) {
    if (connectTimeoutMillis > 0) {
        connectTimeoutMillis_ = connectTimeoutMillis;
    }
    else {
        LOG_WARN << "connectTimeoutMillis is negative, using default.";
    }

    return *this;
}

}
}
