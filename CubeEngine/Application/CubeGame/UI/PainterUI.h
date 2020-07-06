#pragma once
#include "CubeGame/GameWorld.h"
#include "2D/GUISystem.h"
#include <functional>

namespace tzw
{
	class PainterUI : public IMGUIObject
	{
	public:
		// Í¨¹ý IMGUIObject ¼Ì³Ð
		void drawIMGUI(bool * isOpen) override;
		PainterUI();
		std::function<void (std::string)> m_onCreate;
		void prepare();
    private:
		ImVec4 customColorList[8];
		ImVec4 m_tmpColor;
		float * m_tmpModifedColor;
	};


}
