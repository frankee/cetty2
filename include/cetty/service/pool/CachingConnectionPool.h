#if !defined(CETTY_SERVICE_POOL_CACHINGCONNECTIONPOOL_H)
#define CETTY_SERVICE_POOL_CACHINGCONNECTIONPOOL_H

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

#include <boost/date_time/posix_time/posix_time_duration.hpp>
#include <cetty/service/pool/ConnectionPool.h>

namespace cetty {
namespace service {
namespace pool {

/**
 * A pool that temporarily caches items from the underlying one, up to
 * the given timeout amount of time.
 */
class CachingConnectionPool : public ConnectionPool {
public:
    CachingConnectionPool(const std::vector<Connection>& connections,
                          int cacheSize,
                          const boost::posix_time::time_duration& ttl);

private:
    int cacheSize;
    boost::posix_time::time_duration ttl;
};

}
}
}

#endif //#if !defined(CETTY_SERVICE_POOL_CACHINGCONNECTIONPOOL_H)

// Local Variables:
// mode: c++
// End:
