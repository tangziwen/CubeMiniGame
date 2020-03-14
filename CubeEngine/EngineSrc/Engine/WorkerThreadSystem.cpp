#include "WorkerThreadSystem.h"
#include <thread>
#include <mutex>
#include "CubeGame/LoadingUI.h"

namespace tzw
{
	WorkerJob::WorkerJob(VoidJob work, VoidJob finish):m_work(work),m_onFinished(finish)
	{
		
	}

	WorkerJob::WorkerJob(VoidJob work):m_work(work),m_onFinished(nullptr)
	{

	}

	WorkerJob::WorkerJob():m_onFinished(nullptr),m_work(nullptr)
	{
	
	}

	TZW_SINGLETON_IMPL(WorkerThreadSystem);

	WorkerThreadSystem::WorkerThreadSystem()
	{
		m_thread = nullptr;
		m_readyToDeathCount = 0;
	}



	void WorkerThreadSystem::pushOrder(WorkerJob order)
	{
		m_rwMutex.lock();
		m_functionList1.push_back(order);
		m_rwMutex.unlock();
		if(!m_thread)
		{
			init();
		}
	}

	void WorkerThreadSystem::pushMainThreadOrder(WorkerJob order)
	{
		//order();
		m_mainThreadFunctionList.push_back(order);
	}

	void WorkerThreadSystem::pushMainThreadOrderWithLoading(std::string tipsInfo, WorkerJob order)
	{
        pushMainThreadOrder(WorkerJob([tipsInfo] {LoadingUI::shared()->setTipsInfo(tipsInfo);}));
		pushMainThreadOrder(order);
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
			m_rwMutex.lock();
			std::swap(m_functionList1, m_functionList2);
			m_rwMutex.unlock();
			while(!m_functionList2.empty())
			{
				auto job = m_functionList2.front();
				m_functionList2.pop_front();
				if(job.m_work)
				{
					job.m_work();
					if(job.m_onFinished)
					{
						m_rwMutex.lock();
						m_mainThreadCB1.push_back(job);
						m_rwMutex.unlock();
					}
				}
			}
		}
	}

	void WorkerThreadSystem::mainThreadUpdate()
	{
		m_rwMutex.lock();
		std::swap(m_mainThreadCB1, m_mainThreadCB2);
		m_rwMutex.unlock();
		if(!m_mainThreadCB2.empty())
		{
			for(auto cb : m_mainThreadCB2)
			{
				if(cb.m_onFinished)
				{
					cb.m_onFinished();
				}
			}
			m_mainThreadCB2.clear();
		}
		if(!m_mainThreadFunctionList.empty())
		{
			auto job = m_mainThreadFunctionList.front();
			m_mainThreadFunctionList.pop_front();
			job.m_work();
		}
	}
}
