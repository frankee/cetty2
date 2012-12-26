#if !defined(HTTP_SNOOP_HTTPREQUESTHANDLER_H)
#define HTTP_SNOOP_HTTPREQUESTHANDLER_H

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

#include <cetty/buffer/Unpooled.h>

#include <cetty/channel/ChannelFutureListener.h>
#include <cetty/channel/ChannelHandlerContext.h>
#include <cetty/channel/ChannelInboundMessageHandler.h>

#include <cetty/handler/codec/http/HttpPackage.h>
#include <cetty/handler/codec/http/HttpRequest.h>
#include <cetty/handler/codec/http/HttpResponse.h>
#include <cetty/handler/codec/http/HttpChunk.h>
#include <cetty/handler/codec/http/HttpChunkTrailer.h>
#include <cetty/handler/codec/http/HttpHeaders.h>
#include <cetty/handler/codec/http/HttpResponseStatus.h>
#include <cetty/handler/codec/http/QueryStringDecoder.h>

#include <cetty/util/URI.h>
#include <cetty/util/NameValueCollection.h>

using namespace cetty::channel;
using namespace cetty::buffer;
using namespace cetty::util;
using namespace cetty::handler::codec::http;

/**
 * @author <a href="http://www.jboss.org/netty/">The Netty Project</a>
 * @author Andy Taylor (andy.taylor@jboss.org)
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 *
 * @version $Rev: 2288 $, $Date: 2010-05-27 21:40:50 +0900 (Thu, 27 May 2010) $
 */
class HttpRequestHandler : private boost::noncopyable {
public:
    typedef ChannelInboundMessageHandler<HttpRequestHandler,
        HttpPackage,
        HttpPackage> Handler;

    typedef Handler::Context Context;
    typedef Handler::InboundContainer InboundContainer;
    typedef Handler::OutboundTransfer OutboundTransfer;

public:
    HttpRequestHandler() : readingChunks(false) {}
    ~HttpRequestHandler() {}

    void registerTo(Context& ctx) {

    }
    
    void messageReceived(ChannelHandlerContext& ctx,
        const HttpPackage& msg) {
        if (!readingChunks) {
            if (!msg.httpRequest()) {

            }

            request.reset();
            request = msg.httpRequest();

            if (!request) {
                // TODO
                // log here
            }
            
            buf.clear();
            buf.append("WELCOME TO THE WILD WILD WEB SERVER\r\n");
            buf.append("===================================\r\n");

            buf.append("VERSION: ");
            buf.append(request->version().toString());
            buf.append("\r\n");

            buf.append("HOSTNAME: ");
            buf.append(request->headers().host("unknown"));
            buf.append("\r\n");
            
            buf.append("REQUEST_URI: ");
            buf.append(request->getUriString());
            buf.append("\r\n\r\n");

            HttpRequest::ConstHeaderIterator itr = request->getFirstHeader();
            HttpRequest::ConstHeaderIterator end = request->getLastHeader();
            for (; itr != end; ++itr) {
                buf.append("HEADER: ");
                buf.append(itr->first);
                buf.append(" = ");
                buf.append(itr->second);
                buf.append("\r\n");
            }
            buf.append("\r\n");

            QueryStringDecoder queryStringDecoder(request->uri());
            NameValueCollection params;
            queryStringDecoder.getParameters(&params);

            if (!params.empty()) {
                NameValueCollection::ConstIterator itr;
                for (itr = params.begin(); itr != params.end(); ++itr) {
                        buf.append("PARAM: ");
                        buf.append(itr->first);
                        buf.append(" = ");
                        buf.append(itr->second);
                        buf.append("\r\n");
                }
                buf.append("\r\n");
            }

            if (request->transferEncoding().multiple()) {
                readingChunks = true;
            }
            else {
                ChannelBufferPtr content = request->content();
                if (content->readable()) {
                    buf.append("CONTENT: ");
                    buf.append(content->toString());
                    buf.append("\r\n");
                }
                writeResponse(ctx);
            }
        }
        else if (msg.isHttpChunk()) {
            HttpChunkPtr chunk = msg.httpChunk();

            buf.append("CHUNK: ");
            buf.append(chunk->getContent()->toString());
            buf.append("\r\n");
        }
        else if (msg.isHttpChunkTrailer()) {
            HttpChunkTrailerPtr trailer = msg.httpChunkTrailer();

            //if (chunk->isLast()) {
                readingChunks = false;
                buf.append("END OF CONTENT\r\n");

                std::vector<std::string> names;
                trailer->headerNames(&names);
                if (!names.empty()) {
                    buf.append("\r\n");
                    std::vector<std::string> values;
                    for (size_t i = 0; i < names.size(); ++i) {
                        trailer->headerValues(names[i], &values);
                        for (size_t j = 0; j < values.size(); ++j) {
                            buf.append("TRAILING HEADER: ");
                            buf.append(names[i]);
                            buf.append(" = ");
                            buf.append(values[j]);
                            buf.append("\r\n");
                        }
                    }
                    buf.append("\r\n");
                }

                writeResponse(ctx);
            //}
        }
    }

    void exceptionCaught(ChannelHandlerContext& ctx,
        const ChannelException& e) {
        ctx.close();
    }

private:
    void writeResponse(ChannelHandlerContext& ctx) {
        // Decide whether to close the connection or not.
        bool keepAlive = request->keepAlive();

        // Build the response object.
        HttpResponsePtr response = 
            HttpResponsePtr(new HttpResponse(HttpVersion::HTTP_1_1,
                                                    HttpResponseStatus::OK));
        response->setContent(Unpooled::copiedBuffer(buf));
        response->setHeader(HttpHeaders::Names::CONTENT_TYPE, "text/plain; charset=UTF-8");

        if (keepAlive) {
            // Add 'Content-Length' header only for a keep-alive connection.
            response->setHeader(HttpHeaders::Names::CONTENT_LENGTH,
                                response->content()->readableBytes());
        }

        // Encode the cookie.
//         std::string cookieString = request->getHeader(COOKIE);
//         if (cookieString != null) {
//             CookieDecoder cookieDecoder = new CookieDecoder();
//             Set<Cookie> cookies = cookieDecoder.decode(cookieString);
//             if(!cookies.isEmpty()) {
//                 // Reset the cookies if necessary.
//                 CookieEncoder cookieEncoder = new CookieEncoder(true);
//                 for (Cookie cookie : cookies) {
//                     cookieEncoder.addCookie(cookie);
//                 }
//                 response.addHeader(SET_COOKIE, cookieEncoder.encode());
//             }
//         }

        // Write the response.
        ChannelFuturePtr future = ctx.channel()->newFuture();

        // Close the non-keep-alive connection after the write operation is done.
        if (!keepAlive) {
            future->addListener(ChannelFutureListener::CLOSE);
        }

        transfer_->write(response, future);
    }

private:
    bool readingChunks;

    InboundContainer* container_;
    OutboundTransfer* transfer_;

    HttpRequestPtr request;

    /** Buffer that stores the response content */
    std::string buf;
};

#endif //#if !defined(HTTP_SNOOP_HTTPREQUESTHANDLER_H)
