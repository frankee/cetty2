#if !defined(CETTY_SHUTTLE_SHUTTLEBACKEND_H)
#define CETTY_SHUTTLE_SHUTTLEBACKEND_H

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
#include <string>
#include <cetty/util/CurrentThread.h>
#include <cetty/channel/ChannelPtr.h>
#include <cetty/channel/EventLoopPool.h>
#include <cetty/channel/ChannelPipeline.h>
#include <cetty/service/builder/ClientBuilder.h>
#include <cetty/shuttle/ShuttleConfig.cnf.h>
#include <cetty/shuttle/protobuf/ProtobufProxyMessagePtr.h>

namespace cetty {
namespace shuttle {

using namespace cetty::util;
using namespace cetty::channel;
using namespace cetty::config;
using namespace cetty::service;
using namespace cetty::service::builder;
using namespace cetty::shuttle::protobuf;

class ShuttleBackend;
typedef boost::shared_ptr<ShuttleBackend> ShuttleBackendPtr;

class ShuttleBackend : private boost::noncopyable {
public:
    typedef ClientBuilder<ProtobufProxyMessagePtr, ProtobufProxyMessagePtr> Builder;

public:
    static void configure(const std::map<std::string, Backend*>& config,
                          const EventLoopPoolPtr& pool);

    static const ChannelPtr& getChannel(const std::string& method);

public:
    ShuttleBackend(const EventLoopPtr& loop);

    void configure(const std::map<std::string, Backend*>& config);
    const ChannelPtr& channel(const std::string& method) const;

private:
    void init();
    bool initializeChannel(ChannelPipeline& pipeline);

private:
    Builder builder_;
    std::map<std::string, ChannelPtr> channels_;

private:
    static ChannelPtr emptyChannel_;
    static std::map<ThreadId, ShuttleBackendPtr> backends_;
};

}
}

#endif //#if !defined(CETTY_SHUTTLE_SHUTTLEBACKEND_H)

// Local Variables:
// mode: c++
// End:
