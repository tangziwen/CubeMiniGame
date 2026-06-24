#pragma once
#include "CubeGame/GameWorld.h"
#include "2D/IMGUISystem.h"
#include <functional>

namespace tzw
{
	class NewWorldSettingUI : public IMGUIObject
	{
	public:
		// 通过 IMGUIObject 继承
		void drawIMGUI(bool * isOpen) override;
		NewWorldSettingUI();
		WorldInfo getWorldInfo() const;
		std::function<void (WorldInfo)> m_onCreate;
    private:
		WorldInfo m_worldInfo;
	};


}
