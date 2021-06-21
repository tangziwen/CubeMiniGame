#pragma once
#include "Engine/EngineDef.h"
#include <map>
#include "Timer.h"
#include <list>
#include <cstdint>


namespace tzw {
	struct Counter
	{
		Counter(int64_t newCounter):m_counter(newCounter)
		{
		}
		Counter():m_counter(0)
		{
		}
		static Counter now();
		static float deltaSecond(const Counter & start, const Counter & end);
		static int deltaMili(const Counter & start, const Counter & end);
		int64_t m_counter;
	};
	class TimerMgr : public Singleton<TimerMgr>
	{
	public:
		TimerMgr();
		void addTimer(Timer * timer);
		void handle(float dt);
	private:
		std::list<Timer *>m_timerList;
	};
} // namespace tzw