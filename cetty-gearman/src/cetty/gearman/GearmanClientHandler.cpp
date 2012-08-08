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
#include <cetty/buffer/ChannelBuffers.h>
#include <cetty/handler/codec/CodecUtil.h>

#include <cetty/gearman/GearmanMessage.h>
#include <cetty/gearman/GearmanWorker.h>
#include <string>
#include <iostream>


namespace cetty {
namespace gearman {

using namespace cetty::channel;
using namespace cetty::buffer;
using namespace cetty::handler::codec;

GearmanClientHandler::GearmanClientHandler(): channel(0) {
}

GearmanClientHandler::~GearmanClientHandler() {
}

void GearmanClientHandler::submitJob(ChannelHandlerContext& ctx, const GearmanMessagePtr& msg) {
    if (CodecUtil<GearmanMessagePtr>::unfoldAndAdd(ctx, msg, false)) {
        ctx.flush();
    }
}

void GearmanClientHandler::messageReceived(ChannelHandlerContext& ctx, const GearmanMessagePtr& msg) {
    std::string  data;
    std::vector<std::string> params;

    switch (msg->getType()) {
    case GearmanMessage::JOB_CREATED:
        std::cout<<"the JOB_SUBMIT is ok "<< std::endl;
        params = msg->getParameters();
        std::cout<<"the job-handler is "<<params[0]<<std::endl;
        //can store the job-handler for match the result
        break;

    case GearmanMessage::WORK_STATUS:
        std::cout<<"the  WORK_STATUS"<< std::endl;
        params = msg->getParameters();
        std::cout<<"the job-handler is "<<params[0]<<std::endl;
        std::cout<<"the percent complete numerator is "<<params[1]<<std::endl;
        std::cout<<"the Percent complete denominator is "<<params[2]<<std::endl;
        //to monitor the Percent complete of job
        break;

        //use job handler to identify a job  define a map<Ö÷job£¬vector<·Öjob handler> > for split
    case GearmanMessage::WORK_COMPLETE:
        std::cout<<"the WORK_COMPLETE! "<< std::endl;
        params = msg->getParameters();
        std::cout<<"the job-handler is "<<params[0]<<std::endl;

        data = ChannelBuffers::hexDump(msg->getData());
        std::cout<<"the work complete data is "<< data <<std::endl;

        if (CodecUtil<GearmanMessagePtr>::unfoldAndAdd(ctx, msg, true)) {
            ctx.fireMessageUpdated();
        }
        break;

    case GearmanMessage::WORK_WARNING:
        std::cout<<"the client reciver the WORK_WARNING, handler it "<< std::endl;
        params = msg->getParameters();
        std::cout<<"the job-handler is "<<params[0]<<std::endl;
        data = ChannelBuffers::hexDump(msg->getData());
        std::cout<<"the work data is "<<data<<std::endl;
        break;

    case GearmanMessage::WORK_FAIL:
        std::cout<<"the WORK_FAIL! "<< std::endl;
        params = msg->getParameters();
        std::cout<<"the job-handler is "<<params[0]<<std::endl;
        break;

    case GearmanMessage::WORK_EXCEPTION:
        std::cout<<"the WORK_EXCEPTION! "<< std::endl;
        break;

    case GearmanMessage::WORK_DATA:
        std::cout<<"the client reciver the WORK_DATA, handler it "<< std::endl;
        params = msg->getParameters();
        std::cout<<"the job-handler is "<<params[0]<<std::endl;
        data = ChannelBuffers::hexDump(msg->getData());
        std::cout<<"the work data is "<<data<<std::endl;
        break;

    case GearmanMessage::STATUS_RES:
        std::cout<<"the GET_STATUS is ok,only for bg way"<< std::endl;
        params = msg->getParameters();
        std::cout<<"the job-handler is "<<params[0]<<std::endl;
        std::cout<<"the known status is "<<params[1]<<std::endl;
        std::cout<<"the running status is "<<params[2]<<std::endl;
        std::cout<<"the  complete numerator is "<<params[3]<<std::endl;
        std::cout<<"the  complete denominator is "<<params[4]<<std::endl;
        break;

    case GearmanMessage::OPTION_RES:
        std::cout<<"the OPTION_REQ is ok "<< std::endl;
        params = msg->getParameters();
        std::cout<<"the name of the option is "<<params[0]<<std::endl;
        break;

    default:
        break;
    }
}

ChannelHandlerPtr GearmanClientHandler::clone() {
    return ChannelHandlerPtr(new GearmanClientHandler);
}

std::string GearmanClientHandler::toString() const {
    return "GearmanClientHandler";
}

void GearmanClientHandler::flush(ChannelHandlerContext& ctx, const ChannelFuturePtr& future) {
    OutboundMessageContext::MessageQueue& in = ctx.getOutboundMessageQueue();

    while (!in.empty()) {
        GearmanMessagePtr& msg = in.front();
        if (msg) {
            msgs.push_back(msg);
            CodecUtil<GearmanMessagePtr>::unfoldAndAdd(ctx, msg, false);
        }
        in.pop_front();
    }

    ctx.flush();
}

void GearmanClientHandler::channelActive(ChannelHandlerContext& ctx) {
    channel = ctx.getChannel();
}

}
}