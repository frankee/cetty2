#if !defined(CETTY_SERVICE_POOL_WATERMARKCONNECTIONPOOL_H)
#define CETTY_SERVICE_POOL_WATERMARKCONNECTIONPOOL_H

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

#include <cetty/service/pool/ConnectionPool.h>

namespace cetty {
namespace service {
namespace pool {

/**
 * The watermark pool is an connection pool with low & high
 * watermarks. It keeps the number of connections from a given service
 * factory in a certain range.
 *
 * This behaves as follows: the pool will persist up to
 * the low watermark number of items (as long as they have been
 * created), and won't start queueing requests until the high
 * watermark has been reached. Put another way: up to `lowWatermark'
 * items may persist indefinitely, while there are at no times more
 * than `highWatermark' items in concurrent existence.
 */
class WatermarkConnectionPool : public ConnectionPool {
public:
    WatermarkConnectionPool(const Connections& connections);
    WatermarkConnectionPool(const Connections& connections,
                            const EventLoopPtr& eventLoop);

    WatermarkConnectionPool(const Connections& connections,
                            int lowWatermark,
                            int highWatermark);

    WatermarkConnectionPool(const Connections& connections,
                            const EventLoopPtr& eventLoop,
                            int lowWatermark,
                            int highWatermark);

    virtual ~WatermarkConnectionPool();

    int setLowWatermark(int lowWatermark);
    int setHighWatermark(int hightWatermark);

    int lowWatermark() const { return lowWatermark_; }
    int highWatermark() const { return highWatermark_; }

    void start();

private:
    int maxWaiters_;
    int lowWatermark_;
    int highWatermark_;

};

inline int WatermarkConnectionPool::setLowWatermark(int lowWatermark) {
    this->lowWatermark_ = lowWatermark;
    return 0;
}

inline int WatermarkConnectionPool::setHighWatermark(int hightWatermark) {
    this->highWatermark_ = highWatermark_;
    return 0;
}

}
}
}

#endif //#if !defined(CETTY_SERVICE_POOL_WATERMARKCONNECTIONPOOL_H)

// Local Variables:
// mode: c++
// End:
