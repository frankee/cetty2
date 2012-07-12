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

#include <boost/function.hpp>
#include <boost/intrusive_ptr.hpp>
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
    virtual ~GearmanWorkerHandler();

    void registerWorker(const std::string& functionName,
        const GrabJobCallback& worker);

    virtual void messageReceived(ChannelHandlerContext& ctx,
        const MessageEvent& e);
    virtual void writeRequested(ChannelHandlerContext& ctx,
        const MessageEvent& e);

    virtual ChannelHandlerPtr clone();
    virtual std::string toString() const;

private:
    static void start();
    static void grabJob();
    static void grabJobUnique();
    static void preSleep();
};

}
}

#endif //#if !defined(CETTY_GEARMAN_GEARMANWORKERHANDLER_H)

// Local Variables:
// mode: c++
// End:
