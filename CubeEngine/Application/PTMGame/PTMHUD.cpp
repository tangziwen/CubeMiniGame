#include "PTMHUD.h"
#include "Scene/SceneMgr.h"
#include "EngineSrc/CubeEngine.h"
#include "EngineSrc/Event/EventMgr.h"
#include "PTMTile.h"
#include "PTMNation.h"
#include "2D/LabelNew.h"
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
		m_gold = LabelNew::create("0");
		m_time = LabelNew::create("1440:1:1");
		m_frame->addChild(m_nationName);
		m_frame->addChild(m_gold);
		m_frame->addChild(m_time);
	}

	void PTMHUD::setController(PTMPlayerController* controller)
	{
		m_controller = controller;
	}

}