/*
 * Producer.h
 *
 *  Created on: Mar 11, 2013
 *      Author: chenhl
 */

#ifndef PRODUCER_H_
#define PRODUCER_H_

#include <cetty/beanstalk/protocol/BeanstalkCommand.h>

namespace cetty {
namespace beanstalk {
namespace protocol {
namespace command {

#define DEFAULT_PRIORITY 1024
#define DEFAULT_TTR 60

/**
* Adds a job consisting of a string to the server
*
* @param data The data to send
*
* @return The id the job got on the server
*
* @throws ServerException With reason BAD_FORMAT on unexpected server response
* @throws ServerException With reason JOB_TOO_BIG on jobs the server deems too big
*/
BeanstalkCommandPtr put(const std::string &data,
		                int pri = DEFAULT_PRIORITY,
		                int delay = 0,
		                int ttr = DEFAULT_TTR);

/**
* Selects the tube to send jobs through. If no tube has been selected,
* the tube "default" is used.
*
* @param tubeName The name of the tube
*
* @throws ServerException With reason BAD_FORMAT on unexpected server response
*/
BeanstalkCommandPtr use(const std::string &tubeName);

}
}
}
}



#endif /* PRODUCER_H_ */
