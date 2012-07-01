#if !defined(CETTY_GEARMAN_GEARMANWORKER_H)
#define CETTY_GEARMAN_GEARMANWORKER_H

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
#include <vector>

struct gearman_job_info
{
	std::map<std::string functionName,fn functionPtr> options;
}

struct  gearman_worker_info
{
	std::vector<gearman_job_info> job_list;
	vector<connect> conns;
}

struct servAddr
{
	const std::string host;
	int port;
};

namespace cetty { namespace gearman { 
	class GearmanWorker
	{
	public:
		GearmanWorker(){}
		~GearmanWorker(){}
		//连接初始化
		void addServer(const std::string host,int port);
		void removeServer(const std::string host,int port);
		//对前面addServer的数据建立连接
		bool  workerRun();
		
		//向jobserver注册，同时写入workFuncMap中
		void registerFunc(const std::string& funcName);
		//cant_do one func
		void unregisterFunc(const std::string& funcName);
		//reset all functions to be disable
		void unregisterAllFunc(const std::string& funcName);
		//for trace to identify the unique worker
		void setClientId();
		//返回函数指针
		void* getFunc(const std::string& funcName);
		//在接收到job后
		static void handleJob(const std::string& funcName,const std::string& args);

	private:
		//这些是内部使用的，被handleJob调用
		void sendGrabJob();
		void sendGrabJobUniq();
		void sendPreSleep();

		//bg 定时发送或者手动发送，
		void sendWorkStatus();
		//这个是在大job的时候才需要用到
		void sendWorkData();
		void sendWorkComplete();
		void sendWorkWarning();
		void sendWorkFail();
		void sendWorkException();
	private:
		//std::vector<std::string> regisFuncs;
		std::vector<servAddr> servAddrs;
		//to maintain funcs
		std::map<std::string,worker_fn>workFuncMap;
		
	};
}}

#endif //#if !defined(CETTY_GEARMAN_GEARMANWORKER_H)

// Local Variables:
// mode: c++
// End:
