#pragma once
#include "Engine/EngineDef.h"
#include <map>
#include "Timer.h"
#include <list>

namespace tzw {
	class TimerMgr
	{
	public:
		TZW_SINGLETON_DECL(TimerMgr);
		TimerMgr();
		void addTimer(Timer * timer);
		void handle(float dt);
	private:
		std::list<Timer *>m_timerList;
	};
} // namespace tzw