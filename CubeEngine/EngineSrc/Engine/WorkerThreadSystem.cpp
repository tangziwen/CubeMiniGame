#include "WorkerThreadSystem.h"
#include <thread>
#include <mutex>
namespace tzw
{ 
	TZW_SINGLETON_IMPL(WorkerThreadSystem);

	WorkerThreadSystem::WorkerThreadSystem()
	{
		m_thread = nullptr;
		m_readyToDeathCount = 0;
	}



	void WorkerThreadSystem::pushOrder(WorkerJob order)
	{

		m_rwMutex.lock();
		m_functionList.push_back(order);
		m_rwMutex.unlock();
		if(!m_thread)
		{
			init();
		}
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
			if(m_functionList.empty())
			{
				m_readyToDeathCount ++;
				if(m_readyToDeathCount > 100)
				{
					m_thread = nullptr;
					break;
				}
			}
		}
	}
}