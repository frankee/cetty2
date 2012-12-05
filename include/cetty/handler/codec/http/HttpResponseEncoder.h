#if !defined(CETTY_HANDLER_CODEC_HTTP_HTTPRESPONSEENCODER_H)
#define CETTY_HANDLER_CODEC_HTTP_HTTPRESPONSEENCODER_H

/*
 * Copyright 2009 Red Hat, Inc.
 *
 * Red Hat licenses this file to you under the Apache License, version 2.0
 * (the "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at:
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 */
/*
 * Copyright (c) 2010-2011 frankee zhou (frankee.zhou at gmail dot com)
 * Distributed under under the Apache License, version 2.0 (the "License").
 */

#include <cetty/handler/codec/MessageToBufferEncoder.h>
#include <cetty/handler/codec/http/HttpPackageEncoder.h>

namespace cetty {
namespace handler {
namespace codec {
namespace http {

/**
 * Encodes an {@link HttpResponse} or an {@link HttpChunk} into
 * a {@link ChannelBuffer}.
 *
 *
 * @author Andy Taylor (andy.taylor@jboss.org)
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 * @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
 */


class HttpResponseEncoder : private boost::noncopyable {
public:
    typedef boost::shared_ptr<HttpResponseEncoder> HandlerPtr;

    typedef ChannelMessageHandlerContext<HttpResponseEncoder,
            VoidMessage,
            VoidMessage,
            HttpPackage,
            ChannelBufferPtr,
            VoidMessageContainer,
            VoidMessageContainer,
            ChannelMessageContainer<HttpPackage, MESSAGE_BLOCK>,
            ChannelBufferContainer> Context;

public:
    HttpResponseEncoder();

    void registerTo(Context& ctx) {
        encoder_.registerTo(ctx);
    }

private:
    HttpPackageEncoder responseEncoder_;
    MessageToBufferEncoder<HttpResponseEncoder, HttpPackage, Context> encoder_;
};

}
}
}
}

#endif //#if !defined(CETTY_HANDLER_CODEC_HTTP_HTTPRESPONSEENCODER_H)

// Local Variables:
// mode: c++
// End:
