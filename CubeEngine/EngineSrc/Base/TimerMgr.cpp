#include "TimerMgr.h"
#include <assert.h>
#include "uuid4.h"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/prettywriter.h"
#include "Utility/file/Tfile.h"
#include "Utility/log/Log.h"

namespace tzw
{
	TZW_SINGLETON_IMPL(TimerMgr)
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
} // namespace tzw

