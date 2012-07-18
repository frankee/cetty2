#ifndef __WORKER_FUNC_H__
#define __WORKER_FUNC_H__
#include <string>

namespace cetty {namespace gearman{
	class WorkerFunction
	{
		public:
			WorkerFunction(){}
			~WorkerFunction(){}

			static void handle1(std::string& args);
			static void handle2(std::string& args);
			static void handle3(std::string& args);
			static void handle4(std::string& args);
			static void handle5(std::string& args);
	};
}}

#endif
