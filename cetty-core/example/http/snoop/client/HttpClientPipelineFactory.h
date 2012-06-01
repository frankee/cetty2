#if !defined(HTTP_SNOOP_HTTPCLIENTPIPELINEFACTORY_H)
#define HTTP_SNOOP_HTTPCLIENTPIPELINEFACTORY_H

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
#include "cetty/handler/codec/http/HttpClientCodec.h"

#include "HttpResponseHandler.h"

using namespace cetty::channel;
using namespace cetty::handler::codec::http;

/**
 * @author <a href="http://www.jboss.org/netty/">The Netty Project</a>
 * @author Andy Taylor (andy.taylor@jboss.org)
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 *
 * @version $Rev: 2226 $, $Date: 2010-03-31 11:26:51 +0900 (Wed, 31 Mar 2010) $
 */
class HttpClientPipelineFactory : public cetty::channel::ChannelPipelineFactory {
public:
    HttpClientPipelineFactory() : ssl(false) {

    }

    HttpClientPipelineFactory(bool ssl) : ssl(ssl) {
    }

    virtual ~HttpClientPipelineFactory() {

    }

    virtual ChannelPipeline* getPipeline() {
        // Create a default pipeline implementation.
        ChannelPipeline* pipeline = Channels::pipeline();

        // Enable HTTPS if necessary.
        if (ssl) {
//             SSLEngine engine =
//                 SecureChatSslContextFactory.getClientContext().createSSLEngine();
//             engine.setUseClientMode(true);
// 
//             pipeline.addLast("ssl", new SslHandler(engine));
        }

        pipeline->addLast("codec", ChannelHandlerPtr(new HttpClientCodec));

        // Remove the following line if you don't want automatic content decompression.
        //pipeline->addLast("inflater", new HttpContentDecompressor());

        // Uncomment the following line if you don't want to handle HttpChunks.
        //pipeline.addLast("aggregator", new HttpChunkAggregator(1048576));

        pipeline->addLast("handler", ChannelHandlerPtr(new HttpResponseHandler()));
        return pipeline;
    }

private:
    bool ssl;
};


#endif //#if !defined(HTTP_SNOOP_HTTPCLIENTPIPELINEFACTORY_H)
