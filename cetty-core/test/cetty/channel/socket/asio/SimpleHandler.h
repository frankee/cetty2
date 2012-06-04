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

#include "cetty/buffer/ChannelBuffer.h"
#include "cetty/channel/Channel.h"
#include "cetty/channel/SimpleChannelHandler.h"
#include "cetty/channel/ChannelHandlerContext.h"
#include "cetty/channel/MessageEvent.h"
#include "cetty/channel/ChannelMessage.h"

using namespace cetty::buffer;
using namespace cetty::channel;

/**
 *
 * @author <a href="http://www.jboss.org/netty/">The Netty Project</a>
 * @author Daniel Bevenius (dbevenius@jboss.com)
 * @version $Rev: 2119 $, $Date: 2010-02-01 20:46:09 +0900 (Mon, 01 Feb 2010) $
 */
class SimpleHandler : public SimpleChannelHandler {
public:
	SimpleHandler() {}
	virtual ~SimpleHandler() {}

	virtual toString() const { return "SimpleHandler"; }
	virtual ChannelHandlerPtr clone() { return shared_from_this(); }

    virtual void messageReceived(ChannelHandlerContext& ctx, const MessageEvent& e) {
		ChannelBufferPtr cb = e.getMessage().sharedPointer<ChannelBuffer>();
		if (cb) {
			ChannelBufferPtr actual = cb->readCopy();
			ctx.getChannel().write(actual);
		}
    }
};
