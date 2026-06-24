#pragma once
#include "CubeGame/GameWorld.h"
#include "2D/IMGUISystem.h"
#include <functional>

namespace tzw
{
	class InventoryUI : public IMGUIObject
	{
	public:
		// 通过 IMGUIObject 继承
		void drawIMGUI(bool * isOpen) override;
		InventoryUI();
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
