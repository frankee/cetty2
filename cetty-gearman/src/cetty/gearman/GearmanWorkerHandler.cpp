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
#include <cetty/channel/InetAddress.h>
#include <cetty/buffer/Unpooled.h>
#include <cetty/buffer/ChannelBuffer.h>
#include <cetty/buffer/ChannelBufferUtil.h>
#include <cetty/logging/LoggerHelper.h>

#include <cetty/gearman/protocol/GearmanMessage.h>
#include <cetty/gearman/protocol/commands/Worker.h>

namespace cetty {
namespace gearman {

using namespace cetty::channel;
using namespace cetty::buffer;

GearmanWorkerHandler::GearmanWorkerHandler()
    : isSleep_(false),
      grabIdleCount_(0),
      maxGrabIdleCount_(3),
      inboundTransfer_(),
      inboundContainer_(),
      outboundTransfer_(),
      outboundContainer_() {
}

GearmanWorkerHandler::GearmanWorkerHandler(int maxGrabIdleCount)
    :isSleep_(false),
     grabIdleCount_(0),
     maxGrabIdleCount_(maxGrabIdleCount),
     inboundTransfer_(),
     inboundContainer_(),
     outboundTransfer_(),
     outboundContainer_() {
}

GearmanWorkerHandler::GearmanWorkerHandler(int maxGrabIdleCount,
        const std::map<std::string, GrabJobCallback> workerFunctors)
    :isSleep_(false),
     grabIdleCount_(0),
     maxGrabIdleCount_(maxGrabIdleCount),
     workerFunctors_(workerFunctors),
     inboundTransfer_(),
     inboundContainer_(),
     outboundTransfer_(),
     outboundContainer_() {
}

GearmanWorkerHandler::~GearmanWorkerHandler() {
}

void GearmanWorkerHandler::registerWorker(const std::string& functionName,
        const GrabJobCallback& worker) {
    workerFunctors_.insert(
        std::pair<std::string, GrabJobCallback>(functionName, worker));
}

void GearmanWorkerHandler::handleJob(const GearmanMessagePtr& gearmanMessage,
                                     ChannelHandlerContext& ctx) {
    const std::vector<std::string>& parameters = gearmanMessage->parameters();
    const std::string& functionName = parameters[1];
    CallbackMap::iterator itr = workerFunctors_.find(functionName);

    if (itr != workerFunctors_.end()) {
        const GrabJobCallback& callback = itr->second;

        if (callback) {
            GearmanMessagePtr message = callback(gearmanMessage);

            DLOG_DEBUG << "handler: " << parameters.front()
                       << "result: " << ChannelBufferUtil::hexDump(message->data());

            if (outboundTransfer_->unfoldAndAdd(message)) {
                ctx.flush();
            }

            return;
        }
    }

    inboundTransfer_->unfoldAndAdd(gearmanMessage);
    ctx.fireMessageUpdated();
}

void GearmanWorkerHandler::registerFunction(const std::string& functionName,
        ChannelHandlerContext& ctx) {
    GearmanMessagePtr msg = commands::candoMessage(functionName);

    if (outboundTransfer_->unfoldAndAdd(msg)) {
        ctx.flush();
    }
}

void GearmanWorkerHandler::grabJob(ChannelHandlerContext& ctx) {
    GearmanMessagePtr msg = commands::grabJobMessage();

    if (outboundTransfer_->unfoldAndAdd(msg)) {
        ctx.flush();
    }
}

void GearmanWorkerHandler::grabJobUnique(ChannelHandlerContext& ctx) {
    GearmanMessagePtr msg = commands::grabJobUniqMessage();

    if (outboundTransfer_->unfoldAndAdd(msg)) {
        ctx.flush();
    }
}

void GearmanWorkerHandler::preSleep(ChannelHandlerContext& ctx) {
    GearmanMessagePtr msg = commands::preSleepMessage();

    if (outboundTransfer_->unfoldAndAdd(msg)) {
        ctx.flush();
    }
}

void GearmanWorkerHandler::messageReceived(ChannelHandlerContext& ctx) {
    std::string data;
    std::string jobhandle;

    InboundQueue& queue = inboundContainer_->getMessages();

    while (!queue.empty()) {
        const GearmanMessagePtr& msg = queue.front();

        if (msg) {
            const std::vector<std::string>& params = msg->parameters();

            switch (msg->type()) {
            case GearmanMessage::NOOP:
                LOG_INFO << "the NOOP is received, wake up to do job";
                grabJob(ctx);
                break;

            case GearmanMessage::NO_JOB:
                LOG_INFO << "the GRAB_JOB  RSP is NO_JOB";

                if (++grabIdleCount_ > maxGrabIdleCount_) {
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
                           << "arg data: " << ChannelBufferUtil::hexDump(msg->data());

                handleJob(msg, ctx);
                grabIdleCount_ = 0;
                grabJob(ctx);
                break;


            case GearmanMessage::JOB_ASSIGN_UNIQ:
                LOG_INFO << "the GRAB_JOB_UNIQ  RSP  is JOB_ASSIGN_UNIQ which have uniqId assigned from client";

                DLOG_DEBUG << "JOB_ASSIGN_UNIQ, job-handler: " << params[0]
                           << "function name: " << params[1]
                           << "unique ID: " << params[2]
                           << "arg data: " << ChannelBufferUtil::hexDump(msg->data());

                grabIdleCount_ = 0;
                handleJob(msg, ctx);
                break;

            case GearmanMessage::ECHO_RES:
                LOG_INFO << "the ECHO_RES is ok";

                DLOG_DEBUG << "ECHO_RES, data: " << ChannelBufferUtil::hexDump(msg->data());
                break;

            case GearmanMessage::ERROR:
                LOG_WARN << "ERROR";
                break;

            default:
                break;
            }
        }

        queue.pop_front();
    }
}

void GearmanWorkerHandler::flush(ChannelHandlerContext& ctx,
                                 const ChannelFuturePtr& future) {
    OutboundQueue& queue = outboundContainer_->getMessages();

    while (!queue.empty()) {
        const GearmanMessagePtr& message = queue.front();

        switch (message->type()) {
        case GearmanMessage::WORK_COMPLETE:
            outboundTransfer_->unfoldAndAdd(message);
            break;

        default:
            break;
        }

        queue.pop_front();
    }

    ctx.flush();
}

void GearmanWorkerHandler::channelActive(ChannelHandlerContext& ctx) {
    CallbackMap::iterator itr = workerFunctors_.begin();

    for (; itr != workerFunctors_.end(); ++itr) {
        registerFunction(itr->first, ctx);
    }

    grabJob(ctx);
}

void GearmanWorkerHandler::registerTo(Context& ctx) {
    inboundTransfer_ = ctx.inboundTransfer();
    inboundContainer_ = ctx.inboundContainer();

    outboundTransfer_ = ctx.outboundTransfer();
    outboundContainer_ = ctx.outboundContainer();

    ctx.setChannelActiveCallback(boost::bind(
                                     &GearmanWorkerHandler::channelActive,
                                     this,
                                     _1));

    ctx.setChannelMessageUpdatedCallback(boost::bind(
            &GearmanWorkerHandler::messageReceived,
            this,
            _1));

    ctx.setFlushFunctor(boost::bind(
                            &GearmanWorkerHandler::flush,
                            this,
                            _1,
                            _2));
}

}
}
