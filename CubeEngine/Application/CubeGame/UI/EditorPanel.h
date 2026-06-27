#pragma once

#include "2D/IMGUISystem.h"
#include "CubeGame/UI/EditorModalState.h"
#include "CubeGame/UI/InspectPanel.h"
#include "CubeGame/UI/RailEditorModule.h"
#include "CubeGame/UI/TerrainEditorModule.h"
#include "Event/Event.h"

#include <functional>
#include <string>

namespace tzw
{
	class EditorPanel : public IMGUIObject, public EventListener
	{
	public:
		void drawIMGUI(bool * isOpen) override;
		void onFrameUpdate(float delta) override;
		bool onMousePress(int button, vec2 pos) override;
		EditorPanel();
		std::function<void (std::string)> m_onCreate;
		void prepare();
	private:
		void drawCameraModeWindow(vec2 screenSize);
		void drawCameraModeCombo();
		void drawParameterPanel();
		void drawParameterWindow(vec2 screenSize);
		void switchToTab(int tabIndex, EditorModuleId moduleId);

		EditorModalState m_modalState;
		TerrainEditorModule m_terrainModule;
		RailEditorModule m_railModule;
		InspectPanel m_inspectPanel;
		int m_activeTab = 0;
	};
}
