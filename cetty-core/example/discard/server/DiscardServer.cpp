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
#include <cetty/channel/ChannelInitializer.h>

#include "DiscardServerHandler.h"

using namespace cetty::bootstrap;
using namespace cetty::channel;

/**
 * Discards any incoming data.
 *
 * @author <a href="http://www.jboss.org/netty/">The Netty Project</a>
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 *
 * @version $Rev: 2080 $, $Date: 2010-01-26 18:04:19 +0900 (Tue, 26 Jan 2010) $
 */

int main(int argc, char* argv[]) {
    int threadCount = 1;

    if (argc == 2) {
        threadCount = atoi(argv[1]);
    }

    ChannelInitializer1<DiscardServerHandler> initializer("discard");

    ServerBootstrap bootstrap(threadCount);

    bootstrap.setChildInitializer(boost::bind<bool>(initializer, _1))
        .setChannelOption(ChannelOption::CO_TCP_NODELAY, true)
        .setChannelOption(ChannelOption::CO_SO_REUSEADDR, true)
        .setChannelOption(ChannelOption::CO_SO_BACKLOG, 4096)
        .bind(1980)->await();

    bootstrap.waitingForExit();
    return 0;
};
