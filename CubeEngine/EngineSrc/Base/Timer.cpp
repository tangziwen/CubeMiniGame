#include "Timer.h"
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
	Timer::Timer(std::function<void()> func, float duration):m_cb(func), m_duration(duration),m_currDuration(0.0f),m_frame(0)
	{
	}

	void Timer::trigger()
	{
		if(m_cb)
		{
			m_cb();
		}
	}

	bool Timer::handle(float dt)
	{
		bool isOk = false;
		if(m_currDuration > m_duration)
		{
			trigger();
			tlog("execute in %d frames", m_frame);
			isOk = true;
		}
		m_currDuration += dt;
		m_frame++;
		return isOk;
	}
} // namespace tzw

