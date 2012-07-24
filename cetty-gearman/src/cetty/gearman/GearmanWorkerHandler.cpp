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
#include <cetty/channel/DownstreamMessageEvent.h>
#include <cetty/channel/UpstreamMessageEvent.h>
#include <cetty/channel/SocketAddress.h>

#include <cetty/gearman/GearmanMessage.h>

namespace cetty {
namespace gearman {

using namespace cetty::channel;
using namespace cetty::buffer;

GearmanWorkerHandler::GearmanWorkerHandler()
    : isSleep(false), channel(0), grabIdleCount(0),maxGrabIdleCount(3) {
}

GearmanWorkerHandler::GearmanWorkerHandler(int maxGrabIdleCount):isSleep(false),
    channel(0),grabIdleCount(0) {
    maxGrabIdleCount = maxGrabIdleCount;
}

GearmanWorkerHandler::GearmanWorkerHandler(int maxGrabIdleCount,
        const std::map<std::string, GrabJobCallback> workerFunctorMap):isSleep(false),channel(0),grabIdleCount(0) {
    maxGrabIdleCount = maxGrabIdleCount;
    workerFunctors = workerFunctorMap;
}

GearmanWorkerHandler::~GearmanWorkerHandler() {
}

void GearmanWorkerHandler::registerFunction(const std::string& functionName,
        ChannelHandlerContext& ctx) {
    ChannelMessage msg(GearmanMessage::createCandoMessage(functionName));
    DownstreamMessageEvent evt(channel,channel->getSucceededFuture(),msg,channel->getLocalAddress());
    ctx.sendDownstream(evt);
}

void GearmanWorkerHandler::registerWorker(const std::string& functionName,
        const GrabJobCallback& worker) {
    workerFunctors.insert(std::pair<std::string, GrabJobCallback>(functionName, worker));
}


void GearmanWorkerHandler::channelConnected(ChannelHandlerContext& ctx,
        const ChannelStateEvent& e) {
    channel = ctx.getChannel();

    if (channel) {
        CallbackMap::iterator itr = workerFunctors.begin();

        for (; itr != workerFunctors.end(); ++itr) {
            registerFunction(itr->first,ctx);
        }

        ChannelMessage msg;
        grabJob(ctx);
    }
}



void GearmanWorkerHandler::handleJob(const GearmanMessagePtr& gearmanMessage,
                                     ChannelHandlerContext& ctx,
                                     const MessageEvent& e) {
    const std::string& functionName = gearmanMessage->getParamater(1);
    CallbackMap::iterator itr;
    itr = workerFunctors.find(functionName);

    if (itr != workerFunctors.end()) {
        const GrabJobCallback& callback = itr->second;

        if (callback) {
            GearmanMessagePtr message = callback(gearmanMessage);

            std::string ret;
            ret = ChannelBuffers::hexDump(message->getData());
            std::cout<<"the ret is  "<< ret <<std::endl;
            std::cout << "handleJob::the msg is  " << ret << std::endl;
            std::cout << "handleJob::the msg is  " << (message->getParameters())[0] <<std::endl;

            //if (channel) {
            //    channel->write(ChannelMessage(message));
            //}
            DownstreamMessageEvent msg(e.getChannel(),e.getFuture(),ChannelMessage(message),e.getRemoteAddress());
            ctx.sendDownstream(msg);
        }
        else {
            ctx.sendUpstream(e);
        }
    }
    else {
        ctx.sendUpstream(e);
    }
}

void GearmanWorkerHandler::grabJob(ChannelHandlerContext& ctx) {
    ChannelMessage msg(GearmanMessage::createGrabJobMessage());
    /*if(m_channel)
    {
        m_channel->write(ChannelMessage(msg));
    }*/

    DownstreamMessageEvent evt(channel,channel->getSucceededFuture(),msg,channel->getRemoteAddress());
    ctx.sendDownstream(evt);
}

void GearmanWorkerHandler::grabJobUnique(ChannelHandlerContext& ctx) {
    ChannelMessage msg(GearmanMessage::createGrabJobUniqMessage());
    DownstreamMessageEvent evt(channel,channel->getSucceededFuture(),msg,channel->getRemoteAddress());
    ctx.sendDownstream(evt);
    //m_channel->write(ChannelMessage(msg));
}

void GearmanWorkerHandler::preSleep(ChannelHandlerContext& ctx) {
    ChannelMessage msg(GearmanMessage::createPreSleepMessage());
    DownstreamMessageEvent evt(channel,channel->getSucceededFuture(),msg,channel->getRemoteAddress());
    ctx.sendDownstream(evt);
    //m_channel->write(ChannelMessage(msg));
}

void GearmanWorkerHandler::messageReceived(ChannelHandlerContext& ctx,
        const MessageEvent& e) {
    GearmanMessagePtr message = e.getMessage().smartPointer<GearmanMessage>();
    std::string  data;
    std::string jobhandle;
    std::vector<std::string> params;

    if (!message) {
        ctx.sendUpstream(e);
    }

    switch (message->getType()) {
    case GearmanMessage::NOOP:
        std::cout<<"the NOOP is received, wake up to do job"<< std::endl;
        grabJob(ctx);
        break;

    case GearmanMessage::NO_JOB:
        std::cout<<"the GRAB_JOB  RSP is NO_JOB "<< std::endl;

        if (++grabIdleCount > maxGrabIdleCount) {
            preSleep(ctx);
        }
        else {
            grabJob(ctx);
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
        handleJob(message,ctx,e);
        grabIdleCount = 0;
        grabJob(ctx);
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

    case GearmanMessage::ERROR:

        //do something
    default:
        ctx.sendUpstream(e);
    }
}

void GearmanWorkerHandler::writeRequested(ChannelHandlerContext& ctx,
        const MessageEvent& e) {
    GearmanMessagePtr message = e.getMessage().smartPointer<GearmanMessage>();
    std::string jobhandle;
    DownstreamMessageEvent* evt;
    ChannelMessage* msg;

    if (!message) {
        ctx.sendDownstream(e);
    }

    switch (message->getType()) {
        //below is for protobufRep
    case GearmanMessage::WORK_COMPLETE:
        msg = new ChannelMessage(message);
        evt = new DownstreamMessageEvent(e.getChannel(),e.getFuture(),*msg,e.getRemoteAddress());
        ctx.sendDownstream(*evt);
        break;

    default:
        ctx.sendDownstream(e);
        break;
    }
}

ChannelHandlerPtr GearmanWorkerHandler::clone() {
    return ChannelHandlerPtr(new GearmanWorkerHandler(maxGrabIdleCount, workerFunctors));
}

std::string GearmanWorkerHandler::toString() const {
    return "GearmanWorkerHandler";
}

}
}