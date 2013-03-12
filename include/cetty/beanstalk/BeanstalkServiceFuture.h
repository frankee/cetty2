/*
 * BeanstalkServiceFuture.h
 *
 *  Created on: Mar 12, 2013
 *      Author: chenhl
 */

#ifndef BEANSTALKSERVICEFUTURE_H_
#define BEANSTALKSERVICEFUTURE_H_

#include <cetty/service/ServiceFuture.h>
#include <cetty/beanstalk/protocol/BeanstalkReply.h>

namespace cetty {
namespace beanstalk {

typedef cetty::service::ServiceFuture<protocol::BeanstalkReplyPtr> BeanstalkServiceFuture;
typedef boost::intrusive_ptr<BeanstalkServiceFuture> BeanstalkServiceFuturePtr;

}
}


#endif /* BEANSTALKSERVICEFUTURE_H_ */
