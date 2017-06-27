#pragma once

#include "Engine/EngineDef.h"
#include "SimGame/SimMainMenu.h"
#include "SimMap.h"
namespace tzw
{
	class SimGameSystem
	{
	TZW_SINGLETON_DECL(SimGameSystem)
	public:
		SimGameSystem();
		void init();
		void showMainMenu(bool isVisible);
		void startGame();
	private:
		SimMap * m_map;
		SimMainMenu * m_mainMenu;
	};

}