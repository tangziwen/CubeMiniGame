#pragma once
#include "CubeGame/GameWorld.h"
#include "2D/GUISystem.h"
#include <functional>
#include <array>
namespace tzw
{
	class HudUI : public IMGUIObject
	{
	public:
		// Í¨¹ý IMGUIObject ¼Ì³Ð
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
		std::array<GameItem *, 9> m_itemSlot {nullptr};
		int m_currIndex = 0;
	};


}
