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

#include "cetty/channel/ChannelPipelineFactory.h"
#include "cetty/channel/ChannelFactory.h"
#include "cetty/channel/ChannelPipeline.h"
#include "cetty/channel/Channels.h"

#include "HexDumpProxyInboundHandler.h"

using namespace cetty::channel;

/**
 * @author <a href="http://www.jboss.org/netty/">The Netty Project</a>
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 * @version $Rev: 2080 $, $Date: 2010-01-26 18:04:19 +0900 (Tue, 26 Jan 2010) $
 */

class HexDumpProxyPipelineFactory : public cetty::channel::ChannelPipelineFactory {
public:
    HexDumpProxyPipelineFactory(const ChannelFactoryPtr& cf, const std::string& remoteHost, int remotePort)
        : cf(cf), remoteHost(remoteHost), remotePort(remotePort) {
    }

    ChannelPipeline* getPipeline() {
        ChannelPipeline* p = Channels::pipeline(
            ChannelHandlerPtr(new HexDumpProxyInboundHandler(cf, remoteHost, remotePort)));
        return p;
    }

private:
    ChannelFactoryPtr cf;
    std::string remoteHost;
    int remotePort;
};
