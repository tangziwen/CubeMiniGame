#pragma once
#include "CubeGame/GameWorld.h"
#include "2D/GUISystem.h"
#include <functional>

namespace tzw
{
	class LoadWorldUI : public IMGUIObject
	{
	public:
		// Í¨¹ý IMGUIObject ¼Ì³Ð
		void drawIMGUI(bool * isOpen) override;
		LoadWorldUI();
		std::function<void (std::string)> m_onCreate;
		void prepare();
    private:
		char m_worldName[64];
		std::vector<std::filesystem::path> m_entryList;
	};


}
