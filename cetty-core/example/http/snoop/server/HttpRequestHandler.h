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

#include "cetty/buffer/ChannelBuffers.h"

#include "cetty/channel/ChannelFutureListener.h"
#include "cetty/channel/SimpleChannelUpstreamHandler.h"

#include "cetty/channel/ExceptionEvent.h"
#include "cetty/channel/MessageEvent.h"

#include "cetty/handler/codec/http/HttpChunk.h"
#include "cetty/handler/codec/http/HttpChunkTrailer.h"
#include "cetty/handler/codec/http/HttpHeaders.h"
#include "cetty/handler/codec/http/HttpRequest.h"
#include "cetty/handler/codec/http/HttpResponseStatus.h"
#include "cetty/handler/codec/http/DefaultHttpResponse.h"
#include "cetty/handler/codec/http/QueryStringDecoder.h"

using namespace cetty::channel;
using namespace cetty::buffer;
using namespace cetty::handler::codec::http;

/**
 * @author <a href="http://www.jboss.org/netty/">The Netty Project</a>
 * @author Andy Taylor (andy.taylor@jboss.org)
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 *
 * @version $Rev: 2288 $, $Date: 2010-05-27 21:40:50 +0900 (Thu, 27 May 2010) $
 */
class HttpRequestHandler : public cetty::channel::SimpleChannelUpstreamHandler {
public:
    HttpRequestHandler() : readingChunks(false) {}
    virtual ~HttpRequestHandler() {}

    virtual std::string toString() const { return "HttpRequestHandler"; }

    virtual ChannelHandlerPtr clone() {
        return ChannelHandlerPtr(new HttpRequestHandler());
    }
    
    virtual void messageReceived(ChannelHandlerContext& ctx, const MessageEvent& e) {
        if (!readingChunks) {
            request.reset();
            request = e.getMessage().smartPointer<HttpRequest, HttpMessage>();

            if (!request) {
                // TODO
                // log here
            }
            
            buf.clear();
            buf.append("WELCOME TO THE WILD WILD WEB SERVER\r\n");
            buf.append("===================================\r\n");

            buf.append("VERSION: ");
            buf.append(request->getProtocolVersion().toString());
            buf.append("\r\n");

            buf.append("HOSTNAME: ");
            buf.append(HttpHeaders::getHost(*request, "unknown"));
            buf.append("\r\n");
            
            buf.append("REQUEST_URI: ");
            buf.append(request->getUri());
            buf.append("\r\n\r\n");

            HttpHeader::NameValueList headers;
            request->getHeaders(headers);

            HttpHeader::NameValueListConstItr itr;
            for (itr = headers.begin(); itr != headers.end(); ++itr) {
                buf.append("HEADER: ");
                buf.append(itr->first);
                buf.append(" = ");
                buf.append(itr->second);
                buf.append("\r\n");
            }
            buf.append("\r\n");

            QueryStringDecoder queryStringDecoder(request->getUri());
            const QueryStringDecoder::ParametersType& params =
                                        queryStringDecoder.getParameters();
            if (!params.empty()) {
                QueryStringDecoder::ParametersType::const_iterator itr;
                for (itr = params.begin(); itr != params.end(); ++itr) {
                    const std::string& key = itr->first;
                    const std::vector<std::string>& vals = itr->second;
                    for (size_t i = 0; i < vals.size(); ++i) {
                        buf.append("PARAM: ");
                        buf.append(key);
                        buf.append(" = ");
                        buf.append(vals[i]);
                        buf.append("\r\n");
                    }
                }
                buf.append("\r\n");
            }

            if (request->isChunked()) {
                readingChunks = true;
            }
            else {
                ChannelBufferPtr content = request->getContent();
                if (content->readable()) {
                    buf.append("CONTENT: ");
                    buf.append(content->toString());
                    buf.append("\r\n");
                }
                writeResponse(e);
            }
        }
        else {
            HttpChunkPtr chunk = e.getMessage().smartPointer<HttpChunk, HttpMessage>();
            if (chunk->isLast()) {
                readingChunks = false;
                buf.append("END OF CONTENT\r\n");

                HttpChunkTrailerPtr trailer =
                    boost::dynamic_pointer_cast<HttpChunkTrailer>(chunk);

                HttpChunkTrailer::StringList names;
                trailer->getHeaderNames(names);
                if (!names.empty()) {
                    buf.append("\r\n");
                    HttpChunkTrailer::StringList values;
                    for (size_t i = 0; i < names.size(); ++i) {
                        trailer->getHeaders(names[i], values);
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
                writeResponse(e);
            }
            else {
                buf.append("CHUNK: ");
                buf.append(chunk->getContent()->toString());
                buf.append("\r\n");
            }
        }
    }

    virtual void exceptionCaught(ChannelHandlerContext& ctx, const ExceptionEvent& e) {
        //e.getCause().printStackTrace();
        e.getChannel().close();
    }

private:
    void writeResponse(const MessageEvent& e) {
        // Decide whether to close the connection or not.
        bool keepAlive = HttpHeaders::isKeepAlive(*request);

        // Build the response object.
        HttpResponsePtr response = 
            HttpResponsePtr(new DefaultHttpResponse(HttpVersion::HTTP_1_1,
                                                    HttpResponseStatus::OK));
        response->setContent(ChannelBuffers::copiedBuffer(buf));
        response->setHeader(HttpHeaders::Names::CONTENT_TYPE, "text/plain; charset=UTF-8");

        if (keepAlive) {
            // Add 'Content-Length' header only for a keep-alive connection.
            response->setHeader(HttpHeaders::Names::CONTENT_LENGTH,
                                response->getContent()->readableBytes());
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
        ChannelFuturePtr future = e.getChannel().write(ChannelMessage(response));

        // Close the non-keep-alive connection after the write operation is done.
        if (!keepAlive) {
            future->addListener(ChannelFutureListener::CLOSE);
        }
    }

private:
    HttpRequestPtr request;
    bool readingChunks;
    
    /** Buffer that stores the response content */
    std::string buf;
};

#endif //#if !defined(HTTP_SNOOP_HTTPREQUESTHANDLER_H)
