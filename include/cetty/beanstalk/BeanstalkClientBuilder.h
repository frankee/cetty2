/*
 * BeanstalkClientBuilder.h
 *
 *  Created on: Mar 11, 2013
 *      Author: chenhl
 */

#ifndef BEANSTALKCLIENTBUILDER_H_
#define BEANSTALKCLIENTBUILDER_H_

#include <cetty/beanstalk/protocol/BeanstalkReply.h>
#include <cetty/beanstalk/protocol/BeanstalkCommand.h>
#include <cetty/service/builder/ClientBuilder.h>

namespace cetty {
namespace beanstalk {

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


#endif /* BEANSTALKCLIENTBUILDER_H_ */
