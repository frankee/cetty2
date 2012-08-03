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
#include <cetty/channel/ChannelHandlerContext.h>
#include <cetty/channel/SocketAddress.h>
#include <cetty/handler/codec/CodecUtil.h>
#include <cetty/buffer/ChannelBuffer.h>
#include <cetty/buffer/ChannelBuffers.h>

#include <cetty/gearman/GearmanMessage.h>

namespace cetty {
namespace gearman {

using namespace cetty::channel;
using namespace cetty::buffer;
using namespace cetty::handler::codec;

GearmanWorkerHandler::GearmanWorkerHandler()
    : isSleep(false), channel(0), grabIdleCount(0),maxGrabIdleCount(3) {
}

GearmanWorkerHandler::GearmanWorkerHandler(int maxGrabIdleCount):isSleep(false),
    channel(0),grabIdleCount(0) {
    maxGrabIdleCount = maxGrabIdleCount;
}

GearmanWorkerHandler::GearmanWorkerHandler(int maxGrabIdleCount,
        const std::map<std::string, GrabJobCallback> workerFunctorMap)
    :isSleep(false), channel(0), grabIdleCount(0) {
    maxGrabIdleCount = maxGrabIdleCount;
    workerFunctors = workerFunctorMap;
}

GearmanWorkerHandler::~GearmanWorkerHandler() {
}

void GearmanWorkerHandler::registerFunction(const std::string& functionName,
        ChannelHandlerContext& ctx) {
    GearmanMessagePtr msg = GearmanMessage::createCandoMessage(functionName);

    OutboundMessageContext* context
        = ctx.nextOutboundMessageHandlerContext<OutboundMessageContext>();

    if (context) {
        context->addOutboundMessage(msg);
        ctx.flush();
    }
}

void GearmanWorkerHandler::registerWorker(const std::string& functionName,
        const GrabJobCallback& worker) {
    workerFunctors.insert(std::pair<std::string, GrabJobCallback>(functionName, worker));
}

void GearmanWorkerHandler::handleJob(const GearmanMessagePtr& gearmanMessage,
                                     ChannelHandlerContext& ctx) {
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

            OutboundMessageContext* context
                = ctx.nextOutboundMessageHandlerContext<OutboundMessageContext>();

            if (context) {
                context->addOutboundMessage(message);
                ctx.flush();
            }
        }
        else {
            ctx.fireMessageUpdated();
        }
    }
    else {
        ctx.fireMessageUpdated();
    }
}

void GearmanWorkerHandler::grabJob(ChannelHandlerContext& ctx) {
    GearmanMessagePtr msg = GearmanMessage::createGrabJobMessage();
    if (CodecUtil<GearmanMessagePtr>::unfoldAndAdd(ctx, msg, true)) {
        ctx.flush();
    }
}

void GearmanWorkerHandler::grabJobUnique(ChannelHandlerContext& ctx) {
    GearmanMessagePtr msg = GearmanMessage::createGrabJobUniqMessage();
    if (CodecUtil<GearmanMessagePtr>::unfoldAndAdd(ctx, msg, true)) {
        ctx.flush();
    }
}

void GearmanWorkerHandler::preSleep(ChannelHandlerContext& ctx) {
    GearmanMessagePtr msg = GearmanMessage::createPreSleepMessage();
    if (CodecUtil<GearmanMessagePtr>::unfoldAndAdd(ctx, msg, true)) {
        ctx.flush();
    }
}

void GearmanWorkerHandler::messageReceived(ChannelHandlerContext& ctx,
        const GearmanMessagePtr& msg) {
    std::string  data;
    std::string jobhandle;
    std::vector<std::string> params;

    if (msg) {
        switch (msg->getType()) {
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
            params = msg->getParameters();
            std::cout<<"the job-handler is "<<params[0]<<std::endl;
            std::cout<<"the function name is "<<params[1]<<std::endl;
            data = ChannelBuffers::hexDump(msg->getData());
            std::cout<<"the arg data is "<<data<<std::endl;
#endif
            handleJob(msg, ctx);
            grabIdleCount = 0;
            grabJob(ctx);
            break;


        case GearmanMessage::JOB_ASSIGN_UNIQ:
            std::cout<<"the GRAB_JOB_UNIQ  RSP  is JOB_ASSIGN_UNIQ which have uniqId assined from client "<< std::endl;
            params = msg->getParameters();
            std::cout<<"the job-handler is "<<params[0]<<std::endl;
            std::cout<<"the function name is "<<params[1]<<std::endl;
            std::cout<<"the unique ID is "<<params[2]<<std::endl;
            data = ChannelBuffers::hexDump(msg->getData());
            std::cout<<"the arg data is "<<data<<std::endl;

            grabIdleCount = 0;
            handleJob(msg, ctx);
            break;

        case GearmanMessage::ECHO_RES:
            data = ChannelBuffers::hexDump(msg->getData());
            std::cout<<"the ECHO_RES is ok "<< std::endl;
            std::cout<<"the data is  "<<data<<std::endl;
            break;

        case GearmanMessage::ERROR:

            //do something
            break;

        default:
            break;
        }
    }
}

ChannelHandlerPtr GearmanWorkerHandler::clone() {
    return ChannelHandlerPtr(new GearmanWorkerHandler(maxGrabIdleCount, workerFunctors));
}

std::string GearmanWorkerHandler::toString() const {
    return "GearmanWorkerHandler";
}

void GearmanWorkerHandler::flush(OutboundMessageContext& ctx,
                                 const ChannelFuturePtr& future) {
    OutboundMessageContext::MessageQueue& in
        = ctx.getOutboundMessageQueue();

    while (!in.empty()) {
        const GearmanMessagePtr& message = in.front();

        switch (message->getType()) {
        case GearmanMessage::WORK_COMPLETE:
            CodecUtil<GearmanMessagePtr>::unfoldAndAdd(ctx, message, true);
            break;

        default:
            break;
        }
    }

    ctx.flush();
}

void GearmanWorkerHandler::channelActive(ChannelHandlerContext& ctx) {
    channel = ctx.getChannel();

    if (channel) {
        CallbackMap::iterator itr = workerFunctors.begin();

        for (; itr != workerFunctors.end(); ++itr) {
            registerFunction(itr->first,ctx);
        }

        grabJob(ctx);
    }
}

}
}