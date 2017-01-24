#include "WorkerThreadSystem.h"
#include <thread>
#include <mutex>
namespace tzw
{ 
	TZW_SINGLETON_IMPL(WorkerThreadSystem);

	WorkerThreadSystem::WorkerThreadSystem()
	{
		m_thread = nullptr;
	}



	void WorkerThreadSystem::pushOrder(WorkerJob order)
	{
		m_rwMutex.lock();
		m_functionList.push_back(order);
		m_rwMutex.unlock();
	}

	void WorkerThreadSystem::init()
	{
		m_thread = new std::thread([&]() {workderUpdate();});
		m_thread->detach();
	}

	void WorkerThreadSystem::workderUpdate()
	{
		for(;;)
		{
			WorkerJob job = nullptr;
			m_rwMutex.lock();
			if(!m_functionList.empty())
			{
				job = m_functionList.front();
				m_functionList.pop_front();
			}
			m_rwMutex.unlock();
			if(job)
			{
				job();
			}
		}
	}


}