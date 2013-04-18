#if !defined(CETTY_CRAFT_SECURITY_PROTOBUFPARAMETERVALIDATOR_H)
#define CETTY_CRAFT_SECURITY_PROTOBUFPARAMETERVALIDATOR_H

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

#include <cetty/handler/codec/MessageToMessageDecoder.h>
#include <cetty/protobuf/service/ProtobufServiceMessagePtr.h>

namespace cetty {
namespace craft {
namespace security {

using namespace cetty::channel;
using namespace cetty::handler::codec;
using namespace cetty::protobuf::service;

class ProtobufParameterValidator : private boost::noncopyable {
public:
    typedef MessageToMessageDecoder<ProtobufParameterValidator,
            ProtobufServiceMessagePtr,
            ProtobufServiceMessagePtr> Filter;

    typedef Filter::Context Context;

public:
    ProtobufParameterValidator()
        : filter_(boost::bind(&ProtobufParameterValidator::filter,
                              this,
                              _1,
                              _2)) {
    }

    void registerTo(Context& ctx) {
        filter_.registerTo(ctx);
    }

private:
    ProtobufServiceMessagePtr filter(ChannelHandlerContext& ctx,
                                     const ProtobufServiceMessagePtr& in) {

    }

    int validate(const ProtobufServiceMessagePtr& message) {

    }

private:
    Filter filter_;
};

}
}
}

#endif //#if !defined(CETTY_CRAFT_SECURITY_PROTOBUFPARAMETERVALIDATOR_H)

// Local Variables:
// mode: c++
// End:
