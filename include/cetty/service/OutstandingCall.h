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
#include <boost/date_time/posix_time/ptime.hpp>

#include <cetty/util/ReferenceCounter.h>
#include <cetty/service/ServiceFuture.h>

namespace cetty {
namespace service {

using namespace boost::posix_time;

template<typename Request, typename Response>
class OutstandingCall
    : public cetty::util::ReferenceCounter<OutstandingCall<Request, Response> > {
public:
    typedef boost::intrusive_ptr<ServiceFuture<Response> > ServiceFuturePtr;

public:
    OutstandingCall(const Request& request, const ServiceFuturePtr& future)
        : request_(request),
         future_(future),
         timeStamp_() {
    }

    int64_t id() const {
        return id_;
    }

    void setId(int64_t id) {
        id_ = id;
    }

    const Request& request() const {
        return request_;
    }

    const Response& response() const {
        return response_;
    }

    const ServiceFuturePtr& future() const {
        return future_;
    }

    void setResponse(const Response& response) {
        response_ = response;
        if (future_) {
            future_->setResponse(response);
        }
    }

private:
    int hash_;
    int64_t id_;
    int64_t timeout_;
    ptime timeStamp_;
    Request request_;
    Response response_;
    ServiceFuturePtr future_;
};

}
}

#endif //#if !defined(CETTY_SERVICE_OUTSTANDINGCALL_H)

// Local Variables:
// mode: c++
// End:
