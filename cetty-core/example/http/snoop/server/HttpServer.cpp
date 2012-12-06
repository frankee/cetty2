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

#include <cetty/bootstrap/ServerBootstrap.h>
#include <cetty/bootstrap/asio/AsioServerBootstrap.h>

#include <cetty/channel/ChannelInitializer.h>
#include <cetty/handler/codec/http/HttpServerCodec.h>
#include <cetty/handler/codec/http/HttpChunkAggregator.h>
#include "HttpRequestHandler.h"

using namespace cetty::bootstrap::asio;
using namespace cetty::channel;
using namespace cetty::handler::codec::http;

/**
 * An HTTP server that sends back the content of the received HTTP request
 * in a pretty plaintext form.
 *
 * @author <a href="http://www.jboss.org/netty/">The Netty Project</a>
 * @author Andy Taylor (andy.taylor@jboss.org)
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 *
 * @version $Rev: 2080 $, $Date: 2010-01-26 18:04:19 +0900 (Tue, 26 Jan 2010) $
 */

int main(int argc, const char* argv[]) {
    // Configure the server.
    AsioServerBootstrap bootstrap(0);

    ChannelInitializer2<HttpServerCodec, HttpRequestHandler> initializer;

    bootstrap.setOption(ChannelOption::CO_TCP_NODELAY, true)
    .setOption(ChannelOption::CO_SO_REUSEADDR, true)
    .setOption(ChannelOption::CO_SO_BACKLOG, 4096)
    .setChildInitializer(boost::bind<bool>(initializer, _1));

    bootstrap.bind(8080);
};
