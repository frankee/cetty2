#if !defined(HTTP_SNOOP_HTTPRESPONSEHANDLER_H)
#define HTTP_SNOOP_HTTPRESPONSEHANDLER_H

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
#include <cetty/buffer/ChannelBuffer.h>

#include <cetty/channel/ChannelHandlerContext.h>
#include <cetty/channel/ChannelInboundMessageHandler.h>

#include <cetty/handler/codec/http/HttpChunk.h>
#include <cetty/handler/codec/http/HttpPackage.h>
#include <cetty/handler/codec/http/HttpMessage.h>
#include <cetty/handler/codec/http/HttpResponse.h>

using namespace cetty::channel;
using namespace cetty::buffer;
using namespace cetty::util;
using namespace cetty::handler::codec::http;

/**
 * @author <a href="http://www.jboss.org/netty/">The Netty Project</a>
 * @author Andy Taylor (andy.taylor@jboss.org)
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 *
 * @version $Rev: 2189 $, $Date: 2010-02-19 18:02:57 +0900 (Fri, 19 Feb 2010) $
 */
class HttpResponseHandler : public ChannelInboundMessageHandler<HttpPackage> {
public:
    HttpResponseHandler() : readingChunks(false) {}
    virtual ~HttpResponseHandler() {}

    virtual ChannelHandlerPtr clone() {
        return new HttpResponseHandler;
    }
    virtual std::string toString() const {
        return "HttpResponseHandler";
    }

protected:
    virtual void messageReceived(ChannelHandlerContext& ctx,
        const HttpPackage& msg) {
        if (!readingChunks) {
            if (!msg.isHttpMessage()) {

            }

            HttpResponsePtr response =
                boost::dynamic_pointer_cast<HttpResponse>(msg.httpMessage());

            printf("STATUS: %s\n", response->getStatus().toString().c_str());
            printf("VERSION: %s\n", response->getProtocolVersion().toString().c_str());
            printf("\n");

            std::vector<std::string> names;
            std::vector<std::string> values;
            response->getHeaderNames(&names);
            for (size_t i = 0; i < names.size(); ++i) {
                values.clear();
                response->getHeaders(names[i], &values);
                for (size_t j = 0; j < values.size(); ++j) {
                    printf("HEADER: %s = %s\n", names[i].c_str(), values[j].c_str());
                }
                printf("\n");
            }

            if (response->getStatus().getCode() == 200
                && response->getTransferEncoding().isMultiple()) {
                readingChunks = true;
                printf("CHUNKED CONTENT {\n");
            }
            else {
                ChannelBufferPtr content = response->getContent();
                if (content->readable()) {
                    printf("CONTENT {\n");
                    printf("%s\n", content->toString());
                    printf("} END OF CONTENT\n");
                }
            }
        }
        else {
            HttpChunkPtr chunk = msg.httpChunk();
            if (chunk->isLast()) {
                readingChunks = false;
                printf("} END OF CHUNKED CONTENT\n");
            }
            else {
                printf(chunk->getContent()->toString().c_str());
            }
        }
    }

private:
    bool readingChunks;
};

#endif //#if !defined(HTTP_SNOOP_HTTPRESPONSEHANDLER_H)
