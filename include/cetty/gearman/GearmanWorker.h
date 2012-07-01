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
		//���ӳ�ʼ��
		void addServer(const std::string host,int port);
		void removeServer(const std::string host,int port);
		//��ǰ��addServer�����ݽ�������
		bool  workerRun();
		
		//��jobserverע�ᣬͬʱд��workFuncMap��
		void registerFunc(const std::string& funcName);
		//cant_do one func
		void unregisterFunc(const std::string& funcName);
		//reset all functions to be disable
		void unregisterAllFunc(const std::string& funcName);
		//for trace to identify the unique worker
		void setClientId();
		//���غ���ָ��
		void* getFunc(const std::string& funcName);
		//�ڽ��յ�job��
		static void handleJob(const std::string& funcName,const std::string& args);

	private:
		//��Щ���ڲ�ʹ�õģ���handleJob����
		void sendGrabJob();
		void sendGrabJobUniq();
		void sendPreSleep();

		//bg ��ʱ���ͻ����ֶ����ͣ�
		void sendWorkStatus();
		//������ڴ�job��ʱ�����Ҫ�õ�
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
