#pragma once
#include "Engine/EngineDef.h"
#include "2D/Sprite.h"
#include "EngineSrc/Event/Event.h"
namespace tzw
{
	class LabelNew;
	struct PTMTile;
	class PTMTown;
	class PTMNation;

	class PTMGameTimeMgr : public Singleton<PTMGameTimeMgr>
	{
	public:
		PTMGameTimeMgr();
		uint32_t getCurrDate() {return m_day;};
		void setStartDate(int year, int month, int day);
		bool isPause(){return m_isPause;};
		void pause();
		void unpause();
		void togglePause();
		float getSpeedRate();
		void setOnDayTickCallback(std::function<void(uint32_t)> dayTickCB);
		void setOnMonthTickCallback(std::function<void()> dayTickCB);
		void setOnPauseStateChangedCallback(std::function<void(bool)> dayTickCB);
		void tick(float dt);
	private:
		std::function<void(uint32_t)> m_dayTickCb;
		std::function<void()> m_monthTickCb;
		std::function<void(bool)> m_pauseStateChangedCb;
		int m_speedLevel = 0;
		bool m_isPause = false;
		uint32_t m_day = {0};
		float m_duration = {0.f};
		uint32_t m_dayInMonth = {0};
	};

}