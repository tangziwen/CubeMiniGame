#pragma once
#include "CubeGame/GameWorld.h"
#include "2D/GUISystem.h"
#include <functional>

namespace tzw
{
	class NewWorldSettingUI : public IMGUIObject
	{
	public:
		// Í¨¹ý IMGUIObject ¼Ì³Ð
		void drawIMGUI(bool * isOpen) override;
		NewWorldSettingUI();
		WorldInfo getWorldInfo() const;
		std::function<void (WorldInfo)> m_onCreate;
    private:
		WorldInfo m_worldInfo;
	};


}
