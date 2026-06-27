#pragma once

#include "2D/IMGUISystem.h"
#include "CubeGame/Editor/EditorController.h"
#include "CubeGame/UI/InspectPanel.h"
#include "CubeGame/UI/RailEditorImGuiView.h"
#include "CubeGame/UI/TerrainEditorImGuiView.h"
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

		EditorController m_editorController;
		TerrainEditorImGuiView m_terrainView;
		RailEditorImGuiView m_railView;
		InspectPanel m_inspectPanel;
	};
}
