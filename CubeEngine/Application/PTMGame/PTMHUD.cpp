#include "PTMHUD.h"
#include "Scene/SceneMgr.h"
#include "EngineSrc/CubeEngine.h"
#include "EngineSrc/Event/EventMgr.h"
#include "PTMTile.h"
#include "PTMNation.h"
#include "2D/LabelNew.h"
#include <sstream>
#include <sstream>
#include "PTMPlayerController.h"
#include "PTMGameTimeMgr.h"
#include "PTMEventMgr.h"
namespace tzw
{



	PTMHUD::PTMHUD()
		:m_frame(nullptr), m_controller(nullptr)
	{
	}

	void PTMHUD::init()
	{
		m_frame = GUIFrame::create(vec2(Engine::shared()->windowWidth(), 50));
		m_frame->setLocalPiority(9999);
		g_GetCurrScene()->addNode(m_frame);

		m_nationName = LabelNew::create("unknown");
		m_gold = LabelNew::create("Gold:0");
		m_time = LabelNew::create("1440:1:1");
		m_frame->addChild(m_nationName);
		m_frame->addChild(m_gold);
		m_frame->addChild(m_time);
		m_nationName->setPos2D(20, 25);
		m_gold->setPos2D(200, 25);
		m_time->setPos2D(400, 25);
		
		PTMEventMgr::shared()->listen(PTMEventType::PLAYER_RESOURCE_CHANGED, 
			[this](PTMEventArgPack pack)
			{
				updateResource();
			}
		);
	}

	void PTMHUD::setController(PTMPlayerController* controller)
	{
		m_controller = controller;
		m_nationName->setString(controller->getControlledNation()->getName());
	}

	void PTMHUD::updateTimeOfDay()
	{
		uint32_t tmp =PTMGameTimeMgr::shared()->getCurrDate();
		uint32_t day_in_month = tmp %30;
		tmp /= 30;
		uint32_t month =  tmp % 12;
		tmp /= 12;
		uint32_t year = tmp;
		std::stringstream ss;
		ss<<"Date>> "<< (year + 1)<<":" <<(month + 1)<<":" << (day_in_month + 1);
		m_time->setString(ss.str());

		updateTimePauseState(PTMGameTimeMgr::shared()->isPause());
		updateResource();
	}

	void PTMHUD::updateTimePauseState(bool isPause)
	{
		if(isPause)
		{
			m_time->setColor(vec4(1.0f, 0.0f, 0.0f, 1.0f));
		}
		else
		{
			m_time->setColor(vec4(0.0f, 1.0f, 0.0f, 1.0f));
		}
	}

	void PTMHUD::updateMonthly()
	{
		updateResource();
	}

	void PTMHUD::updateResource()
	{
		std::stringstream ss;
		ss<<"Gold:"<<int(m_controller->getControlledNation()->getGold())<<" ADM:"<<m_controller->getControlledNation()->getAdminPoint();
		m_gold->setString(ss.str());
	}

	void PTMHUD::updateAll()
	{
		updateTimeOfDay();
		updateResource();
	}


}