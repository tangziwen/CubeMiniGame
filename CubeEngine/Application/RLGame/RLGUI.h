#pragma once
#include "2D/GUISystem.h"

namespace tzw
{
	class RLGUI: public Singleton<RLGUI>, public IMGUIObject
	{
	public:
		RLGUI();
		void init();
		void drawIMGUI() override;
		void drawMainMenu();
		void drawInGame();
		void drawAfterMath();
		void drawWin();
		void drawPurchaseMenu();
		void drawPrepareMenu();
	};
}
