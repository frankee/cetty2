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

#include <cetty/gearman/GearmanMessageHandler.h>
#include <cetty/channel/ChannelHandlerContext.h>
#include <cetty/channel/Channels.h>
#include <cetty/channel/MessageEvent.h>
#include <cetty/channel/ChannelMessage.h>

#include <cetty/gearman/GearmanTask.h>
#include <cetty/gearman/GearmanMessage.h>

namespace cetty {
namespace gearman {

using namespace cetty::channel;

void GearmanMessageHandler::messageReceived(ChannelHandlerContext& ctx, const MessageEvent& e) {
    GearmanMessagePtr message = e.getMessage().smartPointer<GearmanMessage>();

    if (!message) {
        ctx.sendUpstream(e);
    }

    switch (message->type) {
    case GearmanMessage::CAN_DO:
        break;

    case GearmanMessage::CANT_DO:
        break;

    default:
        ctx.sendUpstream(e);
    }
}

void GearmanMessageHandler::writeRequested(ChannelHandlerContext& ctx, const MessageEvent& e) {
    GearmanTaskPtr task = e.getMessage().smartPointer<GearmanTask>();

    tasks.push_back(task);

    Channels::write(ctx, e.getFuture(), ChannelMessage(task->request));
}

}
}