#if !defined(CETTY_BEANSTALK_PROTOCOL_COMMANDS_CONSUMER_H)
#define CETTY_BEANSTALK_PROTOCOL_COMMANDS_CONSUMER_H

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
#include <cetty/beanstalk/protocol/commands/Producer.h>

namespace cetty {
namespace beanstalk {
namespace protocol {
namespace commands {

/**
* Reserves the next job in the queue. This function is blocking until a job becomes available in
* the queue.
*
* TJob should be either @c Job, or a subclass of @c Job.
*
* @throws ServerException With reason BAD_FORMAT on unexpected server response
*/
BeanstalkCommandPtr reserve();

/**
* Tries to reserve a job from the queue. If a job is available within @p timeout, it will be
* placed in @p jobPtr, and true will be returned. Otherwise @p jobPtr will be left unchanged,
* and false will be returned.
*
* @param jobPtr  Return-value if a job was found. Should be @c Job or a subclass of @c Job
* @param timeout The timeout counted in seconds
*
* @return Wether a job was found and put into @p jobPtr
*
* @throws ServerException With reason BAD_FORMAT on unexpected server response
*/
BeanstalkCommandPtr reserve(int timeout);

/**
* Peeks at the next ready job in the queue. This will not reserve the job.
*
* @param jobPtr Return-value if a job was found. Should be @c Job or a subclass of @c Job
*
* @return True of a job was found, otherwise false.
*
* @throws ServerException With reason BAD_FORMAT on unexpected server response
*/
BeanstalkCommandPtr peekReady();
BeanstalkCommandPtr peekDelayed();
BeanstalkCommandPtr peekBuried();

BeanstalkCommandPtr peek(int id);

/**
* The delete command removes a job from the server entirely. It is normally used by the client
* when the job has successfully run to completion. A client can only delete jobs that it has
* reserved or jobs that are buried.
*
* @param j The job to delete
*
* @throws ServerException With reason BAD_FORMAT on unexpected server response
*/
BeanstalkCommandPtr del(int jobId);

BeanstalkCommandPtr release(int id,
		                    int priority = DEFAULT_PRIORITY,
		                    int delay = 0);

/**
* The bury command puts a job into the "buried" state. Buried jobs are put into a FIFO linked
* list and will not be touched by the server again until a client kicks them with the "kick"
* command.
*
* @param j        The job to bury
* @param priority The priority in the buried queue
*
* @throws ServerException With status NOT_FOUND if the job was not found on the server
* @throws ServerException With reason BAD_FORMAT on unexpected server response
*/
BeanstalkCommandPtr bury(int jobId, int priority = 10);

BeanstalkCommandPtr kick(int bound);
BeanstalkCommandPtr kickJob(int id);

/**
 * The "watch" command adds the named tube to the watch list for the current connection. A reserve
 * command will take a job from any of the tubes in the watch list. For each new connection, the
 * watch list initially consists of one tube, named "default".
 *
 * @param tube The new tube to watch
 *
 * @throws ServerException With reason BAD_FORMAT on unexpected server response
 *
 * @return The number of tubes currently watched
 */
BeanstalkCommandPtr watch(const std::string &tube);

/**
 * Returns a list of all tubes available at the beanstalk server
 *
 * @throws ServerException With reason BAD_FORMAT on unexpected server response
 */
BeanstalkCommandPtr listTubes();

BeanstalkCommandPtr touch(int id);
BeanstalkCommandPtr ignore(const std::string &tubeName);

BeanstalkCommandPtr quit();

BeanstalkCommandPtr statsJob(int id);
BeanstalkCommandPtr statsTube(const std::string &tube);
BeanstalkCommandPtr stats();

BeanstalkCommandPtr listTubeUsed();
BeanstalkCommandPtr listTubesWatched();

BeanstalkCommandPtr pauseTube(const std::string &tube, int delay);

}
}
}
}

#endif //#if !defined(CETTY_BEANSTALK_PROTOCOL_COMMANDS_CONSUMER_H)

// Local Variables:
// mode: c++
// End:
