#include "WorkerThreadSystem.h"

namespace tzw
{ 
	RWMutex::RWMutex() : rmtx(rmm, std::defer_lock)
	{
		rd_cnt = 0;
		wr_cnt = false;
	}

	void RWMutex::rLock()
	{
		while (wr_cnt)  
		{
			cond.wait(rmtx);
		}
	}

	void RWMutex::rUnLock()
	{

		if (!wr_cnt)  
			cond.notify_all();
	}

	void RWMutex::wLock()
	{

		wm.lock();  
		while (wr_cnt)  
			cond.wait(rmtx);  
		wr_cnt = true;  
		wm.unlock();
	}

	void RWMutex::wUnlock()
	{

		wm.lock();  
		wr_cnt = false;  
		if (!wr_cnt)  
			cond.notify_all();  
		wm.unlock();
	}

	TZW_SINGLETON_IMPL(WorkerThreadSystem);

	WorkerThreadSystem::WorkerThreadSystem()
	{
		m_thread = nullptr;
	}



	void WorkerThreadSystem::pushOrder(WorkerJob order)
	{
		m_mutex.wLock();
		m_functionList.push_back(order);
		m_mutex.wUnlock();
	}

	void WorkerThreadSystem::init()
	{
		m_thread = new std::thread([&]() {workderUpdate();});
		m_thread->detach();
	}

	void WorkerThreadSystem::workderUpdate()
	{
		m_mutex.rLock();
		for(;;)
		{
			WorkerJob job = nullptr;
			if(!m_functionList.empty())
			{
				job = m_functionList.front();
				m_functionList.pop_front();
			}
			if(job)
			{
				job();
			}
		}
		m_mutex.rUnLock();
	}


}