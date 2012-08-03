#if !defined(CETTY_GEARMAN_PROTOBUF_GEARMANPROTOBUFCLIENTFILTER_H)
#define CETTY_GEARMAN_PROTOBUF_GEARMANPROTOBUFCLIENTFILTER_H

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
#include <cetty/service/ClientServiceFilter.h>
#include <cetty/protobuf/service/ProtobufServiceMessagePtr.h>
#include <cetty/gearman/GearmanMessagePtr.h>

namespace cetty {
namespace gearman {
namespace protobuf {

using namespace cetty::service;
using namespace cetty::channel;
using namespace cetty::protobuf::service;
using namespace cetty::gearman;

class GearmanProtobufClientFilter
    : public cetty::service::ClientServiceFilter<
    ProtobufServiceMessagePtr,
    GearmanMessagePtr,
    GearmanMessagePtr,
    ProtobufServiceMessagePtr> {
public:
    GearmanProtobufClientFilter();
    virtual ~GearmanProtobufClientFilter();

public:
    typedef boost::intrusive_ptr<GearmanProtobufClientFilter> GearmanProtobufClientFilterPtr;

    virtual ChannelHandlerPtr clone();
    virtual std::string toString() const;

protected:
    virtual GearmanMessagePtr filterRequest(OutboundMessageContext& ctx,
        const ProtobufServiceMessagePtr& req);

    virtual ProtobufServiceMessagePtr filterResponse(InboundMessageContext& ctx,
        const ProtobufServiceMessagePtr& req,
        const GearmanMessagePtr& rep);

};

}
}
}

#endif //#if !defined(CETTY_GEARMAN_PROTOBUF_GEARMANPROTOBUFCLIENTFILTER_H)

// Local Variables:
// mode: c++
// End:
