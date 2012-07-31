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

#include <cetty/gearman/GearmanClientHandler.h>
#include <cetty/channel/ChannelHandlerContext.h>
#include <cetty/channel/Channels.h>
#include <cetty/channel/MessageEvent.h>
#include <cetty/channel/ChannelMessage.h>

#include <cetty/gearman/GearmanMessage.h>
#include <cetty/gearman/GearmanWorker.h>
#include <string>
#include <iostream>


namespace cetty {
namespace gearman {

using namespace cetty::channel;
using namespace cetty::buffer;

GearmanClientHandler::GearmanClientHandler() {
}

GearmanClientHandler::~GearmanClientHandler() {
}

void GearmanClientHandler::messageReceived(ChannelHandlerContext& ctx, const MessageEvent& e) {
    GearmanMessagePtr message = e.getMessage().smartPointer<GearmanMessage>();
    std::string  data;
    std::vector<std::string> params;

    if (!message) {
        ctx.sendUpstream(e);
    }

    switch (message->getType()) {
    case GearmanMessage::JOB_CREATED:
        std::cout<<"the JOB_SUBMIT is ok "<< std::endl;
        params = message->getParameters();
        std::cout<<"the job-handler is "<<params[0]<<std::endl;
        break;

    case GearmanMessage::WORK_STATUS:
        std::cout<<"the  WORK_STATUS"<< std::endl;
        params = message->getParameters();
        std::cout<<"the job-handler is "<<params[0]<<std::endl;
        std::cout<<"the percent complete numerator is "<<params[1]<<std::endl;
        std::cout<<"the Percent complete denominator is "<<params[2]<<std::endl;
        break;

        //job handler来唯一标识一个job  定义一个 map<主job，vector<分job handler> >
    case GearmanMessage::WORK_COMPLETE:
        std::cout<<"the WORK_COMPLETE! "<< std::endl;
        params = message->getParameters();
        std::cout<<"the job-handler is "<<params[0]<<std::endl;
        message->getData()->readBytes(&data);
        std::cout<<"the work complete is "<<data<<std::endl;
        //to send the response to client
        //ctx.getChannel()->write(ChannelMessage(message->getData()));
        //ctx.sendUpstream(e);
        break;

    case GearmanMessage::WORK_WARNING:
        std::cout<<"the client reciver the WORK_WARNING, handler it "<< std::endl;
        params = message->getParameters();
        std::cout<<"the job-handler is "<<params[0]<<std::endl;
        data = ChannelBuffers::hexDump(message->getData());
        std::cout<<"the work data is "<<data<<std::endl;
        break;

    case GearmanMessage::WORK_FAIL:
        std::cout<<"the WORK_FAIL! "<< std::endl;
        params = message->getParameters();
        std::cout<<"the job-handler is "<<params[0]<<std::endl;
        break;

    case GearmanMessage::WORK_EXCEPTION:
        std::cout<<"the WORK_EXCEPTION! "<< std::endl;
        break;

    case GearmanMessage::WORK_DATA:
        std::cout<<"the client reciver the WORK_DATA, handler it "<< std::endl;
        params = message->getParameters();
        std::cout<<"the job-handler is "<<params[0]<<std::endl;
        data = ChannelBuffers::hexDump(message->getData());
        std::cout<<"the work data is "<<data<<std::endl;
        break;

    case GearmanMessage::STATUS_RES:
        std::cout<<"the GET_STATUS is ok,only for bg way"<< std::endl;
        params = message->getParameters();
        std::cout<<"the job-handler is "<<params[0]<<std::endl;
        std::cout<<"the known status is "<<params[1]<<std::endl;
        std::cout<<"the running status is "<<params[2]<<std::endl;
        std::cout<<"the  complete numerator is "<<params[3]<<std::endl;
        std::cout<<"the  complete denominator is "<<params[4]<<std::endl;
        break;

    case GearmanMessage::OPTION_RES:
        std::cout<<"the OPTION_REQ is ok "<< std::endl;
        params = message->getParameters();
        std::cout<<"the name of the option is "<<params[0]<<std::endl;
        break;

    default:
        ctx.sendUpstream(e);
    }
}

void GearmanClientHandler::writeRequested(ChannelHandlerContext& ctx, const MessageEvent& e) {
    GearmanMessagePtr msg = e.getMessage().smartPointer<GearmanMessage>();

    if (msg) {
        msgs.push_back(msg);
        ChannelPipelines::write(ctx, e.getFuture(), UserEvent(msg));
    }
    else {
        ctx.sendDownstream(e);
    }
}

ChannelHandlerPtr GearmanClientHandler::clone() {
    return ChannelHandlerPtr(new GearmanClientHandler);
}

std::string GearmanClientHandler::toString() const {
    return "GearmanClientHandler";
}

}
}