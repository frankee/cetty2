#if !defined(CETTY_GEARMAN_PROTOCOL_COMMANDS_CLIENT_H)
#define CETTY_GEARMAN_PROTOCOL_COMMANDS_CLIENT_H

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

#include <cetty/buffer/ChannelBufferPtr.h>
#include <cetty/gearman/protocol/GearmanMessagePtr.h>

namespace cetty {
namespace gearman {
namespace protocol {
namespace commands {

using namespace cetty::buffer;

// client
static GearmanMessagePtr submitJobMessage(const std::string& functionName, const std::string& uniqueId, const ChannelBufferPtr& payload);
static GearmanMessagePtr submitJobHighMessage(const std::string& functionName, const std::string& uniqueId, const ChannelBufferPtr& payload);
static GearmanMessagePtr submitJobLowMessage(const std::string& functionName, const std::string& uniqueId, const ChannelBufferPtr& payload);

//client submit-bg
static GearmanMessagePtr submitJobBGMessage(const std::string& functionName, const std::string& uniqueId, const ChannelBufferPtr& payload);
static GearmanMessagePtr submitJobHighBGMessage(const std::string& functionName, const std::string& uniqueId, const ChannelBufferPtr& payload);
static GearmanMessagePtr submitJobLowBGMessage(const std::string& functionName, const std::string& uniqueId, const ChannelBufferPtr& payload);

static GearmanMessagePtr getStatusMessage(const std::string& jobHandle);
static GearmanMessagePtr setOptionMessage(const std::string& option);

}
}
}
}

#endif //#if !defined(CETTY_GEARMAN_PROTOCOL_COMMANDS_CLIENT_H)

// Local Variables:
// mode: c++
// End:
