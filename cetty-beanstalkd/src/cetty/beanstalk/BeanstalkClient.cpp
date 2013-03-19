/*
 * BeanstalkClient.cpp
 *
 *  Created on: Mar 13, 2013
 *      Author: chenhl
 */

#include <cetty/beanstalk/BeanstalkClient.h>
#include <cetty/logging/LoggerHelper.h>
#include <cetty/service/ClientService.h>

namespace cetty {
namespace beanstalk {

void dummyCallback(const BeanstalkServiceFuture& future,
                   const BeanstalkReplyPtr& reply) {
	LOG_DEBUG << "beanstalk quit command";
}

void BeanstalkClient::request(const BeanstalkCommandPtr& command,
    		                  const BeanstalkServiceFuturePtr& future) {
	::cetty::service::callMethod(channel, command, future);
}

void BeanstalkClient::put(const std::string& data,
    		              int priority,
    		              int delay,
    		              int ttr,
    		              const IdCallBack &callback) {
	BeanstalkServiceFuturePtr future(
	        new BeanstalkServiceFuture(boost::bind(&BeanstalkClient::idCallBack,
	        		                               _1, _2, callback))
	);

	request(protocol::commands::put(data, priority, delay, ttr), future);
}

void BeanstalkClient::use(const std::string &tubeName,
    		              const DataCallBack &callback) {
	BeanstalkServiceFuturePtr future(
		new BeanstalkServiceFuture(boost::bind(&BeanstalkClient::dataCallBack,
		        		                       _1, _2, callback))
    );

    request(protocol::commands::use(tubeName), future);
}

void BeanstalkClient::reserve(const IdDataCallBack &callback) {
	BeanstalkServiceFuturePtr future(
		new BeanstalkServiceFuture(boost::bind(&BeanstalkClient::idDataCallBack,
			        		                   _1, _2, callback))
	);

	request(protocol::commands::reserve(), future);
}

void BeanstalkClient::reserve(int timeout, const IdDataCallBack &callback) {
	BeanstalkServiceFuturePtr future(
	    new BeanstalkServiceFuture(boost::bind(&BeanstalkClient::idDataCallBack,
			        		                   _1, _2, callback))
    );

	request(protocol::commands::reserve(timeout), future);
}

void BeanstalkClient::del(int id, const CommandCallBack &callback) {
	BeanstalkServiceFuturePtr future(
		new BeanstalkServiceFuture(boost::bind(&BeanstalkClient::commandCallBack,
			        		                   _1, _2, callback))
	);

	request(protocol::commands::del(id), future);

}

void BeanstalkClient::release(int id,
    		                  int priority,
    		                  int delay,
    		                  const CommandCallBack &callback) {
	BeanstalkServiceFuturePtr future(
		new BeanstalkServiceFuture(boost::bind(&BeanstalkClient::commandCallBack,
			        		                   _1, _2, callback))
	);

	request(protocol::commands::release(id, priority, delay), future);
}

void BeanstalkClient::bury(int id,
		                   int priority,
		                   const CommandCallBack &callback) {
	BeanstalkServiceFuturePtr future(
		new BeanstalkServiceFuture(boost::bind(&BeanstalkClient::commandCallBack,
			        		                   _1, _2, callback))
	);

	request(protocol::commands::bury(id, priority), future);
}

void BeanstalkClient::touch(int id, const CommandCallBack &callback) {
	BeanstalkServiceFuturePtr future(
	    new BeanstalkServiceFuture(boost::bind(&BeanstalkClient::commandCallBack,
			        		                   _1, _2, callback))
    );

	request(protocol::commands::touch(id), future);
}

void BeanstalkClient::watch(const std::string &tube,
		                    const IdCallBack &callback) {
	BeanstalkServiceFuturePtr future(
		new BeanstalkServiceFuture(boost::bind(&BeanstalkClient::countCallBack,
				        		               _1, _2, callback))
	);

    request(protocol::commands::watch(tube), future);
}

void BeanstalkClient::ignore(const std::string &tube,
		                     const IdCallBack &callback) {
	BeanstalkServiceFuturePtr future(
		new BeanstalkServiceFuture(boost::bind(&BeanstalkClient::countCallBack,
				        		               _1, _2, callback))
	);

    request(protocol::commands::ignore(tube), future);
}

void BeanstalkClient::peek(int id, const IdDataCallBack &callback) {
	BeanstalkServiceFuturePtr future(
	    new BeanstalkServiceFuture(boost::bind(&BeanstalkClient::idDataCallBack,
					        		           _1, _2, callback))
    );

	request(protocol::commands::peek(id), future);
}

void BeanstalkClient::peekReady(const IdDataCallBack &callback) {
	BeanstalkServiceFuturePtr future(
		new BeanstalkServiceFuture(boost::bind(&BeanstalkClient::idDataCallBack,
						        		       _1, _2, callback))
    );

    request(protocol::commands::peekReady(), future);
}

void BeanstalkClient::peekDelayed(const IdDataCallBack &callback) {
	BeanstalkServiceFuturePtr future(
		new BeanstalkServiceFuture(boost::bind(&BeanstalkClient::idDataCallBack,
						        		       _1, _2, callback))
	);

    request(protocol::commands::peekDelayed(), future);
}

void BeanstalkClient::peekBuried(const IdDataCallBack &callback) {
	BeanstalkServiceFuturePtr future(
		new BeanstalkServiceFuture(boost::bind(&BeanstalkClient::idDataCallBack,
						        		       _1, _2, callback))
	);

    request(protocol::commands::peekBuried(), future);
}

void BeanstalkClient::kick(int bound, const IdCallBack &callback) {
	BeanstalkServiceFuturePtr future(
	    new BeanstalkServiceFuture(boost::bind(&BeanstalkClient::countCallBack,
							        		   _1, _2, callback))
    );

	request(protocol::commands::kick(bound), future);
}

void BeanstalkClient::kickJob(int id, const CommandCallBack &callback) {
	BeanstalkServiceFuturePtr future(
		new BeanstalkServiceFuture(boost::bind(&BeanstalkClient::commandCallBack,
								        	   _1, _2, callback))
	);

	request(protocol::commands::kickJob(id), future);
}

void BeanstalkClient::statsJob(int id, const DataCallBack &callback) {
	BeanstalkServiceFuturePtr future(
		new BeanstalkServiceFuture(boost::bind(&BeanstalkClient::dataCallBack,
								        	   _1, _2, callback))
	);

    request(protocol::commands::statsJob(id), future);
}

void BeanstalkClient::statsTube(const std::string &tube,
		                        const DataCallBack &callback) {
	BeanstalkServiceFuturePtr future(
		new BeanstalkServiceFuture(boost::bind(&BeanstalkClient::dataCallBack,
									           _1, _2, callback))
	);

	request(protocol::commands::statsTube(tube), future);
}

void BeanstalkClient::stats(const DataCallBack &callback) {
	BeanstalkServiceFuturePtr future(
		new BeanstalkServiceFuture(boost::bind(&BeanstalkClient::dataCallBack,
									           _1, _2, callback))
    );

	request(protocol::commands::stats(), future);
}

void BeanstalkClient::listTubes(const DataCallBack &callback) {
	BeanstalkServiceFuturePtr future(
		new BeanstalkServiceFuture(boost::bind(&BeanstalkClient::dataCallBack,
									           _1, _2, callback))
	);

	request(protocol::commands::listTubes(), future);
}

void BeanstalkClient::listTubeUsed(const DataCallBack &callback) {
	BeanstalkServiceFuturePtr future(
		new BeanstalkServiceFuture(boost::bind(&BeanstalkClient::dataCallBack,
									           _1, _2, callback))
	);

	request(protocol::commands::listTubeUsed(), future);
}

void BeanstalkClient::listTubesWatched(const DataCallBack &callback) {
	BeanstalkServiceFuturePtr future(
		new BeanstalkServiceFuture(boost::bind(&BeanstalkClient::dataCallBack,
										       _1, _2, callback))
	);

	request(protocol::commands::listTubesWatched(), future);
}

void BeanstalkClient::pauseTube(const std::string &tube,
    		                    int delay,
    		                    const CommandCallBack& callback) {
	BeanstalkServiceFuturePtr future(
		new BeanstalkServiceFuture(boost::bind(&BeanstalkClient::commandCallBack,
											   _1, _2, callback))
    );

    request(protocol::commands::pauseTube(tube, delay), future);
}

void BeanstalkClient::quit() {
	BeanstalkServiceFuturePtr future(
		new BeanstalkServiceFuture(boost::bind(&dummyCallback, _1, _2))
	);

	request(protocol::commands::quit(), future);
}

void BeanstalkClient::commandCallBack(const BeanstalkServiceFuture& future,
                                      const BeanstalkReplyPtr& reply,
                                      const CommandCallBack& callback) {
	if(!reply) {
		callback(std::string());
	}
	else {
		callback(reply->getResponse());
	}
}

void BeanstalkClient::idCallBack(const BeanstalkServiceFuture& future,
                                 const BeanstalkReplyPtr& reply,
                                 const IdCallBack& callback) {
	if (!reply) {
		callback(std::string(), -1);
	}
	else {
	    callback(reply->getResponse(), reply->getId());
	}
}

void BeanstalkClient::dataCallBack(const BeanstalkServiceFuture& future,
                                   const BeanstalkReplyPtr& reply,
                                   const DataCallBack& callback) {
    if (!reply) {
    	callback(std::string(), std::string());
    }
    else {
    	callback(reply->getResponse(), reply->getValue());
    }
}

void BeanstalkClient::countCallBack(const BeanstalkServiceFuture& future,
                                    const BeanstalkReplyPtr& reply,
                                    const IdCallBack& callback) {
	if (!reply) {
		callback(std::string(), -1);
	}
	else {
	    callback(reply->getResponse(), reply->getCount());
	}
}


void BeanstalkClient::idDataCallBack(const BeanstalkServiceFuture& future,
                                     const BeanstalkReplyPtr& reply,
                                     const IdDataCallBack& callback) {
	if (!reply) {
		callback(std::string(), -1, std::string());
	}
	else {
		callback(reply->getResponse(), reply->getId(), reply->getValue());
	}
}


}
}
