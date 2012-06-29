#include <cetty/bootstrap/ClientBootstrap.h>
#include <cetty/channel/socket/asio/AsioClientSocketChannelFactory.h>
#include <cetty/channel/socket/asio/AsioServicePool.h>
#include <cetty/channel/Channel.h>
#include <cetty/channel/ChannelMessage.h>
#include <cetty/channel/ChannelFuture.h>
#include <cetty/buffer/ChannelBuffers.h>

#include <cetty/gearman/GearmanPipelineFactory.h>
#include <cetty/gearman/GearmanTask.h>
#include <vector>
#include <cstdio>

using namespace cetty::bootstrap;
using namespace cetty::buffer;
using namespace cetty::channel::socket::asio;
using namespace cetty::channel;
using namespace cetty::gearman;
using namespace cetty::util;

void testForCando(ChannelPtr&  c)
{
	const std::string functionName = "hello";
	GearmanTaskPtr task(new GearmanTask);
	task->request = GearmanMessage::createCandoMessage(functionName);
	c->write(ChannelMessage(task));
}

void testForCandoTimeout(ChannelPtr&  c)
{
	const std::string functionName = "hello";
	int timeout;
	GearmanTaskPtr task(new GearmanTask);
	task->request = GearmanMessage::createCandoTimeoutMessage(functionName,timeout);
	c->write(ChannelMessage(task));
}

void testForCantdo(ChannelPtr&  c)
{
	const std::string functionName = "hello";
	GearmanTaskPtr task(new GearmanTask);
	task->request = GearmanMessage::createCantdoMessage(functionName);
	c->write(ChannelMessage(task));
}


void testForResetAbilities(ChannelPtr&  c)
{
	GearmanTaskPtr task(new GearmanTask);
	task->request = GearmanMessage::createResetAbilitiesMessage();
	c->write(ChannelMessage(task));
}


void testForPreSleep(ChannelPtr&  c)
{
	GearmanTaskPtr task(new GearmanTask);
	task->request = GearmanMessage::createPreSleepMessage();
	c->write(ChannelMessage(task));
}


void testForGrabJob(ChannelPtr&  c)
{
	GearmanTaskPtr task(new GearmanTask);
	task->request = GearmanMessage::createGrabJobMessage();
	c->write(ChannelMessage(task));
}


void testForGrabJobUniq(ChannelPtr&  c)
{
	GearmanTaskPtr task(new GearmanTask);
	task->request = GearmanMessage::createGrabJobUniqMessage();
	c->write(ChannelMessage(task));
}



void testForWorkStauts(ChannelPtr&  c)
{
	const std::string jobHandle = "hd:112";
	int numerator = 20;
	int denominator = 100;
	GearmanTaskPtr task(new GearmanTask);
	task->request = GearmanMessage::createWorkStautsMessage(jobHandle,numerator,denominator);
	c->write(ChannelMessage(task));
}

void testForWorkComplete(ChannelPtr&  c)
{
	const std::string jobHandle = "hd:112";
	ChannelBufferPtr payload = ChannelBuffers::buffer(1024,60);
	const std::string  msg = "complete";
	payload->writeBytes(msg);
	GearmanTaskPtr task(new GearmanTask);
	task->request = GearmanMessage::createWorkCompleteMessage(jobHandle,payload);
	c->write(ChannelMessage(task));
}


void testForWorkFail(ChannelPtr&  c)
{
	const std::string jobHandle = "hd:112";
	GearmanTaskPtr task(new GearmanTask);
	task->request = GearmanMessage::createWorkFailMessage(jobHandle);
	c->write(ChannelMessage(task));
}


void testForWorkWarning(ChannelPtr&  c)
{
	const std::string jobHandle = "hd:112";
	ChannelBufferPtr payload = ChannelBuffers::buffer(1024,60);
	const std::string  msg = "warning";
	payload->writeBytes(msg);
	GearmanTaskPtr task(new GearmanTask);
	task->request = GearmanMessage::createWorkWarningMessage(jobHandle,payload);
	c->write(ChannelMessage(task));
}


void testForWorkException(ChannelPtr&  c)
{
	const std::string jobHandle = "hd:112";
	ChannelBufferPtr payload = ChannelBuffers::buffer(1024,60);
	const std::string  msg = "exception";
	payload->writeBytes(msg);
	GearmanTaskPtr task(new GearmanTask);
	task->request = GearmanMessage::createWorkExceptionMessage(jobHandle,payload);
	c->write(ChannelMessage(task));
}


void testForWorkData(ChannelPtr&  c)
{
	const std::string jobHandle = "H:localhost.localdomain:8";
	ChannelBufferPtr payload = ChannelBuffers::buffer(1024,60);
	const std::string  msg = "work_data";
	payload->writeBytes(msg);
	GearmanTaskPtr task(new GearmanTask);
	task->request = GearmanMessage::createWorkDataMessage(jobHandle,payload);
	c->write(ChannelMessage(task));
}


void testForSetClientId(ChannelPtr&  c)
{
	const std::string clientId = "123456";

	GearmanTaskPtr task(new GearmanTask);
	task->request = GearmanMessage::createSetClientIdMessage(clientId);
	c->write(ChannelMessage(task));
}

//not implement yet
void testForAllYoursMessage(ChannelPtr&  c)
{
	GearmanTaskPtr task(new GearmanTask);
	task->request = GearmanMessage::createAllYoursMessage();
	c->write(ChannelMessage(task));
}

int main()
{
	std::string host = "192.168.1.112";
	int port = 4730;
	int ioThreadCount = 1;
	ClientBootstrap  bootstrap(new AsioClientSocketChannelFactory(ioThreadCount));
	bootstrap.setPipelineFactory(new GearmanPipelineFactory);

	std::vector<Channel*>clientChannels;

	ChannelFuturePtr future = bootstrap.connect(host,port);
	future->awaitUninterruptibly();

	ChannelPtr c = future->getChannel();
	
	testForCando(c);
	//testForPreSleep(c);
	Sleep(5000);
	//testForGrabJobUniq(c);
	testForGrabJob(c);
	Sleep(10000);
	testForWorkData(c);
	//testForLog();

	//boost::thread::sleep(5);
	
	//testForGrabJob(c);

	
	system("PAUSE");
	c->getCloseFuture()->awaitUninterruptibly();

	// Shut down thread pools to exit.
	bootstrap.releaseExternalResources();

	return 0;
}
