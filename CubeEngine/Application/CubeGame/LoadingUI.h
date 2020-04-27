#pragma once

#include "2D/GUISystem.h"
#include "Game/DebugInfoPanel.h"
#include "Event/Event.h"
#include "Engine/EngineDef.h"
#include "GameNodeEditor.h"
#include "2D/Sprite.h"
#include "2D/GUIFileBrowser.h"
#include "2D/LabelNew.h"
#include "Utility/file/Data.h"
#include "OptionPanel.h"
#include "2D/GUIFrame.h"

namespace tzw {
class VehicleBroswer;
class LoadingUI : public IMGUIObject, public EventListener, public Singleton<LoadingUI>
{
public:
    LoadingUI();
	void show();
	void hide();
	void drawIMGUI() override;
private:
	bool m_isVisible;
	std::string m_tipsInfo;
public:
	std::string getTipsInfo() const;
	void setTipsInfo(const std::string& tipsInfo);
};

} // namespace tzw
