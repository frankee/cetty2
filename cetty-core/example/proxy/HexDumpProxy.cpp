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

#include <cetty/util/StringUtil.h>
#include <cetty/handler/logging/LoggingHandler.h>
#include "HexDumpProxyFrontendHandler.h"

using namespace cetty::bootstrap;
using namespace cetty::util;
using namespace cetty::channel;
using namespace cetty::handler::logging;

/**
 * @author <a href="http://www.jboss.org/netty/">The Netty Project</a>
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 * @version $Rev: 2080 $, $Date: 2010-01-26 18:04:19 +0900 (Tue, 26 Jan 2010) $
 */

bool initializeChannel(const ChannelPtr& channel,
                       const std::string& host,
                       int port) {
    channel->pipeline().addLast<LoggingHandler::Ptr>("log",
            LoggingHandler::Ptr(new LoggingHandler(LogLevel.INFO)));

    channel->pipeline().addLast<HexDumpProxyFrontendHandler::Ptr>("proxy",
            HexDumpProxyFrontendHandler::Ptr(
                new HexDumpProxyFrontendHandler(host, port)));
}

int main(int argc, char* argv[]) {
    // Validate command line options.
    if (argc != 4) {
        printf(
            "Usage: HexDumpProxy"
            " <local port> <remote host> <remote port>\n");
        return -1;
    }

    // Parse command line options.
    int localPort = StringUtil::strto32(argv[1]);
    int remotePort = StringUtil::strto32(argv[3]);
    std::string remoteHost = argv[2];

    printf("Proxying *:%d to %s:%d ...\n",
           localPort,
           remoteHost.c_str(),
           remotePort);

    ServerBootstrap sb(1);

    sb.setChildInitializer(boost::bind(initializeChannel,
                                       remoteHost,
                                       remotePort))
    .setChildOption(ChannelOption::CO_AUTO_READ, false)
    .bind(localPort)->await();

    sb.waitingForExit();
    return 0;
}
