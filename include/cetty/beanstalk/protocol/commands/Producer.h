#if !defined(CETTY_BEANSTALK_PROTOCOL_COMMANDS_PRODUCER_H)
#define CETTY_BEANSTALK_PROTOCOL_COMMANDS_PRODUCER_H

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

#include <cetty/beanstalk/protocol/BeanstalkCommand.h>

namespace cetty {
namespace beanstalk {
namespace protocol {
namespace commands {

static const int DEFAULT_PRIORITY = 1024;
static const int DEFAULT_TTR = 60;

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

#endif //#if !defined(CETTY_BEANSTALK_PROTOCOL_COMMANDS_PRODUCER_H)

// Local Variables:
// mode: c++
// End:
