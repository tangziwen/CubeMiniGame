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


	WorkerThreadSystem::WorkerThreadSystem()
	{
		m_thread = nullptr;
	}



	void WorkerThreadSystem::pushOrder(WorkerJob order)
	{

		m_mutex.lock();
		m_JobRecieverList.push_back(order);
		m_mutex.unlock();
		m_cond_var.notify_one();
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
			//m_mutex.lock();
			std::unique_lock<std::mutex> locker(m_mutex);
			while(m_JobRecieverList.empty())
			{
				m_cond_var.wait(locker);
			}
			std::swap(m_JobRecieverList, m_jobProcessList);
			locker.unlock();
			while(!m_jobProcessList.empty())
			{
				auto job = m_jobProcessList.front();
				m_jobProcessList.pop_front();
				if(job.m_work)
				{
					job.m_work();
					if(job.m_onFinished)
					{
						m_mutex.lock();
						m_mainThreadCB1.push_back(job);
						m_mutex.unlock();
					}
				}
			}
		}
	}

	void WorkerThreadSystem::mainThreadUpdate()
	{
		m_mutex.lock();
		std::swap(m_mainThreadCB1, m_mainThreadCB2);
		m_mutex.unlock();
		if(!m_mainThreadCB2.empty())
		{
			for(const auto &cb : m_mainThreadCB2)
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
