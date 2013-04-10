/*
 * beanstalk.cpp
 *
 *  Created on: Mar 18, 2013
 *      Author: chenhl
 */

#include <cetty/beanstalk/BeanstalkClient.h>
#include <cetty/beanstalk/builder/BeanstalkClientBuilder.h>

using namespace cetty::beanstalk;
using namespace cetty::beanstalk::builder;

int main() {
	BeanstalkClientBuilder builder;
	builder.addConnection("192.168.1.160", 11300);

	BeanstalkClient client(builder.build());

    client.put("hello", BeanstalkClient::ReplyCallback());

    system("pause");
	return 0;
}
