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

#include "cetty/buffer/ChannelBuffers.h"

using namespace cetty::buffer;

/**
 * Generates the demo HTML page which is served at http://localhost:8080/
 *
 * @author <a href="http://www.jboss.org/netty/">The Netty Project</a>
 * @author <a href="http://gleamynode.net/">Trustin Lee</a>
 *
 * @version $Rev: 2080 $, $Date: 2010-01-26 18:04:19 +0900 (Tue, 26 Jan 2010) $
 */
class WebSocketServerIndexPage {
public:
	static ChannelBufferPtr getContent(const std::string& webSocketLocation) {
		std::string str;
		str = precontent;
		str += webSocketLocation;
		str += postcontent;
		return ChannelBuffers::copiedBuffer(str);
    }

private:
	static const char* precontent;
	static const char* postcontent;
};
