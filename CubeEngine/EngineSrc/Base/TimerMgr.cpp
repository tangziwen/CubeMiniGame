#include "TimerMgr.h"
#include <assert.h>
#include "uuid4.h"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/prettywriter.h"
#include "Utility/file/Tfile.h"
#include "Utility/log/Log.h"
#include <Windows.h>
namespace tzw
{


	struct CounterFrequenceInfo
	{
		double m_miliFactor;
		double m_secFactor;
		CounterFrequenceInfo() noexcept
		{
			LARGE_INTEGER freq;
			QueryPerformanceFrequency(&freq);
			m_miliFactor = 1000.0 / (double)freq.QuadPart;
			m_secFactor = 1.0 / (double)freq.QuadPart;
		}
	};
	static const CounterFrequenceInfo g_CounterFrequenceInfo;

	TimerMgr::TimerMgr()
	{

	}

	void TimerMgr::addTimer(Timer* timer)
	{
		m_timerList.push_back(timer);
	}

	void TimerMgr::handle(float dt)
	{
		for(auto i = m_timerList.begin(); i != m_timerList.end();)
		{
			auto isFinish = (*i)->handle(dt);
			if(isFinish)
			{
				i = m_timerList.erase(i);
			} else
			{
				i++;
			}
		}
	}

	Counter Counter::now()
	{
		LARGE_INTEGER nowStamp;
		QueryPerformanceCounter(&nowStamp);
		return Counter(nowStamp.QuadPart);
	}
	float Counter::deltaSecond(const Counter & start, const Counter & end)
	{
		return (end.m_counter - start.m_counter) * g_CounterFrequenceInfo.m_secFactor;
	}
	int Counter::deltaMili(const Counter & start, const Counter & end)
	{
		return (end.m_counter - start.m_counter) * g_CounterFrequenceInfo.m_miliFactor;
	}
} // namespace tzw

