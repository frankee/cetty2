#if !defined(CETTY_GEARMAN_GEARMANCLIENTREQUESTHANDLER_H)
#define CETTY_GEARMAN_GEARMANCLIENTREQUESTHANDLER_H

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
#include <cetty/service/OutstandingCall.h>
#include <cetty/service/ServiceRequestHandler.h>
#include <cetty/gearman/GearmanMessagePtr.h>

namespace cetty { namespace gearman { 

using namespace cetty::service;
using namespace cetty::protobuf::service;

typedef OutstandingCall<GearmanMessagePtr, GearmanMessagePtr> GearmanClientCall;
typedef boost::intrusive_ptr<GearmanClientCall> GearmanClientCallPtr;

typedef ServiceRequestHandler<GearmanMessagePtr, GearmanMessagePtr> GearmanClientRequestHandler;

}
}

#endif //#if !defined(CETTY_GEARMAN_GEARMANCLIENTREQUESTHANDLER_H)

// Local Variables:
// mode: c++
// End:
