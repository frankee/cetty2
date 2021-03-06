#if !defined(CETTY_CRAFT_SECURITY_ACCESSCONTROLLER_H)
#define CETTY_CRAFT_SECURITY_ACCESSCONTROLLER_H

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

#include <cetty/protobuf/service/handler/ProtobufServiceInboundFilter.h>

namespace cetty {
namespace craft {
namespace security {

using namespace cetty::channel;
using namespace cetty::protobuf::service::handler;

class AccessController {
public:
    typedef ProtobufServiceInboundFilter<AccessController> InboundFilter;

    typedef InboundFilter::FilterCallback FilterCallback;

public:
    AccessController();
    ~AccessController();

private:
    void filter(ChannelHandlerContext& ctx,
                const ProtobufServiceMessagePtr& req,
                const FilterCallback& filterCallback);

    void onAuthorized(bool permitted,
                      const std::string& principal,
                      const std::string& permission,
                      ChannelHandlerContext& ctx,
                      const ProtobufServiceMessagePtr& req);

private:
    InboundFilter inboundFilter_;
};

}
}
}

#endif //#if !defined(CETTY_CRAFT_SECURITY_ACCESSCONTROLLER_H)

// Local Variables:
// mode: c++
// End:
