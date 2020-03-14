#ifndef WORKER_THREAD_SYSTEM_H
#define WORKER_THREAD_SYSTEM_H
#include "EngineDef.h"
#include <functional>
#include <vector>
#include <list>
#include <thread>
#include <mutex>
namespace tzw{
	typedef std::function<void ()> VoidJob;
	class WorkerJob
	{
	public:
		WorkerJob(VoidJob work, VoidJob finish);
		WorkerJob(VoidJob work);
		WorkerJob();
		VoidJob m_work;
		VoidJob m_onFinished;
	};
	//typedef std::function<void ()> WorkerJob;
	class WorkerThreadSystem
	{
		TZW_SINGLETON_DECL(WorkerThreadSystem)
	public:
		WorkerThreadSystem();
		void pushOrder(WorkerJob order);
		void pushMainThreadOrder(WorkerJob order);
		void pushMainThreadOrderWithLoading(std::string tipsInfo, WorkerJob order);
		void init();
		void workderUpdate();
		void mainThreadUpdate();
	private:
		std::list<WorkerJob> m_functionList1;
		std::list<WorkerJob> m_functionList2;
		std::list<WorkerJob> m_mainThreadFunctionList;
		std::list<WorkerJob> m_mainThreadCB1;
		std::list<WorkerJob> m_mainThreadCB2;
		std::thread * m_thread;
		std::mutex m_rwMutex;
		int m_readyToDeathCount;
	};
}

#endif
