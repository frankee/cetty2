#if !defined(HTTP_SNOOP_HTTPSERVERPIPELINEFACTORY_H)
#define HTTP_SNOOP_HTTPSERVERPIPELINEFACTORY_H

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

#include "cetty/channel/Channels.h"
#include "cetty/channel/ChannelPipelineFactory.h"

#include "cetty/handler/codec/http/HttpRequestDecoder.h"
#include "cetty/handler/codec/http/HttpResponseEncoder.h"

#include "HttpRequestHandler.h"

using namespace cetty::channel;
using namespace cetty::handler::codec::http;

/**
 * @author <a href="http://www.jboss.org/netty/">The Netty Project</a>
 * @author Andy Taylor (andy.taylor@jboss.org)
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 *
 * @version $Rev: 2080 $, $Date: 2010-01-26 18:04:19 +0900 (Tue, 26 Jan 2010) $
 */
class HttpServerPipelineFactory : public ChannelPipelineFactory {
public:
    HttpServerPipelineFactory() {}
    virtual ~HttpServerPipelineFactory() {}

    ChannelPipeline* getPipeline() {
        // Create a default pipeline implementation.
        ChannelPipeline* pipeline = Channels::pipeline();
        BOOST_ASSERT(pipeline);

        // Uncomment the following line if you want HTTPS
        //SSLEngine engine = SecureChatSslContextFactory.getServerContext().createSSLEngine();
        //engine.setUseClientMode(false);
        //pipeline.addLast("ssl", new SslHandler(engine));

        pipeline->addLast("decoder", ChannelHandlerPtr(new HttpRequestDecoder()));
        
        // Uncomment the following line if you don't want to handle HttpChunks.
        //pipeline.addLast("aggregator", new HttpChunkAggregator(1048576));
        
        pipeline->addLast("encoder", ChannelHandlerPtr(new HttpResponseEncoder()));
        
        // Remove the following line if you don't want automatic content compression.
        //pipeline.addLast("deflater", new HttpContentCompressor());

        pipeline->addLast("handler", ChannelHandlerPtr(new HttpRequestHandler()));

        return pipeline;
    }
};

#endif //#if !defined(HTTP_SNOOP_HTTPSERVERPIPELINEFACTORY_H)
