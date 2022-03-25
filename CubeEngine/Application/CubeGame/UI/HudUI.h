#pragma once
#include "CubeGame/GameWorld.h"
#include "2D/GUISystem.h"
#include <functional>

namespace tzw
{
	class HudUI : public IMGUIObject
	{
	public:
		// ͨ�� IMGUIObject �̳�
		void drawIMGUI(bool * isOpen) override;
		HudUI();
		std::function<void (std::string)> m_onCreate;
		void prepare();
    private:
		ImVec4 customColorList[8];
		ImVec4 m_tmpColor;
		float * m_tmpModifedColor;
		Texture * m_emptyIcon;
		bool m_isDragingInventory {false};
		GameItem * m_currentSelectItem;
	};


}
