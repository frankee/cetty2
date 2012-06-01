#if !defined(TELNET_TELNETCLIENTPIPELINEFACTORY_H)
#define TELNET_TELNETCLIENTPIPELINEFACTORY_H

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

#include "cetty/channel/ChannelHandler.h"
#include "cetty/channel/Channels.h"
#include "cetty/channel/ChannelPipelineFactory.h"
#include "cetty/handler/codec/frame/DelimiterBasedFrameDecoder.h"
#include "cetty/handler/codec/frame/Delimiters.h"
#include "cetty/handler/codec/string/StringDecoder.h"
#include "cetty/handler/codec/string/StringEncoder.h"

#include "TelnetClientHandler.h"

using namespace cetty::channel;
using namespace cetty::handler::codec::frame;
using namespace cetty::handler::codec::string;

/**
 * Creates a newly configured {@link ChannelPipeline} for a new channel.
 *
 * @author <a href="http://www.jboss.org/netty/">The Netty Project</a>
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 *
 * @version $Rev: 2080 $, $Date: 2010-01-26 18:04:19 +0900 (Tue, 26 Jan 2010) $
 *
 */
class TelnetClientPipelineFactory : public ChannelPipelineFactory {
public:
    TelnetClientPipelineFactory() {}
    virtual ~TelnetClientPipelineFactory() {}


    virtual ChannelPipeline* getPipeline() {
        // Create a default pipeline implementation.
        ChannelPipeline* pipeline = Channels::pipeline();

        // Add the text line codec combination first,
        pipeline->addLast("framer", ChannelHandlerPtr(
            new DelimiterBasedFrameDecoder(8192,
                                           Delimiters::lineDelimiter())));
        pipeline->addLast("decoder", ChannelHandlerPtr(new StringDecoder()));
        pipeline->addLast("encoder", ChannelHandlerPtr(new StringEncoder()));

        // and then business logic.
        pipeline->addLast("handler", ChannelHandlerPtr(new TelnetClientHandler()));

        return pipeline;
    }
};


#endif //#if !defined(TELNET_TELNETCLIENTPIPELINEFACTORY_H)
