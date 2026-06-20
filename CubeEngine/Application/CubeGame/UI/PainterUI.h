#pragma once
#include "CubeGame/GameWorld.h"
#include "2D/GUISystem.h"
#include <functional>

namespace tzw
{
	class PainterUI : public IMGUIObject
	{
	public:
		// 通过 IMGUIObject 继承
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
