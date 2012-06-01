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

#include <cetty/channel/ChannelState.h>

namespace cetty {
namespace channel {

const ChannelState ChannelState::OPEN = 0;
const ChannelState ChannelState::BOUND = 1;
const ChannelState ChannelState::CONNECTED = 2;
const ChannelState ChannelState::INTEREST_OPS = 3;

std::string ChannelState::toString() const {
    switch (m_value) {
    case 0:
        return "OPEN";

    case 1:
        return "BOUND";

    case 2:
        return "CONNECTED";

    case 3:
        return "INTEREST_OPS";

    default:
        return "UNKNOWN STATE";
    }
}

}
}