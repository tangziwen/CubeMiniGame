#pragma once
#include "Engine/EngineDef.h"
#include <map>
#include <functional>

namespace tzw {
	class Timer
	{
	public:
		Timer(std::function<void ()> func, float duration);
		void trigger();
		bool handle(float dt);
	private:
		std::function<void ()> m_cb;
		float m_duration;
		float m_currDuration;
		int m_frame;
	};
} // namespace tzw
#define TR(theString) TranslationMgr::shared()->getStr(theString)
#define TRC(theString) TR(theString).c_str()