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
#include <cetty/buffer/Unpooled.h>
#include <cetty/buffer/ChannelBuffer.h>
#include <cetty/buffer/ChannelBufferUtil.h>
#include <cetty/logging/LoggerHelper.h>

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

    if (outboundTransfer.unfoldAndAdd(msg)) {
        ctx.flush();
    }
}

void GearmanWorkerHandler::registerWorker(const std::string& functionName,
        const GrabJobCallback& worker) {
    workerFunctors.insert(std::pair<std::string, GrabJobCallback>(functionName, worker));
}

void GearmanWorkerHandler::handleJob(const GearmanMessagePtr& gearmanMessage,
                                     ChannelHandlerContext& ctx) {
    const std::vector<std::string>& parameters = gearmanMessage->getParameters();
    const std::string& functionName = parameters[1];
    CallbackMap::iterator itr = workerFunctors.find(functionName);

    if (itr != workerFunctors.end()) {
        const GrabJobCallback& callback = itr->second;

        if (callback) {
            GearmanMessagePtr message = callback(gearmanMessage);

            DLOG_DEBUG << "handler: " << parameters.front()
                       << "result: " << ChannelBufferUtil::hexDump(message->getData());

            if (outboundTransfer.unfoldAndAdd(message)) {
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

    if (outboundTransfer.unfoldAndAdd(msg)) {
        ctx.flush();
    }
}

void GearmanWorkerHandler::grabJobUnique(ChannelHandlerContext& ctx) {
    GearmanMessagePtr msg = GearmanMessage::createGrabJobUniqMessage();

    if (outboundTransfer.unfoldAndAdd(msg)) {
        ctx.flush();
    }
}

void GearmanWorkerHandler::preSleep(ChannelHandlerContext& ctx) {
    GearmanMessagePtr msg = GearmanMessage::createPreSleepMessage();

    if (outboundTransfer.unfoldAndAdd(msg)) {
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
            LOG_INFO << "the NOOP is received, wake up to do job";
            grabJob(ctx);
            break;

        case GearmanMessage::NO_JOB:
            LOG_INFO << "the GRAB_JOB  RSP is NO_JOB";

            if (++grabIdleCount > maxGrabIdleCount) {
                preSleep(ctx);
            }
            else {
                grabJob(ctx);
            }

            break;

        case GearmanMessage::JOB_ASSIGN:
            LOG_INFO << "the response of GRAB_JOB is JOB_ASSIGN, do the job now";

            DLOG_DEBUG << "JOB_ASSIGN, job-handler: " << params[0]
                       << "function name: " << params[1]
                       << "arg data: " << ChannelBufferUtil::hexDump(msg->getData());

            handleJob(msg, ctx);
            grabIdleCount = 0;
            grabJob(ctx);
            break;


        case GearmanMessage::JOB_ASSIGN_UNIQ:
            LOG_INFO << "the GRAB_JOB_UNIQ  RSP  is JOB_ASSIGN_UNIQ which have uniqId assigned from client";

            DLOG_DEBUG << "JOB_ASSIGN_UNIQ, job-handler: " << params[0]
                       << "function name: " << params[1]
                       << "unique ID: " << params[2]
                       << "arg data: " << ChannelBufferUtil::hexDump(msg->getData());

            grabIdleCount = 0;
            handleJob(msg, ctx);
            break;

        case GearmanMessage::ECHO_RES:
            LOG_INFO << "the ECHO_RES is ok";

            DLOG_DEBUG << "ECHO_RES, data: " << ChannelBufferUtil::hexDump(msg->getData());
            break;

        case GearmanMessage::ERROR:
            LOG_WARN << "ERROR";
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

void GearmanWorkerHandler::flush(ChannelHandlerContext& ctx,
                                 const ChannelFuturePtr& future) {
    while (!outboundQueue.empty()) {
        const GearmanMessagePtr& message = outboundQueue.front();

        switch (message->getType()) {
        case GearmanMessage::WORK_COMPLETE:
            outboundTransfer.unfoldAndAdd(message);
            break;

        default:
            break;
        }

        outboundQueue.pop_front();
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
