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
#include <stack>
#include <boost/function.hpp>
#include <boost/intrusive_ptr.hpp>
#include <cetty/channel/ChannelPtr.h>
#include <cetty/channel/ChannelMessageHandlerAdapter.h>
#include <cetty/gearman/GearmanMessagePtr.h>

namespace cetty {
namespace gearman {

using namespace cetty::channel;

class GearmanWorkerHandler;
typedef boost::intrusive_ptr<GearmanWorkerHandler> GearmanWorkerHandlerPtr;

class GearmanWorkerHandler
    : public ChannelMessageHandlerAdapter<GearmanMessagePtr,
      GearmanMessagePtr,
      GearmanMessagePtr,
          GearmanMessagePtr> {
public:
    typedef boost::function1<GearmanMessagePtr, const GearmanMessagePtr&> GrabJobCallback;

public:
    GearmanWorkerHandler();
    GearmanWorkerHandler(int maxGrabIdleCount);
    GearmanWorkerHandler(int maxGrabIdleCount,
                         const std::map<std::string, GrabJobCallback> workerFunctorMap);

    virtual ~GearmanWorkerHandler();

    virtual void channelActive(ChannelHandlerContext& ctx);

    virtual ChannelHandlerPtr clone();
    virtual std::string toString() const;

    void registerWorker(const std::string& functionName,
                        const GrabJobCallback& worker);

    virtual void messageReceived(ChannelHandlerContext& ctx,
                                 const GearmanMessagePtr& msg);

    virtual void flush(ChannelHandlerContext& ctx,
                       const ChannelFuturePtr& future);

private:
    void handleJob(const GearmanMessagePtr& gearmanMessage,
                   ChannelHandlerContext& ctx);

private:
    void registerFunction(const std::string& functionName,
                          ChannelHandlerContext& ctx);

    void preSleep(ChannelHandlerContext& ctx);
    void grabJob(ChannelHandlerContext& ctx);
    void grabJobUnique(ChannelHandlerContext& ctx);

private:
    typedef std::map<std::string, GrabJobCallback> CallbackMap;

private:
    bool isSleep;
    int maxGrabIdleCount;
    int grabIdleCount;
    ChannelPtr channel;
    CallbackMap workerFunctors;
};

}
}

#endif //#if !defined(CETTY_GEARMAN_GEARMANWORKERHANDLER_H)

// Local Variables:
// mode: c++
// End:
