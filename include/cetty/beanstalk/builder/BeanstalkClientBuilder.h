#if !defined(CETTY_BEANSTALK_BUILDER_BEANSTALKCLIENTBUILDER_H)
#define CETTY_BEANSTALK_BUILDER_BEANSTALKCLIENTBUILDER_H

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
/*
 *      Author: chenhl
 */

#include <cetty/beanstalk/protocol/BeanstalkReply.h>
#include <cetty/beanstalk/protocol/BeanstalkCommand.h>
#include <cetty/service/builder/ClientBuilder.h>

namespace cetty {
namespace beanstalk {
namespace builder {

using namespace cetty::service::builder;
using namespace cetty::beanstalk::protocol;

class BeanstalkClientBuilder {
public:
    typedef ClientBuilder<BeanstalkCommandPtr, BeanstalkReplyPtr> ClientBuilderType;

public:
    BeanstalkClientBuilder();
    BeanstalkClientBuilder(int threadCnt);
    BeanstalkClientBuilder(const EventLoopPtr& eventLoop);
    BeanstalkClientBuilder(const EventLoopPoolPtr& eventLoopPool);

    void addConnection(const std::string& host, int port, int limit = 1) {
        builder_.addConnection(host, port, limit);
    }

    ChannelPtr build() {
        return builder_.build();
    }

private:
    void init();

private:
    ClientBuilderType builder_;
};

}
}
}

#endif //#if !defined(CETTY_BEANSTALK_BUILDER_BEANSTALKCLIENTBUILDER_H)

// Local Variables:
// mode: c++
// End:
