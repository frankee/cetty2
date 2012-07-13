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

#include <cetty/gearman/GearmanWorkerHandler.h>

#include <string>
#include <iostream>

#include <cetty/channel/Channel.h>
#include <cetty/channel/Channels.h>
#include <cetty/channel/MessageEvent.h>
#include <cetty/channel/ChannelMessage.h>
#include <cetty/channel/ChannelStateEvent.h>
#include <cetty/channel/ChannelHandlerContext.h>

#include <cetty/gearman/GearmanMessage.h>

namespace cetty {
namespace gearman {

using namespace cetty::channel;
using namespace cetty::buffer;

GearmanWorkerHandler::GearmanWorkerHandler()
    : channel(0), grabIdleCount(0) {
}

GearmanWorkerHandler::~GearmanWorkerHandler() {
}

void GearmanWorkerHandler::channelConnected(ChannelHandlerContext& ctx,
        const ChannelStateEvent& e) {
    channel = ctx.getChannel();

    if (channel) {
        CallbackMap::const_iterator itr = workerFunctors.begin();

        for (; itr != workerFunctors.end(); ++itr) {
            registerFunction(itr->first);
        }

        grabJob();
    }
}

void GearmanWorkerHandler::registerWorker(const std::string& functionName,
        const GrabJobCallback& worker) {
    //GearmanMessagePtr msg(GearmanMessage::createCandoMessage(functionName));
    //channel->write(ChannelMessage(msg));
    //add the worker to the funcMap
    workerFunctors.insert(std::make_pair(functionName, worker));
}

void GearmanWorkerHandler::handleJob(const GearmanMessagePtr& gearmanMessage,
                                     ChannelHandlerContext& ctx,
                                     const MessageEvent& e) {

    const std::string& functionName = gearmanMessage->getParamater(1);
    CallbackMap::const_iterator itr = workerFunctors.find(functionName);

    if (itr != workerFunctors.end()) {
        const GrabJobCallback& callback = itr->second;

        if (callback) {
            GearmanMessagePtr message = callback(gearmanMessage);
            //
        }
        else {
            ctx.sendUpstream(e);
        }
    }
}

void GearmanWorkerHandler::grabJob() {
    GearmanMessagePtr msg(GearmanMessage::createGrabJobMessage());
    channel->write(ChannelMessage(msg));
}

void GearmanWorkerHandler::grabJobUnique() {
    GearmanMessagePtr msg(GearmanMessage::createGrabJobUniqMessage());
    channel->write(ChannelMessage(msg));
}

void GearmanWorkerHandler::preSleep() {
    GearmanMessagePtr msg(GearmanMessage::createPreSleepMessage());
    channel->write(ChannelMessage(msg));
}

void GearmanWorkerHandler::messageReceived(ChannelHandlerContext& ctx,
        const MessageEvent& e) {
    GearmanMessagePtr message = e.getMessage().smartPointer<GearmanMessage>();
    std::string  data;
    std::vector<std::string> params;

    if (!message) {
        ctx.sendUpstream(e);
    }

    switch (message->getType()) {
    case GearmanMessage::NOOP:
        std::cout<<"the NOOP is received, wake up to do job"<< std::endl;
        grabJob();
        break;

    case GearmanMessage::NO_JOB:
        std::cout<<"the GRAB_JOB  RSP is NO_JOB "<< std::endl;

        if (++grabIdleCount > maxGrabIdleCount) {
            preSleep();
        }
        else {
            grabJob();
        }

        break;

    case GearmanMessage::JOB_ASSIGN:
        std::cout<<"the GRAB_JOB  RSP  is JOB_ASSIGN,do the job now "<< std::endl;

#if defined(_DEBUG)
        params = message->getParameters();
        std::cout<<"the job-handler is "<<params[0]<<std::endl;
        std::cout<<"the function name is "<<params[1]<<std::endl;
        data = ChannelBuffers::hexDump(message->getData());
        std::cout<<"the arg data is "<<data<<std::endl;
#endif

        handleJob(message, ctx, e);
        grabIdleCount = 0;
        break;

    case GearmanMessage::JOB_ASSIGN_UNIQ:
        std::cout<<"the GRAB_JOB_UNIQ  RSP  is JOB_ASSIGN_UNIQ which have uniqId assined from client "<< std::endl;
        params = message->getParameters();
        std::cout<<"the job-handler is "<<params[0]<<std::endl;
        std::cout<<"the function name is "<<params[1]<<std::endl;
        std::cout<<"the unique ID is "<<params[2]<<std::endl;
        data = ChannelBuffers::hexDump(message->getData());
        std::cout<<"the arg data is "<<data<<std::endl;

        grabIdleCount = 0;
        handleJob(message, ctx, e);
        break;

    case GearmanMessage::ECHO_RES:
        data = ChannelBuffers::hexDump(message->getData());
        std::cout<<"the ECHO_RES is ok "<< std::endl;
        std::cout<<"the data is  "<<data<<std::endl;
        break;

    default:
        ctx.sendUpstream(e);
    }
}

void GearmanWorkerHandler::writeRequested(ChannelHandlerContext& ctx,
        const MessageEvent& e) {

    ctx.sendDownstream(e);
}

ChannelHandlerPtr GearmanWorkerHandler::clone() {
    return ChannelHandlerPtr(new GearmanWorkerHandler);
}

std::string GearmanWorkerHandler::toString() const {
    return "GearmanWorkerHandler";
}

}
}