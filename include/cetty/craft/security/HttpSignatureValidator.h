#if !defined(CETTY_CRAFT_SECURITY_HTTPSIGNATUREVALIDATOR_H)
#define CETTY_CRAFT_SECURITY_HTTPSIGNATUREVALIDATOR_H

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
#include <cetty/handler/codec/http/HttpPackage.h>

namespace cetty {
namespace craft {
namespace security {

using namespace cetty::channel;
using namespace cetty::handler::codec;
using namespace cetty::handler::codec::http;

class HttpSignatureValidator : private boost::noncopyable {
public:
    typedef MessageToMessageDecoder<HttpSignatureValidator,
            HttpPackage,
            HttpPackage> Filter;

    typedef Filter::Context Context;

public:
    HttpSignatureValidator()
        : filter_(boost::bind(&HttpSignatureValidator::filter,
                              this,
                              _1,
                              _2)) {
    }

    void registerTo(Context& ctx) {
        filter_.registerTo(ctx);
    }

private:
    HttpPackage filter(ChannelHandlerContext& ctx,
                       const HttpPackage& in) {

    }

    int validate(const HttpPackage& message) {

    }

private:
    Filter filter_;
};

}
}
}


#endif //#if !defined(CETTY_CRAFT_SECURITY_HTTPSIGNATUREVALIDATOR_H)

// Local Variables:
// mode: c++
// End:
