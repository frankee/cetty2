#if !defined(CETTY_SERVICE_OUTSTANDINGCALL_H)
#define CETTY_SERVICE_OUTSTANDINGCALL_H

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

#include <boost/intrusive_ptr.hpp>
#include <cetty/util/ReferenceCounter.h>
#include <cetty/service/ServiceFuture.h>

namespace cetty {
namespace service {

template<typename RequestT, typename ResponseT>
class OutstandingCall : public cetty::util::ReferenceCounter<OutstandingCall<RequestT, ResponseT>, int> {
public:
    typedef boost::intrusive_ptr<ServiceFuture<ResponseT> > ServiceFuturePtr;

public:
    void setId(boost::int64_t id);
    boost::int64_t getId() const;

public:
    RequestT request;
    ServiceFuturePtr future;

public:
    OutstandingCall(const RequestT& request, const ServiceFuturePtr& future)
        : request(request), future(future) {}

private:
    boost::int64_t id;
};

}
}

#endif //#if !defined(CETTY_SERVICE_OUTSTANDINGCALL_H)

// Local Variables:
// mode: c++
// End:
