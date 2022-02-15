#include "PTMGameTimeMgr.h"
#include "Scene/SceneMgr.h"
#include "EngineSrc/CubeEngine.h"
#include "EngineSrc/Event/EventMgr.h"
#include "PTMTile.h"
#include "PTMNation.h"
#include "2D/LabelNew.h"
namespace tzw
{



	PTMGameTimeMgr::PTMGameTimeMgr()
	{
	}

	void PTMGameTimeMgr::setStartDate(int year, int month, int day)
	{
		m_day = (year - 1) * 360 + month * 30 + day;
		m_dayInMonth = m_day % 30;
	}

	void PTMGameTimeMgr::pause()
	{
		m_isPause = true;
		m_pauseStateChangedCb(m_isPause);
	}

	void PTMGameTimeMgr::unpause()
	{
		m_isPause = false;
		m_pauseStateChangedCb(m_isPause);
	}

	void PTMGameTimeMgr::togglePause()
	{
		if(m_isPause)
		{
			unpause();
		}
		else
		{
			pause();
		}
	}

	float PTMGameTimeMgr::getSpeedRate()
	{
		float result = 2.0;
		switch(m_speedLevel)
		{
		case 0:
			result = 1.5;
			break;
		case 1:
			result = 1.0;
			break;
		case 2:
			result = 0.55;
			break;
		}
		return result;
	}

	void PTMGameTimeMgr::setOnDayTickCallback(std::function<void(uint32_t)> dayTickCB)
	{
		m_dayTickCb = dayTickCB;
	}

	void PTMGameTimeMgr::setOnMonthTickCallback(std::function<void()> monthTickCb)
	{
		m_monthTickCb = monthTickCb;
	}

	void PTMGameTimeMgr::setOnPauseStateChangedCallback(std::function<void(bool)> pauseStateChangedCb)
	{
		m_pauseStateChangedCb = pauseStateChangedCb;
	}

	void PTMGameTimeMgr::tick(float dt)
	{

		if(m_duration >= getSpeedRate())
		{
			m_day +=1;
			m_dayInMonth += 1;
			m_dayTickCb(m_day);
			if(m_dayInMonth >= 30)
			{
				m_dayInMonth = 0;
				m_monthTickCb();
			}
			m_duration = 0.0f;
		}
		m_duration += dt;
	}

}