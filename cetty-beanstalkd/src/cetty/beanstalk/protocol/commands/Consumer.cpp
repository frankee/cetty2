/*
 * Consumer.cpp
 *
 *  Created on: Mar 15, 2013
 *      Author: chenhl
 */

#include <cetty/beanstalk/protocol/commands/Consumer.h>

namespace cetty {
namespace beanstalk {
namespace protocol {
namespace command {

BeanstalkCommandPtr reserve() {
	BeanstalkCommandPtr command = new BeanstalkCommand("reserve");
	*command << "reserve\r\n";

	return command;
}

BeanstalkCommandPtr reserve(int timeout) {
	BeanstalkCommandPtr command = new BeanstalkCommand("reserve-with-timeout");
	*command << "reserve-with-timeout " << timeout << "\r\n";

	return command;

}



}
}
}
}

