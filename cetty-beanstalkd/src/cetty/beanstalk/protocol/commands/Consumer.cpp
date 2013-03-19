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
namespace commands {

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

BeanstalkCommandPtr del(int jobId) {
	BeanstalkCommandPtr command = new BeanstalkCommand("delete");
    *command << "delete " << jobId << "\r\n";

    return command;
}

BeanstalkCommandPtr release(int id,
		                    int priority =  DEFAULT_PRIORITY,
		                    int delay = 0) {
	BeanstalkCommandPtr command = new BeanstalkCommand("release");
    *command << "release " << id << " " << priority << " " << delay << "\r\n";

    return command;
}

BeanstalkCommandPtr bury(int jobId, int priority = 10) {
	BeanstalkCommandPtr command = new BeanstalkCommand("bury");
    *command << "bury " << jobId << " " << priority << "\r\n";

    return command;
}

BeanstalkCommandPtr kick(int bound) {
	BeanstalkCommandPtr command = new BeanstalkCommand("kick");
    *command << "kick " << bound << "\r\n";

    return command;
}

BeanstalkCommandPtr kickJob(int id) {
	BeanstalkCommandPtr command = new BeanstalkCommand("kick");
    *command << "kick-job " << id << "\r\n";

    return command;
}

BeanstalkCommandPtr watch(const std::string &tube) {
	BeanstalkCommandPtr command = new BeanstalkCommand("watch");
    *command << "watch " << tube << "\r\n";

    return command;
}

BeanstalkCommandPtr listTubes() {
	BeanstalkCommandPtr command = new BeanstalkCommand("list-tubes");
    *command << "list-tubes\r\n";

    return command;
}

BeanstalkCommandPtr touch(int id) {
	BeanstalkCommandPtr command = new BeanstalkCommand("touch");
    *command << "touch " << id << "\r\n";

    return command;
}

BeanstalkCommandPtr ignore(const std::string &tubeName) {
	BeanstalkCommandPtr command = new BeanstalkCommand("ignore");
    *command << "ignore " << tubeName << "\r\n";

    return command;
}

BeanstalkCommandPtr quit() {
	BeanstalkCommandPtr command = new BeanstalkCommand("quit");
    *command << "quit\r\n";

    return command;
}

BeanstalkCommandPtr statsJob(int id) {
	BeanstalkCommandPtr command = new BeanstalkCommand("stats-job");
    *command << "stats-job " << id << "\r\n";

    return command;
}

BeanstalkCommandPtr statsTube(const std::string &tube) {
	BeanstalkCommandPtr command = new BeanstalkCommand("stats-tube");
    *command << "stats-tube " << tube << "\r\n";

    return command;
}

BeanstalkCommandPtr stats() {
	BeanstalkCommandPtr command = new BeanstalkCommand("stats");
    *command << "stats\r\n";

    return command;
}

BeanstalkCommandPtr listTubeUsed() {
	BeanstalkCommandPtr command = new BeanstalkCommand("list-tube-used");
    *command << "list-tube-used\r\n";

    return command;
}

BeanstalkCommandPtr listTubesWatched() {
	BeanstalkCommandPtr command = new BeanstalkCommand("list-tubes-watched");
    *command << "list-tubes-watched\r\n";

    return command;
}

BeanstalkCommandPtr pauseTube(const std::string &tube, int delay) {
	BeanstalkCommandPtr command = new BeanstalkCommand("pause-tube");
    *command << "pause-tube " << tube << " " << delay << "\r\n";

    return command;
}

BeanstalkCommandPtr peekReady() {
	BeanstalkCommandPtr command = new BeanstalkCommand("peek-ready");
    *command << "peek-ready\r\n";

    return command;
}

BeanstalkCommandPtr peekDelayed() {
	BeanstalkCommandPtr command = new BeanstalkCommand("peek-delayed");
    *command << "peek-delayed\r\n";

    return command;
}

BeanstalkCommandPtr peekBuried() {
	BeanstalkCommandPtr command = new BeanstalkCommand("peek-buried");
	*command << "peek-buried\r\n";

	return command;
}

BeanstalkCommandPtr peek(int id) {
	BeanstalkCommandPtr command = new BeanstalkCommand("peek");
	*command << "peek " << id << "\r\n";

	return command;
}


}
}
}
}

