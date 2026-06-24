#pragma once
#include "CubeGame/GameWorld.h"
#include "2D/IMGUISystem.h"
#include <functional>
namespace tzw
{
	class HudUI : public IMGUIObject
	{
	public:
		void drawIMGUI(bool * isOpen) override;
		HudUI();
		std::function<void (std::string)> m_onCreate;
		void prepare();
    private:
		ImVec4 customColorList[8];
		ImVec4 m_tmpColor;
		float * m_tmpModifedColor;
	};
}
