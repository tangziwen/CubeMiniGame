#ifndef WORKER_THREAD_SYSTEM_H
#define WORKER_THREAD_SYSTEM_H
#include "EngineDef.h"
#include <functional>
#include <vector>
#include <list>
#include <thread>
#include <mutex>
#include <condition_variable>
namespace tzw{


	class RWMutex 
	{  
	private:  
		std::mutex wm, rmm;  
		std::unique_lock<std::mutex> rmtx;  
		std::condition_variable cond;  
		int rd_cnt; //wait for read  
		bool wr_cnt;    //wait for write  
	public:  
		RWMutex();  
		void rLock();  
		void rUnLock();  
		void wLock();  
		void wUnlock();  
	}; 

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
		RWMutex m_mutex;
	};
}

#endif
