/*
 * Copyright 2010 Red Hat, Inc.
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

#include <boost/algorithm/string/predicate.hpp>

#include "cetty/channel/Channel.h"
#include "cetty/channel/ChannelMessage.h"
#include "cetty/channel/ChannelPipeline.h"
#include "cetty/channel/SimpleChannelUpstreamHandler.h"
#include "cetty/channel/ChannelHandlerContext.h"
#include "cetty/channel/MessageEvent.h"
#include "cetty/channel/ExceptionEvent.h"
#include "cetty/channel/ChannelFutureListener.h"

#include "cetty/handler/codec/http/DefaultHttpResponse.h"
#include "cetty/handler/codec/http/HttpMessage.h"
#include "cetty/handler/codec/http/HttpRequest.h"
#include "cetty/handler/codec/http/HttpHeaders.h"
#include "cetty/handler/codec/http/HttpVersion.h"
#include "cetty/handler/codec/http/HttpMethod.h"
#include "cetty/handler/codec/http/websocket/WebSocketFrame.h"

#include "cetty/handler/codec/http/websocket/WebSocketFrameEncoder.h"
#include "cetty/handler/codec/http/websocket/WebSocketFrameDecoder.h"

#include "WebSocketServerIndexPage.h"

using namespace cetty::channel;
using namespace cetty::handler::codec::http;
using namespace cetty::handler::codec::http::websocket;

/**
 * @author <a href="http://www.jboss.org/netty/">The Netty Project</a>
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 *
 * @version $Rev: 2314 $, $Date: 2010-06-22 16:02:27 +0900 (Tue, 22 Jun 2010) $
 */

class WebSocketServerHandler : public SimpleChannelUpstreamHandler {
public:
	WebSocketServerHandler(){}
	virtual ~WebSocketServerHandler() {}
    
	virtual ChannelHandlerPtr clone() { return ChannelHandlerPtr(new WebSocketServerHandler); }

	virtual std::string toString() const { return "WebSocketServerHandler"; }

    virtual void messageReceived(ChannelHandlerContext& ctx, const MessageEvent& e) {
		HttpRequestPtr msg = e.getMessage().smartPointer<HttpRequest>();
        if (msg) {
            handleHttpRequest(ctx, *msg);
        }
        WebSocketFramePtr frame = e.getMessage().smartPointer<WebSocketFrame>();
        if (frame) {
            handleWebSocketFrame(ctx, frame);
        }
    }

    virtual void exceptionCaught(ChannelHandlerContext& ctx, const ExceptionEvent& e) {
		e.getChannel().close();
	}

private:
    void handleHttpRequest(ChannelHandlerContext& ctx, HttpRequest& req) {
        // Allow only GET methods.
		if (req.getMethod().compareTo(HttpMethod::HM_GET)) {
            sendHttpResponse(ctx, req, 
                HttpResponsePtr(new DefaultHttpResponse(HttpVersion::HTTP_1_1, HttpResponseStatus::FORBIDDEN)));
            return;
        }

        // Send the demo page.
        if (req.getUri().compare("/") == 0) {
			HttpResponsePtr res = 
				HttpResponsePtr(new DefaultHttpResponse(HttpVersion::HTTP_1_1, HttpResponseStatus::OK));

            ChannelBufferPtr content =
				WebSocketServerIndexPage::getContent(getWebSocketLocation(req));

			res->setHeader(HttpHeaders::Names::CONTENT_TYPE, "text/html; charset=UTF-8");
			HttpHeaders::setContentLength(*res, content->readableBytes());

            res->setContent(content);
            sendHttpResponse(ctx, req, res);
            return;
        }

        // Serve the WebSocket handshake request.
        if (req.getUri().compare(WEBSOCKET_PATH) == 0 &&
			boost::iequals(HttpHeaders::Values::UPGRADE, req.getHeader(HttpHeaders::Names::CONNECTION)) &&
			boost::iequals(HttpHeaders::Values::WEBSOCKET, req.getHeader(HttpHeaders::Names::UPGRADE))) {

            // Create the WebSocket handshake response.
            HttpResponsePtr res =
				HttpResponsePtr(new DefaultHttpResponse(
											HttpVersion::HTTP_1_1,
											HttpResponseStatus(101, "Web Socket Protocol Handshake")));
            res->addHeader(HttpHeaders::Values::UPGRADE, HttpHeaders::Values::WEBSOCKET);
            res->addHeader(HttpHeaders::Names::CONNECTION, HttpHeaders::Values::UPGRADE);

            // Fill in the headers and contents depending on handshake method.
            if (req.containsHeader(HttpHeaders::Names::SEC_WEBSOCKET_KEY1) &&
                req.containsHeader(HttpHeaders::Names::SEC_WEBSOCKET_KEY2)) {
                // New handshake method with a challenge:
                res->addHeader(HttpHeaders::Names::SEC_WEBSOCKET_ORIGIN, req.getHeader(HttpHeaders::Names::ORIGIN));
                res->addHeader(HttpHeaders::Names::SEC_WEBSOCKET_LOCATION, getWebSocketLocation(req));
                const std::string& protocol = req.getHeader(HttpHeaders::Names::SEC_WEBSOCKET_PROTOCOL);
                if (!protocol.empty()) {
                    res->addHeader(HttpHeaders::Names::SEC_WEBSOCKET_PROTOCOL, protocol);
                }

                // Calculate the answer of the challenge.
				const std::string& key1 = req.getHeader(HttpHeaders::Names::SEC_WEBSOCKET_KEY1);
				const std::string& key2 = req.getHeader(HttpHeaders::Names::SEC_WEBSOCKET_KEY2);

                int a = 1/*(int) (Long.parseLong(key1.replaceAll("[^0-9]", "")) / key1.replaceAll("[^ ]", "").length())*/;
                int b = 1/*(int) (Long.parseLong(key2.replaceAll("[^0-9]", "")) / key2.replaceAll("[^ ]", "").length())*/;
                boost::int64_t c = req.getContent()->readLong();
				ChannelBufferPtr input = ChannelBuffers::buffer(16);
                input->writeInt(a);
                input->writeInt(b);
                input->writeLong(c);
				ChannelBufferPtr output = input;
                    //ChannelBuffers::wrappedBuffer(MessageDigest.getInstance("MD5").digest(input.array()));
                res->setContent(output);
            }
			else {
                // Old handshake method with no challenge:
                res->addHeader(HttpHeaders::Names::WEBSOCKET_ORIGIN, req.getHeader(HttpHeaders::Names::ORIGIN));
                res->addHeader(HttpHeaders::Names::WEBSOCKET_LOCATION, getWebSocketLocation(req));
				const std::string& protocol = req.getHeader(HttpHeaders::Names::WEBSOCKET_PROTOCOL);
                if (!protocol.empty()) {
                    res->addHeader(HttpHeaders::Names::WEBSOCKET_PROTOCOL, protocol);
                }
            }

            // Upgrade the connection and send the handshake response.
            ChannelPipeline& p = ctx.getChannel().getPipeline();
            p.remove("aggregator");
            p.replace("decoder", "wsdecoder", ChannelHandlerPtr(new WebSocketFrameDecoder()));

            ctx.getChannel().write(res);

            p.replace("encoder", "wsencoder", ChannelHandlerPtr(new WebSocketFrameEncoder()));
            return;
        }

        // Send an error page otherwise.
        sendHttpResponse(ctx, req, 
			HttpResponsePtr(new DefaultHttpResponse(HttpVersion::HTTP_1_1, HttpResponseStatus::FORBIDDEN)));
    }

    void handleWebSocketFrame(ChannelHandlerContext& ctx, const WebSocketFramePtr& frame) {
        // Send the uppercased string back.
        ctx.getChannel().write(ChannelMessage(frame));
                //new DefaultWebSocketFrame(frame.getTextData().toUpperCase()));
    }

    void sendHttpResponse(ChannelHandlerContext& ctx, HttpRequest& req, const HttpResponsePtr& res) {
        // Generate an error page if response status code is not OK (200).
        if (res->getStatus().getCode() != 200) {
            res->setContent(ChannelBuffers::copiedBuffer(res->getStatus().toString()));
			HttpHeaders::setContentLength(*res, res->getContent()->readableBytes());
        }

        // Send the response and close the connection if necessary.
        ChannelFuturePtr f = ctx.getChannel().write(res);
		if (!HttpHeaders::isKeepAlive(req) || res->getStatus().getCode() != 200) {
			f->addListener(ChannelFutureListener::CLOSE);
        }
    }

	std::string getWebSocketLocation(HttpRequest& req) {
		std::string str("ws://");
		str += req.getHeader(HttpHeaders::Names::HOST);
		str += WEBSOCKET_PATH;
		return str;
    }

private:
	static const  std::string WEBSOCKET_PATH;
};
