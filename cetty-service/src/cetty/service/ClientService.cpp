/*
 * Copyright (c) 2010-2012 frankee zhou (frankee.zhou at gmail dot com)
 *
 * Distributed under under the Apache License, version 2.0 (the "License").
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 */

#include <cetty/service/ClientService.h>

#include <cetty/channel/SocketAddress.h>

#include <cetty/logging/LoggerHelper.h>

namespace cetty {
namespace service {

using namespace cetty::channel;

bool ClientService::isOpen() const {
    return true;
}

bool ClientService::isActive() const {
    return true;
}

void ClientService::doBind(const SocketAddress& localAddress) {
    //TODO
}

void ClientService::doDisconnect() {
    //TODO
}

void ClientService::doClose() {
    //TODO
}

ClientService::ClientService(const EventLoopPtr& eventLoop)
    : Channel(ChannelPtr(), eventLoop) {

    //pipeline->attach(shared_from_this());

    //LOG_INFO << "ClientService firing the Channel Create Event.";
    //pipeline->fireChannelOpen();
}

}
}
