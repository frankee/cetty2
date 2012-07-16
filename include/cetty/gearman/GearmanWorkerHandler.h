#if !defined(CETTY_GEARMAN_GEARMANWORKERHANDLER_H)
#define CETTY_GEARMAN_GEARMANWORKERHANDLER_H

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

#include <map>
#include <boost/function.hpp>
#include <boost/intrusive_ptr.hpp>
#include <cetty/channel/ChannelFwd.h>
#include <cetty/channel/SimpleChannelHandler.h>
#include <cetty/gearman/GearmanMessagePtr.h>

namespace cetty {
namespace gearman {

class GearmanWorkerHandler;
typedef boost::intrusive_ptr<GearmanWorkerHandler> GearmanWorkerHandlerPtr;

using namespace cetty::channel;

class GearmanWorkerHandler : public cetty::channel::SimpleChannelHandler {
public:
    typedef boost::function1<GearmanMessagePtr, const GearmanMessagePtr&> GrabJobCallback;

public:
    GearmanWorkerHandler();
    GearmanWorkerHandler(int maxGrabIdleCount);
    GearmanWorkerHandler(int maxGrabIdleCount,
        const std::map<std::string, GrabJobCallback>& workerFunctors);
    virtual ~GearmanWorkerHandler();

    virtual void channelConnected(ChannelHandlerContext& ctx, const ChannelStateEvent& e);

    virtual void messageReceived(ChannelHandlerContext& ctx,
                                 const MessageEvent& e);
    virtual void writeRequested(ChannelHandlerContext& ctx,
                                const MessageEvent& e);

    virtual ChannelHandlerPtr clone();
    virtual std::string toString() const;

    void registerWorker(const std::string& functionName,
                        const GrabJobCallback& worker);

private:
    void handleJob(const GearmanMessagePtr& gearmanMessage,
                   ChannelHandlerContext& ctx,
                   const MessageEvent& e);

private:
    void registerFunction(const std::string& functionName);

    void preSleep();
    void grabJob();
    void grabJobUnique();

private:
    typedef std::map<std::string, GrabJobCallback> CallbackMap;

private:
    bool isSleep;
    int m_maxGrabIdleCount;
    int m_grabIdleCount;
    ChannelPtr m_channel;
    CallbackMap m_workerFunctors;
};

}
}

#endif //#if !defined(CETTY_GEARMAN_GEARMANWORKERHANDLER_H)

// Local Variables:
// mode: c++
// End:
