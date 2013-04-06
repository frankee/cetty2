
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

#include <cetty/channel/ChannelOption.h>

namespace cetty {
namespace channel {

class SctpInitMaxStreamsChecker : public boost::static_visitor<bool> {
public:
    bool operator()(const std::vector<int>& value) const {
        return value.size() == 2;
    }

    template<typename T>
    bool operator()(const T& vlaue) {
        return false;
    }
};

const ValueChecker<int> ChannelOption::INT_VALUE_CHECKER;
const ValueChecker<bool> ChannelOption::BOOL_VALUE_CHECKER;
const ValueChecker<std::string> ChannelOption::STRING_VALUE_CHECKER;
const ValueChecker<std::vector<int> > ChannelOption::INT_VECTOR_VALUE_CHECKER;

static const SctpInitMaxStreamsChecker SCTP_INIT_MAXSTREAMS_CHECKER;

const ChannelOption ChannelOption::CO_CONNECT_TIMEOUT_MILLIS("CONNECT_TIMEOUT_MILLIS", &INT_VALUE_CHECKER);
const ChannelOption ChannelOption::CO_RESERVED_CHILD_COUNT("RESERVED_CHILD_COUNT", &INT_VALUE_CHECKER);

const ChannelOption  ChannelOption::CO_SO_BROADCAST("SO_BROADCAST", &BOOL_VALUE_CHECKER);
const ChannelOption  ChannelOption::CO_SO_KEEPALIVE("SO_KEEPALIVE", &BOOL_VALUE_CHECKER);
const ChannelOption  ChannelOption::CO_SO_SNDBUF("SO_SNDBUF", &INT_VALUE_CHECKER);
const ChannelOption  ChannelOption::CO_SO_RCVBUF("SO_RCVBUF", &INT_VALUE_CHECKER);
const ChannelOption  ChannelOption::CO_SO_REUSEADDR("SO_REUSEADDR", &BOOL_VALUE_CHECKER);
const ChannelOption  ChannelOption::CO_SO_LINGER("SO_LINGER", &INT_VALUE_CHECKER);
const ChannelOption  ChannelOption::CO_SO_BACKLOG("SO_BACKLOG", &INT_VALUE_CHECKER);
const ChannelOption ChannelOption::CO_SO_SNDLOWAT("SO_SNDLOWAT", &INT_VALUE_CHECKER);
const ChannelOption ChannelOption::CO_SO_RCVLOWAT("SO_RCVLOWAT", &INT_VALUE_CHECKER);

const ChannelOption ChannelOption::CO_SNDHIGHWAT("SNDHIGHWAT", &INT_VALUE_CHECKER);
const ChannelOption ChannelOption::CO_RCVHIGHWAT("RCVHIGHWAT", &INT_VALUE_CHECKER);

const ChannelOption  ChannelOption::CO_IP_TOS("IP_TOS", &INT_VALUE_CHECKER);
const ChannelOption  ChannelOption::CO_IP_MULTICAST_ADDR("IP_MULTICAST_ADDR", &STRING_VALUE_CHECKER);
const ChannelOption  ChannelOption::CO_IP_MULTICAST_IF("IP_MULTICAST_IF", &STRING_VALUE_CHECKER);
const ChannelOption  ChannelOption::CO_IP_MULTICAST_TTL("IP_MULTICAST_TTL", &INT_VALUE_CHECKER);
const ChannelOption  ChannelOption::CO_IP_MULTICAST_LOOP_DISABLED("IP_MULTICAST_LOOP_DISABLED", &BOOL_VALUE_CHECKER);
const ChannelOption  ChannelOption::CO_UDP_RECEIVE_PACKET_SIZE("UDP_RECEIVE_PACKET_SIZE", &INT_VALUE_CHECKER);

const ChannelOption  ChannelOption::CO_TCP_NODELAY("TCP_NODELAY", &BOOL_VALUE_CHECKER);

const ChannelOption  ChannelOption::CO_SCTP_DISABLE_FRAGMENTS("SCTP_DISABLE_FRAGMENTS", &BOOL_VALUE_CHECKER);
const ChannelOption  ChannelOption::CO_SCTP_EXPLICIT_COMPLETE("SCTP_EXPLICIT_COMPLETE", &BOOL_VALUE_CHECKER);
const ChannelOption  ChannelOption::CO_SCTP_FRAGMENT_INTERLEAVE("SCTP_FRAGMENT_INTERLEAVE", &INT_VALUE_CHECKER);
const ChannelOption  ChannelOption::CO_SCTP_INIT_MAXSTREAMS("SCTP_INIT_MAXSTREAMS", &SCTP_INIT_MAXSTREAMS_CHECKER);
const ChannelOption  ChannelOption::CO_SCTP_NODELAY("SCTP_NODELAY", &BOOL_VALUE_CHECKER);
const ChannelOption  ChannelOption::CO_SCTP_PRIMARY_ADDR("SCTP_PRIMARY_ADDR", &STRING_VALUE_CHECKER);
const ChannelOption  ChannelOption::CO_SCTP_SET_PEER_PRIMARY_ADDR("SCTP_SET_PEER_PRIMARY_ADDR", &STRING_VALUE_CHECKER);

bool ChannelOption::validate(const ChannelOption::Variant& value) const {
    if (checker_) {
        //return boost::apply_visitor(*checker, value);
    }

    return true;
}

}
}
