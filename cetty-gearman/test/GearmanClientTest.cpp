// echo.cpp : Defines the entry point for the console application.
//

#include <cetty/bootstrap/ClientBootstrap.h>
#include <cetty/channel/asio/AsioClientSocketChannelFactory.h>
#include <cetty/channel/asio/AsioServicePool.h>
#include <cetty/channel/Channel.h>
#include <cetty/channel/ChannelFuture.h>
#include <cetty/buffer/Unpooled.h>

#include <vector>

using namespace cetty::channel;
using namespace cetty::buffer;
using namespace cetty::channel::socket::asio;

using namespace cetty::bootstrap;

using namespace cetty::util;
using namespace cetty::gearman;

void test4Echo(ChannelPtr& c)
{
	GearmanTaskPtr task(new GearmanTask);
	ChannelBufferPtr buffer = Unpooled::buffer(1024, 64);
	for (int i = 0; i < 20; ++i) {
		buffer->writeByte('0');
	}
	
	task->request = GearmanMessage::createEchoReqMessage(buffer);
	//writedown   往gearman写数据
	c->write(UserEvent(task));
}

void test4submitJob(ChannelPtr& c)
{
	const std::string functionName = "hello";
	const std::string uniqueId = "123456";
	GearmanTaskPtr task(new GearmanTask);
	ChannelBufferPtr payload = Unpooled::buffer(1024, 64);
	for (int i = 0; i < 10; ++i) {
		payload->writeByte('0');
	}
	task->request = GearmanMessage::createsubmitJobMessage(functionName,uniqueId,payload);
	//writedown   往gearman写数据
	c->write(UserEvent(task));
}

void test4submitJobHigh(ChannelPtr& c)
{
	const std::string functionName = "hello";
	const std::string uniqueId = "123456";
	GearmanTaskPtr task(new GearmanTask);
	ChannelBufferPtr payload = Unpooled::buffer(1024, 64);
	for (int i = 0; i < 10; ++i) {
		payload->writeByte('0');
	}

	task->request = GearmanMessage::createsubmitJobHighMessage(functionName,uniqueId,payload);
	//writedown   往gearman写数据
	c->write(UserEvent(task));
}

void test4submitJobLow(ChannelPtr& c)
{
	const std::string functionName = "hello";
	const std::string uniqueId = "123456";
	GearmanTaskPtr task(new GearmanTask);
	ChannelBufferPtr payload = Unpooled::buffer(1024, 64);
	for (int i = 0; i < 10; ++i) {
		payload->writeByte('0');
	}
	task->request = GearmanMessage::createsubmitJobLowMessage(functionName,uniqueId,payload);
	//writedown   往gearman写数据
	c->write(UserEvent(task));
}


void test4submitJobBG(ChannelPtr& c)
{
	const std::string functionName = "hello";
	const std::string uniqueId = "123456";
	GearmanTaskPtr task(new GearmanTask);
	ChannelBufferPtr payload = Unpooled::buffer(1024, 64);
	for (int i = 0; i < 10; ++i) {
		payload->writeByte('0');
	}
	task->request = GearmanMessage::createsubmitJobBGMessage(functionName,uniqueId,payload);
	//writedown   往gearman写数据
	c->write(UserEvent(task));
}


void test4submitJobHighBG(ChannelPtr& c)
{
	const std::string functionName = "hello";
	const std::string uniqueId = "123456";
	GearmanTaskPtr task(new GearmanTask);
	ChannelBufferPtr payload = Unpooled::buffer(1024, 64);
	for (int i = 0; i < 10; ++i) {
		payload->writeByte('0');
	}
	task->request = GearmanMessage::createsubmitJobHighBGMessage(functionName,uniqueId,payload);
	//writedown   往gearman写数据
	c->write(UserEvent(task));
}


void test4submitJobLowBG(ChannelPtr& c)
{
	const std::string functionName = "hello";
	const std::string uniqueId = "123456";
	GearmanTaskPtr task(new GearmanTask);
	ChannelBufferPtr payload = Unpooled::buffer(1024, 64);
	for (int i = 0; i < 10; ++i) {
		payload->writeByte('0');
	}
	task->request = GearmanMessage::createsubmitJobLowBGMessage(functionName,uniqueId,payload);
	//writedown   往gearman写数据
	c->write(UserEvent(task));
}

void test4GetStatus(ChannelPtr& c)
{
	const std::string jobHandle = "H:localhost.localdomain:4";
	GearmanTaskPtr task(new GearmanTask);
	task->request = GearmanMessage::createGetStatusMessage(jobHandle);
	//writedown   往gearman写数据
	c->write(UserEvent(task));
}

void test4OptionReq(ChannelPtr& c)
{
	const std::string option = "exception";
	GearmanTaskPtr task(new GearmanTask);
	ChannelBufferPtr payload = Unpooled::buffer(1024, 64);
	for (int i = 0; i < 10; ++i) {
		payload->writeByte('0');
	}
	task->request = GearmanMessage::createOptionReqMessage(option);
	//writedown   往gearman写数据
	c->write(UserEvent(task));
}

/*const std::vector<const std::string&> params;
	params.push_back()*/
int main(int argc, char* argv[]) {
    // Print usage if no argument is specified.

    // Parse options.
    std::string host = "192.168.1.112";
    int port = 4730;
    int ioThreadCount = 1;

    // Configure the client.
    ClientBootstrap bootstrap(new AsioClientSocketChannelFactory(ioThreadCount));

    bootstrap.setPipelineFactory(new GearmanPipelineFactory);

    // Start the connection attempt.
    std::vector<Channel*> clientChannels;

    ChannelFuturePtr future = bootstrap.connect(host, port);
    future->awaitUninterruptibly();

	ChannelPtr c = future->channel();

	//test4Echo(c);
	test4submitJob(c);
	test4submitJobBG(c);
	//Sleep(5000);
 	//test4GetStatus(c);
	//test4submitJobBG(c);

    system("PAUSE");
    c->closeFuture()->awaitUninterruptibly();

    // Shut down thread pools to exit.
    bootstrap.shutdown();

    return 0;
}
