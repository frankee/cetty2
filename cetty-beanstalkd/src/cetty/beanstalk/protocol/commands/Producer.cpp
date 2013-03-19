/*
 * Producer.cpp
 *
 *  Created on: Mar 15, 2013
 *      Author: chenhl
 */

#include <cetty/beanstalk/protocol/commands/Producer.h>

namespace cetty {
namespace beanstalk {
namespace protocol {
namespace command {

BeanstalkCommandPtr put(const std::string &data,
		                int pri = DEFAULT_PRIORITY,
		                int delay = 0,
		                int ttr = DEFAULT_TTR) {
	BeanstalkCommandPtr command(new BeanstalkCommand("put"));

    *command << "put " << pri << " " << delay
    		<< " " << ttr << " " << data.length()
    		<< "\r\n";
    *command << data << "\r\n";

    return command;
}

BeanstalkCommandPtr use(const std::string &tubeName) {
	BeanstalkCommandPtr command(new BeanstalkCommand("use"));
	*command << "use " << tubeName << "\r\n";

	return command;
}

}
}
}
}
