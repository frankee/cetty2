#if !defined(CETTY_HANDLER_CODEC_HTTP_HTTPCHUNKTRAILER_H)
#define CETTY_HANDLER_CODEC_HTTP_HTTPCHUNKTRAILER_H

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

#include <cetty/handler/codec/http/HttpChunk.h>
#include <cetty/handler/codec/http/HttpHeaders.h>
#include <cetty/handler/codec/http/HttpChunkTrailerPtr.h>

namespace cetty {
namespace handler {
namespace codec {
namespace http {

/**
 * The last {@link HttpChunk} which has trailing headers.
 *
 *
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 * @author <a href="mailto:frankee.zhou@gmail.com">Frankee Zhou</a>
 */

class HttpChunkTrailer : public HttpChunk {
public:
    HttpChunkTrailer();
    virtual ~HttpChunkTrailer();

    HttpHeaders& headers() {
        return headers_;
    }

    const HttpHeaders& headers() const {
        return headers_;
    }

    /**
     * Always returns <tt>true</tt>.
     */
    virtual bool isLast() const;

    /**
     * do not allow to set the content.
     */
    virtual void setContent(const ChannelBufferPtr& content);

    virtual std::string toString() const;

private:
    HttpHeaders headers_;
};

}
}
}
}

#endif //#if !defined(CETTY_HANDLER_CODEC_HTTP_HTTPCHUNKTRAILER_H)

// Local Variables:
// mode: c++
// End:
