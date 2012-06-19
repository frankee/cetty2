#if !defined(CETTY_GEARMAN_GEARMANCLIENT_H)
#define CETTY_GEARMAN_GEARMANCLIENT_H

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
#include <iostream>
#include <string>
#include <map>


//trace for job
struct job_status
{
	const std::string job_handle,
	int isknown;//0 false  1 true
	int isrunning;//0 false  1 true
	int numerator;
	int denominator;
}

//client job info
struct gearman_task_info
{
	const std::string job_handle;
	const std::string uniqueId;
	job_status jobstatus;
	const std::string functionName;
	//optionName---optionValue
	std::map<std::string,std::string> options;
}

namespace cetty { namespace gearman { 
	class GearmanClient
	{
		public:
			GearmanClient(){}
			~GearmanClient(){}
			//初始化
			void addServer(const std::string host,int port);
			void removeServer(const std::string host,int port);
			//与jobserver建立连接
			void clientRun();
			//根据task类型进行处理
			void addTask(int type,std::string funcName);

			void submitJob(GearmanMessagePtr& messag);
			void submitJobBG();
			void submitJobHigh();
			void submitJobHighBG();
			void submitJobLow();
			void submitJobLowBG();

			void getStatus();
			void getOption();
			void errorHandle();
		private:
			//std::vector<std::string> regisFuncs;
			std::vector<servAddr> servAddrs;
			ChannelPtr channel;
	};
}}

#endif //#if !defined(CETTY_GEARMAN_GEARMANCLIENT_H)

// Local Variables:
// mode: c++
// End:
