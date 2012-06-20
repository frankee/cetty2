#if !defined(CETTY_SERVICE_CLIENTSERVICE_H)
#define CETTY_SERVICE_CLIENTSERVICE_H

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

#include <vector>

namespace cetty {
namespace service {

template <typename RequestT, typename ResponseT>
class ClientService {
public:
    typedef RequestT& RequestRef;
    typedef boost::intrusive_ptr<ServiceFuture<ResponseT> > ServiceFuturePtr;

public:
    void call(const RequestT& request, const ServiceFuturePtr& future);
};


class ClientServiceMessageHandler {



};


class ClientServiceChannel {
public:

};

}
}

#endif //#if !defined(CETTY_SERVICE_CLIENTSERVICE_H)

// Local Variables:
// mode: c++
// End:
