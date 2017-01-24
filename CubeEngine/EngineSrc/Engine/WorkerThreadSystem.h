#ifndef WORKER_THREAD_SYSTEM_H
#define WORKER_THREAD_SYSTEM_H
#include "EngineDef.h"
#include <functional>
#include <vector>
#include <list>
#include <thread>
#include <mutex>
namespace tzw{
	typedef std::function<void ()> WorkerJob;
	class WorkerThreadSystem
	{
		TZW_SINGLETON_DECL(WorkerThreadSystem)
	public:
		WorkerThreadSystem();
		void pushOrder(WorkerJob order);
		void init();
		void workderUpdate();

	private:
		std::list<WorkerJob> m_functionList;
		std::thread * m_thread;
		std::mutex m_rwMutex;
	};
}

#endif
