#pragma once
#include "2D/GUISystem.h"

namespace tzw
{
class LabelNew;
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
		void drawPause();
		void drawPurchaseMenu();
		void drawPrepareMenu();
		void drawPerkSelection();
	private:
	LabelNew * m_centerTips = nullptr;
	};
}
