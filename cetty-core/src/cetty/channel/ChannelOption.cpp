
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

const ChannelOption::Variant ChannelOption::EMPTY_VALUE;

const ChannelOption ChannelOption::CO_CONNECT_TIMEOUT_MILLIS(1, "CONNECT_TIMEOUT_MILLIS", &INT_VALUE_CHECKER);
const ChannelOption ChannelOption::CO_REUSE_CHILD(2, "REUSE_CHILD", &BOOL_VALUE_CHECKER);
const ChannelOption ChannelOption::CO_RESERVED_CHILD_COUNT(3, "RESERVED_CHILD_COUNT", &INT_VALUE_CHECKER);
const ChannelOption ChannelOption::CO_AUTO_READ(4, "AUTO_READ", &BOOL_VALUE_CHECKER);

const ChannelOption ChannelOption::CO_SO_BROADCAST(10, "SO_BROADCAST", &BOOL_VALUE_CHECKER);
const ChannelOption ChannelOption::CO_SO_KEEPALIVE(11, "SO_KEEPALIVE", &BOOL_VALUE_CHECKER);
const ChannelOption ChannelOption::CO_SO_SNDBUF(12, "SO_SNDBUF", &INT_VALUE_CHECKER);
const ChannelOption ChannelOption::CO_SO_RCVBUF(13, "SO_RCVBUF", &INT_VALUE_CHECKER);
const ChannelOption ChannelOption::CO_SO_REUSEADDR(14, "SO_REUSEADDR", &BOOL_VALUE_CHECKER);
const ChannelOption ChannelOption::CO_SO_LINGER(15, "SO_LINGER", &INT_VALUE_CHECKER);
const ChannelOption ChannelOption::CO_SO_BACKLOG(16, "SO_BACKLOG", &INT_VALUE_CHECKER);
const ChannelOption ChannelOption::CO_SO_SNDLOWAT(17, "SO_SNDLOWAT", &INT_VALUE_CHECKER);
const ChannelOption ChannelOption::CO_SO_RCVLOWAT(18, "SO_RCVLOWAT", &INT_VALUE_CHECKER);

const ChannelOption ChannelOption::CO_SNDHIGHWAT(19, "SNDHIGHWAT", &INT_VALUE_CHECKER);
const ChannelOption ChannelOption::CO_RCVHIGHWAT(20, "RCVHIGHWAT", &INT_VALUE_CHECKER);

const ChannelOption ChannelOption::CO_IP_TOS(30, "IP_TOS", &INT_VALUE_CHECKER);
const ChannelOption ChannelOption::CO_IP_MULTICAST_ADDR(31, "IP_MULTICAST_ADDR", &STRING_VALUE_CHECKER);
const ChannelOption ChannelOption::CO_IP_MULTICAST_IF(32, "IP_MULTICAST_IF", &STRING_VALUE_CHECKER);
const ChannelOption ChannelOption::CO_IP_MULTICAST_TTL(33, "IP_MULTICAST_TTL", &INT_VALUE_CHECKER);
const ChannelOption ChannelOption::CO_IP_MULTICAST_LOOP_DISABLED(34, "IP_MULTICAST_LOOP_DISABLED", &BOOL_VALUE_CHECKER);
const ChannelOption ChannelOption::CO_UDP_RECEIVE_PACKET_SIZE(40, "UDP_RECEIVE_PACKET_SIZE", &INT_VALUE_CHECKER);

const ChannelOption ChannelOption::CO_TCP_NODELAY(50, "TCP_NODELAY", &BOOL_VALUE_CHECKER);

const ChannelOption ChannelOption::CO_SCTP_DISABLE_FRAGMENTS(60, "SCTP_DISABLE_FRAGMENTS", &BOOL_VALUE_CHECKER);
const ChannelOption ChannelOption::CO_SCTP_EXPLICIT_COMPLETE(61, "SCTP_EXPLICIT_COMPLETE", &BOOL_VALUE_CHECKER);
const ChannelOption ChannelOption::CO_SCTP_FRAGMENT_INTERLEAVE(62, "SCTP_FRAGMENT_INTERLEAVE", &INT_VALUE_CHECKER);
const ChannelOption ChannelOption::CO_SCTP_INIT_MAXSTREAMS(63, "SCTP_INIT_MAXSTREAMS", &SCTP_INIT_MAXSTREAMS_CHECKER);
const ChannelOption ChannelOption::CO_SCTP_NODELAY(64, "SCTP_NODELAY", &BOOL_VALUE_CHECKER);
const ChannelOption ChannelOption::CO_SCTP_PRIMARY_ADDR(65, "SCTP_PRIMARY_ADDR", &STRING_VALUE_CHECKER);
const ChannelOption ChannelOption::CO_SCTP_SET_PEER_PRIMARY_ADDR(66, "SCTP_SET_PEER_PRIMARY_ADDR", &STRING_VALUE_CHECKER);

ChannelOption::ChannelOption(int id,
                             const char* name,
                             const boost::static_visitor<bool>* checker)
    : cetty::util::Enum<ChannelOption>(id, name),
      checker_(checker) {
    if (!defaultEnum()) {
        setDefaultEnum(new ChannelOption(-1, "unknown", NULL));
    }
}

bool ChannelOption::validate(const ChannelOption::Variant& value) const {
    if (checker_) {
        //return boost::apply_visitor(*checker, value);
    }

    return true;
}

}
}
