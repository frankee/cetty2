/*
 * beanstalk.cpp
 *
 *  Created on: Mar 18, 2013
 *      Author: chenhl
 */


#include <cetty/beanstalk/BeanstalkClientBuilder.h>

using namespace cetty::beanstalk;

void idCallBack(std::string response, int id) {
	std::cout << response << " " << id << std::endl;
}

int main() {
	BeanstalkClientBuilder builder;
	builder.addConnection("127.0.0.1", 11300);

	BeanstalkClient client(builder.build());

    client.put("hello", 1024, 0, 10, &idCallBack);



	return 0;
}
