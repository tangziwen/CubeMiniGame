#include "SimGameSystem.h"
#include "Scene/SceneMgr.h"
#include "3D/Sky.h"

namespace tzw
{
	TZW_SINGLETON_IMPL(SimGameSystem);

	SimGameSystem::SimGameSystem()
	{
		//disable sky rendering
		Sky::shared()->setIsEnable(false);
	}

	void SimGameSystem::showMainMenu(bool isVisible)
	{
		m_mainMenu->setIsVisible(isVisible);
	}

	void SimGameSystem::startGame()
	{
		m_map = new SimMap();
		m_map->build();
	}

	void SimGameSystem::init()
	{
		m_mainMenu = new SimMainMenu();
		g_GetCurrScene()->addNode(m_mainMenu);
		showMainMenu(true);
	}
}