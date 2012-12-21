
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

#include <cetty/service/pool/WatermarkConnectionPool.h>

#include <boost/bind.hpp>
#include <cetty/channel/ChannelFuture.h>

namespace cetty {
namespace service {
namespace pool {


WatermarkConnectionPool::WatermarkConnectionPool(const Connections& connections)
    : ConnectionPool(connections),
      lowWatermark_(1),
      highWatermark_(0xffff) {
}

WatermarkConnectionPool::WatermarkConnectionPool(const Connections& connections,
        int lowWatermark,
        int highWatermark)
    : ConnectionPool(connections),
      lowWatermark_(lowWatermark),
      highWatermark_(highWatermark) {
}

WatermarkConnectionPool::~WatermarkConnectionPool() {

}

void WatermarkConnectionPool::start() {
    ChannelFuturePtr future =
        bootstrap_.connect(connections_[0].host, connections_[0].port);

    future->addListener(boost::bind(
                            &ConnectionPool::connectedCallback, this, _1));
}

}
}
}
